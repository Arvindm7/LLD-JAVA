package ratelimiter.Practice;

import java.util.HashMap;
import java.util.Map;

/**
 * FIXED WINDOW COUNTER.
 * Time is cut into slots (e.g. each 60s). Count requests per user in the current slot;
 * reset when a new slot begins. Weakness: up to 2x burst at the slot boundary.
 */
public class FixedWindow extends RateLimiter {

    private final int limit;
    private final long windowMs;
    private final Map<String, Long> windowId = new HashMap<>();   // which slot the user is counting in
    private final Map<String, Integer> count = new HashMap<>();   // requests so far in that slot

    public FixedWindow(RateLimitConfig rateLimitConfig) {
        super(rateLimitConfig);
        this.limit = rateLimitConfig.getMaxRequests();
        this.windowMs = rateLimitConfig.getWindowInSeconds() * 1000L;   // L -> long, avoids int overflow
    }

    @Override
    public synchronized boolean allowRequest(String userId) {
        long slot = System.currentTimeMillis() / windowMs;   // integer division = slot number

        // New user or new slot -> start counting from zero.
        if (!windowId.containsKey(userId) || windowId.get(userId) != slot) {
            windowId.put(userId, slot);
            count.put(userId, 0);
        }

        if (count.get(userId) < limit) {
            count.put(userId, count.get(userId) + 1);
            return true;
        }
        return false;
    }
}
