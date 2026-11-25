package ratelimiter.Practice;

/**
 * Factory pattern: callers say WHICH algorithm they want via an enum and never use `new` themselves.
 * Adding an algorithm = add enum value + one branch here.
 */
public class RateLimiterFactory {

    public static RateLimiter createRateLimiter(RateLimitType rateLimitType, RateLimitConfig rateLimitConfig) {
        // FIX: previously referenced a non-existent class FixedWindowRateLimiter and a non-existent
        // config.getCapacity()/getRefillRate(). Every algorithm now takes just the config.
        switch (rateLimitType) {
            case FIXEDWINDOW:
                return new FixedWindow(rateLimitConfig);
            case TOKENBUCKET:
                return new TokenBucket(rateLimitConfig);
            case SLIDINGWINDOWLOG:
                return new SlidingWindowLog(rateLimitConfig);
            case SLIDINGWINDOWCOUNTER:
                return new SlidingWindowCounter(rateLimitConfig);
            default:
                throw new IllegalArgumentException("Invalid RateLimitType");
        }
    }
}
