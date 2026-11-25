# 02 – Coding Round Walkthrough (20 min)

Files: [`code/`](code/). Everything below explains **what each line does, why, and what to say**.

## 0. Time plan for 20 minutes

| Min | Do |
|-----|----|
| 0–3 | Clarify requirements, state assumptions, pick algorithm |
| 3–5 | Sketch interface + class (say it, write signatures) |
| 5–15 | Code token bucket (or fixed window first if nervous, then upgrade) |
| 15–18 | Walk through a test with fake clock; mention edge cases |
| 18–20 | Mention thread-safety, memory cleanup, and "how this extends to distributed" |

**Rule:** a *working simple* solution beats a half-finished fancy one. If stuck, write fixed window, then say the flaw and how token bucket fixes it.

---

## 1. The interface – [RateLimiter.java](code/RateLimiter.java)

```java
public interface RateLimiter {
    boolean allowRequest(String key);
}
```
- **Why an interface?** Strategy pattern + Open/Closed principle: add algorithms without changing callers.
- **Why `String key`?** Generic identity; the caller decides user/IP/tenant. Keeps limiter ignorant of HTTP.
- **Why `boolean`?** Simplest contract. Mention extension: return a `Decision{allowed, remaining, retryAfterMs}` for headers.

> Say: *"I'm keeping the interface tiny; key extraction and HTTP 429 mapping belong to a middleware layer."*

---

## 2. Token Bucket line by line – [TokenBucketRateLimiter.java](code/TokenBucketRateLimiter.java)

### State
```java
private static final class Bucket { double tokens; long lastRefillMillis; }
```
- Only **2 fields per user** → O(1) memory.
- `double` since refill is fractional: 3 tokens/sec after 100ms = 0.3 tokens. With `int` we'd lose precision and under-allow.
- `static final` nested class: no hidden reference to outer instance; encapsulated.

### Fields
```java
private final long capacity;          // max burst size
private final double refillPerSecond; // sustained rate
private final LongSupplier clockMillis; // injected time source
private final ConcurrentHashMap<String, Bucket> buckets
```
- `final` = immutable config → safely shared between threads without locks.
- **`LongSupplier` clock** = *dependency injection*. Tests use a fake clock (no `Thread.sleep`). Interviewers love this.
- **`ConcurrentHashMap`**: thread-safe map with lock striping, so lookups scale. `HashMap` would corrupt under concurrency; `synchronizedMap` is one global lock.

### Constructor validation
```java
if (capacity <= 0 || refillPerSecond < 0) throw new IllegalArgumentException(...)
```
Fail fast on bad config. (`refillPerSecond = 0` is allowed – used in tests for a fixed quota.)

### `allowRequest`
```java
long now = clockMillis.getAsLong();
```
Read the clock **once** so the whole decision uses one consistent time.

```java
Bucket bucket = buckets.computeIfAbsent(key, k -> new Bucket(capacity, now));
```
- **Atomic** get-or-create. The naive `if (!map.containsKey(k)) map.put(k, new …)` is a **race** (two threads create two buckets, one user gets double quota).
- New user starts with a **full** bucket (allows an initial burst – normal behaviour).

```java
synchronized (bucket) { ... }
```
- **Per-key lock**: user A and B never contend. A global lock would serialize every request in the system.
- Needed because "read tokens → refill → decrement" is a **compound check-then-act**; `volatile` or atomics on single fields wouldn't be enough.

```java
long elapsedMillis = Math.max(0, now - bucket.lastRefillMillis);
```
`max(0, …)` protects against clock going backwards (NTP adjustment). In production prefer `System.nanoTime()` (monotonic) for a single JVM.

```java
bucket.tokens = Math.min(capacity, bucket.tokens + (elapsedMillis / 1000.0) * refillPerSecond);
```
- **Lazy refill**: no scheduler thread refilling millions of buckets.
- `Math.min(capacity, …)`: an idle user must **not** accumulate unlimited tokens (test proves this).
- `/ 1000.0` – floating-point division on purpose (integer division would give 0).

