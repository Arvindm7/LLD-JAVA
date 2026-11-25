# 03 – Distributed Rate Limiter (System Design, 15 min)

## 1. Why single-node breaks
With N app servers behind a load balancer, each has its own in-memory counters:
- Limit 100/min, 10 servers → a user can effectively send up to **1000/min**.
- Sticky sessions would fix it but cause hot spots and break on server failure/scale events.

**Need: shared state, or coordinated state.**

## 2. High-level architecture

```
Client → CDN/WAF (coarse IP limit) → API Gateway / Rate Limit Middleware
                                          │  1. extract key (tenant/user/API key)
                                          │  2. load rule (local cached config)
                                          ▼
                                   Redis Cluster  (atomic Lua: check + update)
                                          │
                       allowed ──► Backend service        rejected ──► 429 + Retry-After

Rules/Config service ──(push/poll, cached locally)──► middleware
Metrics (allowed/blocked per rule) ──► monitoring/alerts
```

Components:
1. **Rate-limit middleware** (in gateway or sidecar) – stateless, scales horizontally.
2. **Redis cluster** – holds counters/buckets, sub-ms ops, TTL auto-cleans idle keys.
3. **Rules store** – DB/config service; middleware caches rules in memory and refreshes (e.g. every 30 s).
4. **Observability** – dashboards for 429 rate per tenant.

## 3. Storing state – options

| Option | Pros | Cons |
|--------|------|------|
| **Central Redis** ⭐ | Exact, simple, fast (~0.5 ms in-region) | Extra hop; Redis is a dependency |
| Sticky routing by key (consistent hash to a node) | No external store | Hot keys, rebalancing pain |
| Local counters + periodic sync (gossip) | No hop, very fast | Approximate; can overshoot by (nodes × sync interval) |

Default answer: **Redis**. Mention local+sync as the optimisation for very high QPS tolerating small error.

## 4. The core problem: race conditions

Naive:
```
count = GET key
if count < limit: INCR key
```
Two servers read `99` simultaneously → both allow → 101. **Check-then-act must be atomic.**

Solutions:
- **`INCR` + `EXPIRE`** for fixed window (INCR is atomic) – simple but fixed-window flaw.
- **Lua script** (Redis runs it atomically, single-threaded) ⭐ – use for token bucket / sliding window.
- `MULTI/EXEC` or `WATCH` – optimistic, retries under contention; worse than Lua.
- Redis **sorted sets** for sliding log (`ZREMRANGEBYSCORE`, `ZCARD`, `ZADD`) inside Lua.

### Token bucket in Redis Lua (know this by heart)

```lua
-- KEYS[1] = bucket key ; ARGV = capacity, refillPerSec, nowMillis, requested
local data = redis.call('HMGET', KEYS[1], 'tokens', 'ts')
local capacity, rate, now, req = tonumber(ARGV[1]), tonumber(ARGV[2]), tonumber(ARGV[3]), tonumber(ARGV[4])
local tokens = tonumber(data[1]) or capacity
local ts     = tonumber(data[2]) or now

tokens = math.min(capacity, tokens + math.max(0, now - ts) / 1000 * rate)   -- lazy refill
local allowed = 0
if tokens >= req then tokens = tokens - req; allowed = 1 end

redis.call('HSET', KEYS[1], 'tokens', tokens, 'ts', now)
redis.call('PEXPIRE', KEYS[1], math.ceil(capacity / rate * 1000) * 2)       -- auto-cleanup idle keys
return allowed
```
Points to say:
- **One round trip, atomic** – this is *the same algorithm as the Java code*, just running inside Redis.
- **TTL** = time to refill fully (×2 margin) → idle users vanish → solves the memory-leak problem.
- **Time source**: pass `now` from the client **or** use Redis `TIME`. Using Redis `TIME` avoids clock skew between app servers (in Redis ≥5 with effects replication, calling TIME in scripts is OK).

Java side: Jedis/Lettuce `EVALSHA` with the script (load once, call by SHA).

## 5. Scaling Redis

- **Shard by key** with Redis Cluster (hash slots) – each key lives on one shard, so atomicity is preserved (never split one user's counter across shards!).
- **Capacity estimate** (shows rigor): 10 M active users × ~100 B/key ≈ **1 GB** → fits trivially. QPS: 1 M req/s ÷ ~100k ops/s per shard ≈ 10+ shards.
- **Replication**: primary + replica per shard; replica failover via Sentinel/Cluster. Async replication means a failover may lose a few updates → tiny over-allowance (acceptable).
- **Hot keys** (one huge tenant): split into sub-keys `tenant:{id}:{0..k}` each with limit/k, or use local pre-aggregation / batching.

## 6. Latency optimisations
- Same-region Redis, connection pooling, pipelining.
- **Local token pre-fetch / batching**: each node grabs e.g. 10 tokens at a time from Redis, serves locally. Fewer hops, error bounded by batch size.
- Tiny **local cache of "blocked until"** – once rejected with a known reset time, reject locally without hitting Redis (great vs. abusive clients).

## 7. Failure handling – a must-discuss topic

**If Redis is down:**
| Choice | When |
|--------|------|
| **Fail open** (allow) | Default for most APIs – availability > strict limits; limiter shouldn't be a SPOF |
| Fail closed (reject) | Security-sensitive: login/OTP/payments |
| **Degrade to local in-memory limiter** ⭐ | Best of both: approximate limit = limit/N per node |

Also: **timeouts** (e.g. 10–20 ms) + **circuit breaker** on the Redis call so slowness doesn't cascade into the request path.

## 8. Multi-region
- Global exact limit needs cross-region sync = high latency → don't.
- Options: **per-region limits** (limit split by traffic share), or async replication of counters (eventual), accept slight overshoot.
- Say: *"I'd trade precision for latency across regions; exactness within a region."*

## 9. Rules/configuration
- Rule = `{match: endpoint/plan/tenant, algorithm, capacity, refillRate | limit/window}`.
- Stored centrally, versioned, cached in middleware; hot reload without redeploy.
- Hierarchy of limits: **global → tenant → user → endpoint**; request must pass all (evaluate cheapest/strictest first).
- Redis key design: `rl:{tenantId}:{userId}:{ruleId}` – use **hash tags** `{tenantId}` if multi-key ops need the same slot.

## 10. Response & client friendliness
- `429 Too Many Requests` + `Retry-After`, `X-RateLimit-Remaining/Reset`.
- Clients should use exponential backoff + jitter (mention to show full-picture thinking).
- Don't count rejected requests (or you punish retries).

## 11. Monitoring
- Metrics: allowed vs. blocked per rule/tenant, Redis latency p99, fail-open count.
- Alerts: sudden block-rate spike (attack or misconfigured limit), Redis errors.
- **Shadow/dry-run mode** for new rules: log would-be blocks without enforcing → safe rollout.

## 12. Trade-off summary (final 1 minute)

| Decision | Chosen | Alternative | Why |
|----------|--------|-------------|-----|
| Algorithm | Token bucket | Sliding window counter | Burst tolerance, O(1) |
| State | Redis cluster | Local + sync | Accuracy; local if QPS huge |
| Atomicity | Lua script | WATCH/MULTI | Single RTT, no retries |
| Failure | Fail open + local fallback | Fail closed | Availability |
| Multi-region | Per-region limits | Global sync | Latency |
