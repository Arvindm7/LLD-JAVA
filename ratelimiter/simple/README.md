# Simple 20-Minute Version

Code: [SimpleRateLimiter.java](SimpleRateLimiter.java) — ~25 lines of logic. **This is what you actually write.** The big version in `../code/` is for understanding only.

## Plan (20 min)

| Min | Do | Say |
|-----|----|-----|
| 0–2 | Clarify | "Per-key limit, in-memory, thread-safe, reject with false. OK?" |
| 2–3 | Choose | "Token bucket: O(1) memory per key, allows bursts, lazy refill so no background thread." |
| 3–12 | Code | Class → `Bucket` → `allowRequest` (below) |
| 12–16 | Run `main` | Walk through: burst 3 → T T T F F, bob independent, refill after 1s |
| 16–20 | Verbal extras | The 4 points at the bottom |

## Write in this order (4 steps)

1. **Fields**: `capacity`, `refillPerSec`, `Map<String, Bucket>`.
   *Say:* "Each user has a bucket: tokens + last refill time."
2. **Get-or-create bucket** (`computeIfAbsent`), new user starts full.
3. **Refill**: `tokens = min(capacity, tokens + elapsedSec * rate)`, then `lastRefill = now`.
   *Say:* "Lazy refill – computed on request, so no timers."
4. **Consume**: `if tokens >= 1 → tokens--, return true; else false`.

`synchronized` on the method = thread-safe. *Say:* "Simplest correct choice; I'd go per-key locks if this became a bottleneck."

## Why each piece (one line each)
- `double tokens` – refill is fractional (0.3 token after 300 ms).
- `Math.min(capacity, …)` – idle users can't hoard tokens.
- Update `lastRefill` even on reject – avoids double-counting elapsed time.
- Map keyed by string – limiter doesn't care if it's user, IP or tenant.

## If you're running out of time: fixed window fallback (even shorter)

```java
synchronized boolean allow(String key) {
    long window = System.currentTimeMillis() / windowMs;
    long[] s = map.computeIfAbsent(key, k -> new long[]{window, 0}); // {window, count}
    if (s[0] != window) { s[0] = window; s[1] = 0; }
    return s[1]++ < limit;   // note: counts rejects too; fine for a quick version
}
```
Say its flaw: "2× burst across window boundary; token bucket fixes that."

## Say these 4 things at the end (this is your "depth", costs 1 minute)
1. **Memory**: "Idle buckets never removed – I'd evict after inactivity or use a cache with TTL."
2. **Concurrency**: "Global lock → ConcurrentHashMap + per-bucket lock."
3. **Testability**: "Inject a clock instead of `System.currentTimeMillis()`."
4. **Distributed**: "State moves to Redis; the same refill logic runs in an atomic Lua script." → leads into the design round.

## Run
```powershell
cd "d:\DSA and prep for placement\LLD\rate limiter\simple"
javac SimpleRateLimiter.java; java SimpleRateLimiter
```
