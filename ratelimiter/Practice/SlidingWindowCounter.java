package ratelimiter.Practice;

import java.util.HashMap;
import java.util.Map;

/**
 * SLIDING WINDOW COUNTER (hybrid).
 * Keep counts for the current and previous slot, and weigh the previous one by how much of it
 * still overlaps the sliding window:
 *     estimate = prev * (1 - fractionOfCurrentSlotPassed) + curr
 * O(1) memory, smooths the fixed-window boundary burst, but is an approximation.
 */
public class SlidingWindowCounter extends RateLimiter {

    private final int limit;
    private final long windowMs;
    private final Map<String, Long> windowId = new HashMap<>();   // current slot number per user
    private final Map<String, Integer> curr = new HashMap<>();    // count in current slot
    private final Map<String, Integer> prev = new HashMap<>();    // count in previous slot

    public SlidingWindowCounter(RateLimitConfig rateLimitConfig) {
        super(rateLimitConfig);
        this.limit = rateLimitConfig.getMaxRequests();
        this.windowMs = rateLimitConfig.getWindowInSeconds() * 1000L;
    }

    @Override
    public synchronized boolean allowRequest(String userId) {
        long now = System.currentTimeMillis();
        long slot = now / windowMs;

        // First time: start all counters at zero.
        if (!windowId.containsKey(userId)) {
            windowId.put(userId, slot);
            curr.put(userId, 0);
            prev.put(userId, 0);
        }

        // Moved into a new slot: roll the counters forward.
        if (windowId.get(userId) != slot) {
            // Exactly the next slot -> old current becomes previous. Bigger gap -> previous is stale (0).
            prev.put(userId, windowId.get(userId) == slot - 1 ? curr.get(userId) : 0);
            curr.put(userId, 0);
            windowId.put(userId, slot);
        }

        double fraction = (now % windowMs) / (double) windowMs;          // how far into this slot (0..1)
        double estimate = prev.get(userId) * (1 - fraction) + curr.get(userId);

        if (estimate < limit) {
            curr.put(userId, curr.get(userId) + 1);
            return true;
        }
        return false;
    }
}
