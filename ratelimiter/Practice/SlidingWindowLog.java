package ratelimiter.Practice;

import java.util.ArrayDeque;
import java.util.Deque;
import java.util.HashMap;
import java.util.Map;

/**
 * SLIDING WINDOW LOG.
 * Remember the timestamp of every ACCEPTED request. Before deciding, drop timestamps
 * older than the window. Exact, but memory is O(limit) per user.
 */
public class SlidingWindowLog extends RateLimiter {

    private final int limit;
    private final long windowMs;
    private final Map<String, Deque<Long>> logs = new HashMap<>();   // user -> accepted timestamps (oldest first)

    public SlidingWindowLog(RateLimitConfig rateLimitConfig) {
        super(rateLimitConfig);
        this.limit = rateLimitConfig.getMaxRequests();
        this.windowMs = rateLimitConfig.getWindowInSeconds() * 1000L;
    }

    @Override
    public synchronized boolean allowRequest(String userId) {
        long now = System.currentTimeMillis();
        Deque<Long> log = logs.computeIfAbsent(userId, k -> new ArrayDeque<>());

        // Evict requests that are no longer inside [now - window, now].
        while (!log.isEmpty() && log.peekFirst() <= now - windowMs) {
            log.pollFirst();
        }

        if (log.size() < limit) {
            log.addLast(now);          // only accepted requests are logged
            return true;
        }
        return false;
    }
}
