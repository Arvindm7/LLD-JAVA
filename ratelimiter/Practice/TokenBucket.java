package ratelimiter.Practice;

import java.util.HashMap;
import java.util.Map;

/**
 * TOKEN BUCKET.
 * Bucket holds up to `capacity` tokens; refills at `refillPerSec`; each request takes 1 token.
 * With config (10, 60): capacity = 10 (burst), refill = 10/60 token per second (= 10 per minute).
 */
public class TokenBucket extends RateLimiter {

    private final int capacity;          // max burst
    private final double refillPerSec;   // sustained rate

    // Per-user state (no inner class: two parallel maps).
    // FIX: tokens must be Double, not Integer - refill is fractional (e.g. 0.3 token).
    private final Map<String, Double> userTokens = new HashMap<>();
    private final Map<String, Long> lastRefillTime = new HashMap<>();   // millis

    public TokenBucket(RateLimitConfig rateLimitConfig) {
        super(rateLimitConfig);                       // pass config up to the base class
        this.capacity = rateLimitConfig.getMaxRequests();
        this.refillPerSec = (double) rateLimitConfig.getMaxRequests() / rateLimitConfig.getWindowInSeconds();
    }

    // synchronized: check-refill-consume must be atomic when many threads call this.
    @Override
    public synchronized boolean allowRequest(String userId) {
        long now = System.currentTimeMillis();        // read the clock once

        // First request from this user -> start with a FULL bucket.
        if (!userTokens.containsKey(userId)) {
            userTokens.put(userId, (double) capacity);
            lastRefillTime.put(userId, now);
        }

        // Lazy refill: add tokens for the time passed, but never above capacity.
        double elapsedSec = (now - lastRefillTime.get(userId)) / 1000.0;   // 1000.0 -> decimal division
        double tokens = Math.min(capacity, userTokens.get(userId) + elapsedSec * refillPerSec);
        lastRefillTime.put(userId, now);              // update even on reject, else time is double counted

        if (tokens >= 1) {
            userTokens.put(userId, tokens - 1);       // consume one token
            return true;
        }
        userTokens.put(userId, tokens);               // keep the refilled fractional value
        return false;
    }
}
