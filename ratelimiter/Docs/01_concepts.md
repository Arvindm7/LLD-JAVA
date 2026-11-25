# 01 – Rate Limiter Concepts

## 1. Why rate limit?
1. **Protect availability** – stop a buggy client/DDoS from taking the service down.
2. **Fairness** – no noisy neighbour starves other tenants.
3. **Cost control** – downstream DB/3rd-party calls cost money.
4. **Monetisation** – free vs. paid tiers have different quotas.

## 2. Requirements to CLARIFY first (always ask!)

**Functional**
- What do we limit by? user / IP / API key / tenant / endpoint?
- What's the rule? e.g. "100 requests per minute per user".
- Multiple rules per key? (e.g. 10/sec **and** 1000/hour)
- What happens when exceeded? Reject (429), queue, or throttle/delay?

**Non-functional**
- Latency overhead: must be tiny (**< 1–5 ms**), it's on the hot path.
- Accuracy: strict or approximate OK?
- Scale: single server or fleet? QPS? number of keys?
- Availability: if limiter dies, do we **fail open** (allow) or **fail closed** (reject)?

## 3. Where does the limiter live?

| Placement | Pros | Cons |
|-----------|------|------|
| In the app (library/middleware) | No network hop | Each instance sees only its own traffic |
| **API Gateway / sidecar** (most common) | Central, language-agnostic, protects services | Gateway must be scaled |
| Separate rate-limit service | Reusable, rich rules | Extra network hop |

## 4. The four algorithms

### 4.1 Fixed Window Counter
Divide time into windows (e.g. each minute). Counter per key per window; reject if `count >= limit`.
- ✅ Simple, O(1) memory, O(1) time.
- ❌ **Boundary burst**: 100 reqs at 12:00:59 + 100 at 12:01:00 = 200 reqs in 2 seconds, yet both "windows" are legal.

### 4.2 Sliding Window Log
Store a timestamp of every accepted request; drop timestamps older than the window; allow if `size < limit`.
- ✅ Perfectly accurate.
- ❌ **O(limit) memory per key** (10k limit × millions of users = disaster).

### 4.3 Sliding Window Counter (hybrid)
Keep counts for current and previous window; estimate:
`estimate = prev × (1 − fractionElapsedInCurrent) + curr`
- ✅ O(1) memory, smooths the boundary burst. Cloudflare reports ~0.003% error.
- ❌ Approximation (assumes requests in prev window were evenly spread).

### 4.4 Token Bucket ⭐ (the default answer)
Bucket holds up to `capacity` tokens; refills at `refillRate` tokens/sec. Each request takes 1 token; no token → reject.
- ✅ Allows **bursts** up to capacity but enforces long-term average = refill rate.
- ✅ O(1) memory (just `tokens` + `lastRefillTime`), no background thread needed (**lazy refill**).
- ✅ Used by AWS, Stripe, etc.
- ❌ Two params to tune.

### Bonus: Leaky Bucket
Requests enter a queue processed at a constant rate. Smooths output (good for traffic shaping), but bursts get queued/delayed and old requests can starve new ones.

## 5. Comparison cheat table

| Algorithm | Memory/key | Accuracy | Burst handling | Best for |
|-----------|-----------|----------|----------------|----------|
| Fixed window | O(1) | Low (2× burst at edge) | Bad | Quick & dirty, coarse quotas |
| Sliding log | O(limit) | Exact | Exact | Low limits, strict audit |
| Sliding counter | O(1) | ~99.99% | Good | High-scale APIs |
| **Token bucket** | O(1) | Exact for its model | **Allows controlled bursts** | General purpose default |

> **Interview line:** "I'll go with token bucket – O(1) memory, supports bursts which real clients need, and lazy refill means no timer threads. I'll mention sliding-window-counter as the alternative if they need smoother, burst-free limits."

## 6. Key design principles you'll apply in code
- **Strategy pattern** – `RateLimiter` interface; algorithms are swappable.
- **Dependency injection of the clock** – testability (fake time instead of `Thread.sleep`).
- **Fine-grained locking** – lock per key, not global.
- **Lazy evaluation** – compute refill on request, not via a scheduler.
- **Separation of concerns** – algorithm vs. key resolution vs. config vs. HTTP response.

## 7. HTTP contract (small detail that impresses)
- Status: **429 Too Many Requests**
- Headers: `Retry-After: <seconds>`, `X-RateLimit-Limit`, `X-RateLimit-Remaining`, `X-RateLimit-Reset`