```java
bucket.lastRefillMillis = now;
if (bucket.tokens >= 1) { bucket.tokens -= 1; return true; }
return false;
```
Update the timestamp **even on rejection**, otherwise the same elapsed time would be double-counted next call. Then consume.

### Complexity
Time **O(1)** per request. Space **O(#active keys)**.

---

## 3. Other algorithms (know the one-liners)

**Fixed window** – [FixedWindowRateLimiter.java](code/FixedWindowRateLimiter.java)
- `now - (now % windowMillis)` snaps time to the start of its window: with 10s windows, 12,345 → 10,000.
- New window? reset the counter. Flaw = boundary burst (test `fixedWindowBoundaryBurst` demonstrates 10 requests in 200 ms for a limit of 5).

**Sliding window log** – [SlidingWindowLogRateLimiter.java](code/SlidingWindowLogRateLimiter.java)
- `ArrayDeque` of timestamps; pop from head while expired; allow if size < limit.
- Only **accepted** requests are logged (otherwise attackers extend their own ban by spamming).
- Exact but O(limit) memory.

**Sliding window counter** – [SlidingWindowCounterRateLimiter.java](code/SlidingWindowCounterRateLimiter.java)
- Weighted estimate: `prev * (1 - fraction) + curr`.
- Rolling logic: if exactly one window passed, `prev = curr`; if more, `prev = 0` (stale).

---

## 4. Tests – [RateLimiterDemo.java](code/RateLimiterDemo.java)

| Test | What it proves |
|------|----------------|
| burst & refill | capacity, refill rate, cap on idle |
| per-key isolation | one noisy user doesn't hurt others |
| fixed window boundary | demonstrates the known flaw |
| sliding log boundary | fix for the flaw |
| sliding counter | approximation smooths it |
| **concurrency** | 50 threads × 20 requests racing; **exactly 10** allowed → proves thread-safety |

Frozen clock + `refill=0` makes the concurrency test deterministic.

---

## 5. Design principles checklist (name-drop them)

- ✅ **Strategy pattern** (interface + interchangeable algorithms)
- ✅ **Dependency injection** (clock) → testability
- ✅ **Single Responsibility**: limiter decides; it doesn't know HTTP
- ✅ **Encapsulation**: `Bucket` private
- ✅ **Fine-grained locking** + atomic `computeIfAbsent`
- ✅ **Immutability** of config
- ✅ **Lazy computation** (no background threads)
- ✅ **Fail-fast validation**

## 6. Known limitations to volunteer (shows maturity)

1. **Memory leak**: buckets for inactive users never removed. Fix: scheduled eviction of buckets idle > `capacity/refillRate` (their bucket is full again anyway, so dropping loses nothing), or Caffeine cache with `expireAfterAccess`.
2. **Per-JVM only** → solved in the distributed round (see [03](03_distributed_design.md)).
3. **Single global rule**: extend with `RateLimitConfig` per key/plan (a `Map<plan, Config>` or factory).
4. **Clock**: use `System.nanoTime()` for monotonic time.
5. **Cost-based**: change `allowRequest(key, permits)` to consume N tokens.

## 7. Easy extension: per-plan configuration

```java
RateLimiter forPlan(String plan) {            // Factory + cache
    return limiters.computeIfAbsent(plan, p -> switch (p) {
        case "FREE"       -> new TokenBucketRateLimiter(10, 1);
        case "PREMIUM"    -> new TokenBucketRateLimiter(100, 20);
        default           -> new TokenBucketRateLimiter(5, 0.5);
    });
}
```

## 8. Memorize this skeleton (fastest to write under pressure)

```java
boolean allowRequest(String key) {
    long now = clock.getAsLong();
    Bucket b = map.computeIfAbsent(key, k -> new Bucket(capacity, now));
    synchronized (b) {
        b.tokens = Math.min(capacity, b.tokens + (now - b.last) / 1000.0 * rate);
        b.last = now;
        if (b.tokens < 1) return false;
        b.tokens--;
        return true;
    }
}
```
