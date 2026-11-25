# 04 – How to Communicate (scripts you can adapt)

Core principle: **think out loud, structure it, and drive the conversation.** Interviewers grade *communication and trade-off reasoning* as much as the answer.

## A. Coding round (20 min)

### 1. Open (0–3 min) – say this
> "Before coding, let me clarify. Are we limiting per user, per IP or per tenant? What's the rule – e.g. N requests per time unit? Single process, thread-safe? Should rejected requests be dropped or queued? I'll assume: per-key limit, in-memory, multi-threaded, reject with false."

Then pick the algorithm:
> "I'll use **token bucket**: O(1) memory per key, allows controlled bursts, and I can refill lazily without background threads. Alternatives are fixed window – simple but 2× burst at edges – and sliding window log – exact but O(n) memory."

### 2. Design out loud (3–5 min)
> "An interface `RateLimiter.allowRequest(key)` so algorithms are swappable. Per-key state in a `ConcurrentHashMap`. Each bucket has `tokens` and `lastRefill`. I'll inject the clock so I can test without sleeping."

### 3. While coding – narrate *why*, not *what*
- ❌ "Now I write a synchronized block."
- ✅ "I lock per-bucket so different users never contend; the check-and-decrement must be atomic."
- ✅ "`computeIfAbsent` avoids the check-then-put race creating two buckets."
- ✅ "Capping at capacity so idle users can't hoard tokens."

### 4. Test (15–18 min)
> "Let me walk an example: capacity 5, refill 1/sec. Five instant requests pass, sixth fails; after 2 s two more pass. Also a concurrency test: 50 threads, frozen clock – exactly `capacity` must succeed."

### 5. Close (18–20 min) – proactively list limitations
> "Limitations: memory for idle keys – I'd evict or use TTL; this is per-JVM so across servers it needs shared state, which I'd solve with Redis and an atomic Lua script – happy to go into that."

This **sets up the system design round** and shows you're ahead.

### If you get stuck
- Say what's blocking you, propose an option, ask which they prefer.
- Fall back to fixed window → mention its flaw → describe the upgrade verbally.
- Never go silent for >30 s.

---

## B. System design round (15 min) – timeline

| Min | Step | Say |
|-----|------|-----|
| 0–2 | Requirements & scale | Ask: QPS, users, limits per what, strict vs approximate, latency budget, fail-open or closed? State assumptions with numbers (e.g. 1 M rps, 10 M active keys, <5 ms overhead). |
| 2–4 | Why distributed is hard | "Per-node counters let a user exceed N × limit. We need shared state with atomic updates." |
| 4–7 | High-level design | Draw: Client → Gateway/middleware → Redis cluster → backend; rules service; metrics. |
| 7–10 | Deep dive: atomicity | Race on read-then-write → Lua script on Redis → show the algorithm. |
| 10–13 | Scale + failure | Sharding by key, replicas, hot keys, fail-open/local fallback, circuit breaker, multi-region. |
| 13–15 | Trade-offs & wrap-up | Summary table; mention monitoring, dry-run rollout; invite questions. |

### Draw this on the whiteboard

```
 Client ─► [Gateway + RL middleware] ─► Backend
                  │  ▲
        Lua check │  │ rules (cached)
                  ▼  │
          [Redis Cluster]   [Rules service]
                  │
              Metrics ─► Alerts
```

### Phrases that signal seniority
- "The trade-off here is accuracy versus latency…"
- "I'd start simple – central Redis – and optimise only if numbers demand it."
- "The limiter must not become a single point of failure, so I'd fail open with a local fallback."
- "Check-then-act is a race; I need atomicity, so Lua."
- "For tenant fairness – the noisy-neighbour problem – I'd key on tenant ID with hierarchical limits."
- "I'd roll out new rules in shadow mode first."

### Things to avoid
- Jumping into Redis before clarifying requirements.
- Listing every algorithm without choosing one.
- Ignoring failure modes.
- Saying "it depends" without then deciding.

## C. Back-of-envelope (do it in 30 seconds)
- 10 M active users × (key ~50 B + hash ~50 B) ≈ **1 GB** → single cluster is fine.
- 1 M rps; Redis ≈ 100 k ops/s/shard → ~10–20 shards (with headroom).
- Added latency: ~0.5–1 ms in-region.

## D. Atlassian-specific angles
- **Multi-tenant**: key by cloud site/tenant; protect from noisy neighbours.
- **Tiered by product/plan/endpoint**; cost-based (heavy JQL search costs more tokens).
- **Customer-friendly**: Retry-After headers, clear docs, dry-run.
- Ownership mindset: observability, safe rollout, runbooks.
