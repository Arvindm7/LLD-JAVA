package ratelimiter.Practice;

/** Which algorithm to build; used by RateLimiterFactory. */
public enum RateLimitType {
    FIXEDWINDOW,
    TOKENBUCKET,
    SLIDINGWINDOWLOG,
    SLIDINGWINDOWCOUNTER
}
