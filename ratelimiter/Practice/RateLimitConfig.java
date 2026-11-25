package ratelimiter.Practice;

/**
 * Immutable rule: "at most maxRequests per windowInSeconds".
 * Example: new RateLimitConfig(10, 60) = 10 requests per minute.
 *
 * Renamed fields for clarity: maxResult -> maxRequests, windowsInSecond -> windowInSeconds.
 */
public class RateLimitConfig {
    private final int maxRequests;       // how many requests are allowed
    private final int windowInSeconds;   // ...in how many seconds

    public RateLimitConfig(int maxRequests, int windowInSeconds) {
        // Fail fast on bad config instead of failing mysteriously later.
        if (maxRequests <= 0) {
            throw new IllegalArgumentException("maxRequests must be > 0");
        }
        if (windowInSeconds <= 0) {
            throw new IllegalArgumentException("windowInSeconds must be > 0");
        }
        this.maxRequests = maxRequests;
        this.windowInSeconds = windowInSeconds;
    }

    public int getMaxRequests() {
        return maxRequests;
    }

    public int getWindowInSeconds() {
        return windowInSeconds;
    }
}
