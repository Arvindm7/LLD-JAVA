package ratelimiter.Practice;

/**
 * Base class for all algorithms. Holds the shared config so every algorithm
 * does not repeat it (Template: common state here, algorithm in subclasses).
 */
public abstract class RateLimiter implements RateLimiterInterface {
    private final RateLimitConfig rateLimitConfig;

    public RateLimiter(RateLimitConfig rateLimitConfig) {
        this.rateLimitConfig = rateLimitConfig;
    }

    // FIX: the field is private, so subclasses (TokenBucket, FixedWindow...) need a getter to read it.
    protected RateLimitConfig getConfig() {
        return rateLimitConfig;
    }
}
