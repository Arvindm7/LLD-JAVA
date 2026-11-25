# 05 – Q&A Cheat Sheet + 1-Page Revision

## Follow-up questions with model answers

**1. Why token bucket over sliding window?**
Burst tolerance (real clients are bursty), O(1) memory, simple lazy refill. Sliding window counter is my choice if burst-free smoothness is required.

**2. How is it thread-safe?**
`ConcurrentHashMap.computeIfAbsent` for atomic bucket creation; `synchronized` per bucket so check-refill-consume is atomic; different keys don't contend.

**3. Why not `AtomicInteger`/`AtomicLong`?**
Operation touches two fields (tokens, timestamp) and is conditional – needs compound atomicity. Could do CAS loop on an immutable state object (`AtomicReference<State>`) – lock-free alternative.

**4. Memory grows forever – how to fix?**
TTL/eviction of idle keys (bucket idle ≥ capacity/refill is full again → safe to drop). Caffeine `expireAfterAccess`, or Redis TTL in distributed version.

**5. Clock issues?**
Use monotonic `nanoTime` in one JVM. In distributed, use Redis `TIME` as the single source or tolerate small skew. Guard negative elapsed with `max(0, …)`.

**6. How do you test?**
Inject a fake clock; assert burst, refill, cap, per-key isolation; concurrency test with frozen clock expecting exactly `capacity` successes.

**7. How to make it distributed?**
Shared Redis, atomic Lua script implementing the same token bucket, one key per user/tenant, sharded by key. See [03](03_distributed_design.md).

**8. Redis goes down?**
Fail open with timeout+circuit breaker, optionally local fallback limiter at limit/N; fail closed only for security endpoints (login).

**9. Race condition in naive Redis implementation?**
GET then SET: two servers both read 99 → both allow. Fix with atomic Lua (or INCR for fixed window).

**10. Hot key (one giant tenant)?**
Split key into k sub-buckets with limit/k, route randomly; or local batching of tokens; or dedicated shard.

**11. Multiple limits (10/s and 1000/hr)?**
Compose: a `CompositeRateLimiter` holds a list; allow only if all allow. Beware consuming tokens from the first limiter if the second rejects – check-all-then-consume (or refund) in a single Lua script.

**12. Different limits per user plan / endpoint?**
Config store → rule lookup by (plan, endpoint) → factory returns a limiter. Key = `userId:endpointGroup`.

**13. Cost-based limiting?**
`allowRequest(key, permits)` – consume N tokens. Heavy endpoints cost more.

**14. Should rejected requests count?**
Generally no for token bucket (nothing consumed). For abuse protection, an optional penalty/ban window.

**15. Where to place limiter – client, server, gateway?**
Gateway/sidecar: central enforcement, protects all services, language-agnostic. Client-side limiting is voluntary (not trustworthy).

**16. Rate limiting vs. throttling vs. load shedding?**
Rate limiting = per-client quota. Throttling = slow down/delay. Load shedding = system-wide protection by dropping requests when overloaded regardless of client.

**17. How do you roll out a new limit safely?**
Shadow/dry-run mode, canary a small tenant percentage, monitor 429 rate, instant rollback via config flag.

**18. Strict vs approximate – which for you?**
Approximate is fine for protection use-cases (1–2% error); strict for billing/quotas – then use Redis atomic with exact algorithm.

**19. Leaky bucket vs token bucket?**
Leaky: constant outflow, queues requests, smooths traffic (shaping). Token: permits bursts, rejects instead of queuing.

**20. Latency impact?**
~0.5–1 ms same-region Redis; reduce with connection pooling, pipelining, token batching, local blocked-cache.

---

## 1-PAGE REVISION (read 10 min before the interview)

**Coding (20 min)**
1. Clarify → key, rule, thread-safe, in-memory.
2. Choose **token bucket**; name alternatives + flaws.
3. `interface RateLimiter { boolean allowRequest(String key); }`
4. `ConcurrentHashMap<String, Bucket>`, `computeIfAbsent`, `synchronized(bucket)`.
5. Lazy refill: `tokens = min(cap, tokens + elapsedSec*rate)`; update `last`; consume 1.
6. Inject clock; test burst/refill/cap/isolation/concurrency.
7. Volunteer: eviction, per-JVM limit → Redis.

**System design (15 min)**
1. Requirements + numbers (QPS, keys, latency, fail mode).
2. Problem: N servers × limit. Need shared atomic state.
3. Gateway middleware → **Redis cluster + Lua token bucket** → backend; rules service; metrics.
4. Atomicity: Lua. Cleanup: TTL. Sharding: hash by key.
5. Failure: fail-open + circuit breaker + local fallback. Hot keys: sub-keys.
6. Multi-region: per-region limits. Rollout: dry-run. Headers: 429 + Retry-After.
7. Trade-offs table.

**Numbers to remember**: 1 GB for 10 M keys · ~100 k ops/s/shard · <1 ms Redis RTT · fail-open default.

**Algorithms in one line each**
- Fixed window: counter/window, 2× boundary burst.
- Sliding log: timestamps, exact, O(n) memory.
- Sliding counter: `prev*(1-f)+curr`, O(1), approximate.
- Token bucket: tokens+lazy refill, bursts OK, O(1). ⭐
- Leaky bucket: queue + constant drain.

## Self-test (answer aloud without notes)
1. Why is `containsKey` + `put` wrong? 2. Why `double` tokens? 3. Why update `last` on rejection? 4. Why per-key lock? 5. How does Lua fix the race? 6. What happens when Redis dies? 7. Why cap at capacity? 8. How to evict idle keys safely?

## Next up
Once comfortable, we'll do the next two in the same format: **event-processing system** and **reliable URL fetching service**.
