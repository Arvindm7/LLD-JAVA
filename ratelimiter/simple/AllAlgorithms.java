import java.util.*;

/**
 * ALL 5 ALGORITHMS, SIMPLE VERSION (no inner classes, no clock injection class).
 * Time is passed in as `now` (ms) so the demo is deterministic.
 * In real use: limiter.allowRequest(key, System.currentTimeMillis());
 * Add `synchronized` to allowRequest for thread safety.
 *
 *  Algorithm        | Memory/key | Weakness
 *  -----------------|------------|-----------------------------------
 *  Fixed window     | O(1)       | 2x burst at window edge
 *  Sliding log      | O(limit)   | memory heavy, but exact
 *  Sliding counter  | O(1)       | approximate
 *  Token bucket     | O(1)       | allows bursts (usually a feature)
 *  Leaky bucket     | O(1)       | no bursts beyond capacity, smooth rate
 */
interface RateLimiter {
    boolean allowRequest(String key, long now);
}

// 1. FIXED WINDOW: count requests in the current time slot; reset when slot changes.
class FixedWindow implements RateLimiter {
    int limit;
    long windowMs;
    Map<String, Long> windowId = new HashMap<>();   // which slot this user is counting in
    Map<String, Integer> count = new HashMap<>();   // requests so far in that slot

    FixedWindow(int limit, long windowMs) { this.limit = limit; this.windowMs = windowMs; }

    public synchronized boolean allowRequest(String key, long now) {
        long slot = now / windowMs;                         // e.g. 10s windows: 0-9999 -> slot 0
        if (!windowId.containsKey(key) || windowId.get(key) != slot) {
            windowId.put(key, slot);                        // new slot -> reset counter
            count.put(key, 0);
        }
        if (count.get(key) < limit) {
            count.put(key, count.get(key) + 1);
            return true;
        }
        return false;
    }
}

// 2. SLIDING WINDOW LOG: remember the time of every accepted request.
class SlidingWindowLog implements RateLimiter {
    int limit;
    long windowMs;
    Map<String, Deque<Long>> logs = new HashMap<>();

    SlidingWindowLog(int limit, long windowMs) { this.limit = limit; this.windowMs = windowMs; }

    public synchronized boolean allowRequest(String key, long now) {
        Deque<Long> q = logs.computeIfAbsent(key, k -> new ArrayDeque<>());
        while (!q.isEmpty() && q.peekFirst() <= now - windowMs) {
            q.pollFirst();                                  // drop requests older than the window
        }
        if (q.size() < limit) {
            q.addLast(now);                                 // only log accepted requests
            return true;
        }
        return false;
    }
}

// 3. SLIDING WINDOW COUNTER: fixed-window counts of this and previous slot, weighted.
//    estimate = prev * (1 - fractionOfCurrentSlotPassed) + curr
class SlidingWindowCounter implements RateLimiter {
    int limit;
    long windowMs;
    Map<String, Long> windowId = new HashMap<>();
    Map<String, Integer> curr = new HashMap<>();
    Map<String, Integer> prev = new HashMap<>();

    SlidingWindowCounter(int limit, long windowMs) { this.limit = limit; this.windowMs = windowMs; }

    public synchronized boolean allowRequest(String key, long now) {
        long slot = now / windowMs;
        if (!windowId.containsKey(key)) {                   // first time
            windowId.put(key, slot);
            curr.put(key, 0);
            prev.put(key, 0);
        }
        if (windowId.get(key) != slot) {                    // moved to a new slot
            // if exactly next slot, old current becomes previous; if gap was bigger, previous is stale
            prev.put(key, windowId.get(key) == slot - 1 ? curr.get(key) : 0);
            curr.put(key, 0);
            windowId.put(key, slot);
        }
        double fraction = (now % windowMs) / (double) windowMs;
        double estimate = prev.get(key) * (1 - fraction) + curr.get(key);
        if (estimate < limit) {
            curr.put(key, curr.get(key) + 1);
            return true;
        }
        return false;
    }
}

// 4. TOKEN BUCKET: tokens refill over time, each request takes one.
class TokenBucket implements RateLimiter {
    int capacity;
    double refillPerSec;
    Map<String, Double> tokens = new HashMap<>();
    Map<String, Long> last = new HashMap<>();

    TokenBucket(int capacity, double refillPerSec) { this.capacity = capacity; this.refillPerSec = refillPerSec; }

    public synchronized boolean allowRequest(String key, long now) {
        if (!tokens.containsKey(key)) {                     // new user -> full bucket
            tokens.put(key, (double) capacity);
            last.put(key, now);
        }
        double elapsedSec = (now - last.get(key)) / 1000.0;
        double current = Math.min(capacity, tokens.get(key) + elapsedSec * refillPerSec);
        last.put(key, now);
        if (current >= 1) {
            tokens.put(key, current - 1);
            return true;
        }
        tokens.put(key, current);
        return false;
    }
}

// 5. LEAKY BUCKET (as a meter): each request adds 1 unit of water, water leaks out at a fixed rate.
//    Bucket full -> reject. Mirror image of token bucket.
class LeakyBucket implements RateLimiter {
    int capacity;
    double leakPerSec;
    Map<String, Double> water = new HashMap<>();
    Map<String, Long> last = new HashMap<>();

    LeakyBucket(int capacity, double leakPerSec) { this.capacity = capacity; this.leakPerSec = leakPerSec; }

    public synchronized boolean allowRequest(String key, long now) {
        if (!water.containsKey(key)) {                      // new user -> empty bucket
            water.put(key, 0.0);
            last.put(key, now);
        }
        double elapsedSec = (now - last.get(key)) / 1000.0;
        double level = Math.max(0, water.get(key) - elapsedSec * leakPerSec);
        last.put(key, now);
        if (level + 1 <= capacity) {
            water.put(key, level + 1);
            return true;
        }
        water.put(key, level);
        return false;
    }
}

public class AllAlgorithms {

    // send n requests for "u" at time t, return how many were allowed
    static int burst(RateLimiter rl, int n, long t) {
        int allowed = 0;
        for (int i = 0; i < n; i++) if (rl.allowRequest("u", t)) allowed++;
        return allowed;
    }

    public static void main(String[] args) {
        // Limit: 5 requests per 10s. Send 5 right before the window edge, 5 right after.
        RateLimiter[] windows = { new FixedWindow(5, 10000), new SlidingWindowLog(5, 10000),
                                  new SlidingWindowCounter(5, 10000) };
        String[] names = { "FixedWindow", "SlidingLog", "SlidingCounter" };
        for (int i = 0; i < windows.length; i++) {
            int a = burst(windows[i], 5, 9900) + burst(windows[i], 5, 10100);
            System.out.println(names[i] + ": allowed " + a + " of 10 across the edge");
            // expected: Fixed=10 (flaw), Log=5, Counter=6 (approx)
        }

        // Burst of 7 at t=0 with capacity 5 and rate 0.5/s, then 5 more at t=2s (1 unit freed)
        RateLimiter tb = new TokenBucket(5, 0.5);
        System.out.println("TokenBucket: " + burst(tb, 7, 0) + " then " + burst(tb, 5, 2000)); // 5 then 1
        RateLimiter lb = new LeakyBucket(5, 0.5);
        System.out.println("LeakyBucket: " + burst(lb, 7, 0) + " then " + burst(lb, 5, 2000)); // 5 then 1
    }
}
