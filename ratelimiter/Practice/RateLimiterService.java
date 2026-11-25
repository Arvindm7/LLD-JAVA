package ratelimiter.Practice;

import java.util.HashMap;
import java.util.Map;

/**
 * Entry point used by the application: picks the right limiter for the user's tier.
 * FREE users -> token bucket, 10 req/min.  PREMIUM users -> fixed window, 100 req/min.
 */
public class RateLimiterService {

    // One limiter instance per tier. Each limiter keeps its own per-user state inside.
    private final Map<UserTier, RateLimiter> rateLimiters = new HashMap<>();

    // FIX: the old constructor took a Map parameter named `ratelimiter` and put into THAT (shadowing),
    // so the class field stayed empty and every call threw "No limiter configured".
    public RateLimiterService() {
        rateLimiters.put(
            UserTier.FREE,
            RateLimiterFactory.createRateLimiter(
                RateLimitType.TOKENBUCKET,
                new RateLimitConfig(10, 60)      // 10 req/min
            )
        );

        rateLimiters.put(
            UserTier.PREMIUM,
            RateLimiterFactory.createRateLimiter(
                RateLimitType.FIXEDWINDOW,
                new RateLimitConfig(100, 60)     // 100 req/min
            )
        );
    }

    public boolean allowRequest(User user) {
        RateLimiter limiter = rateLimiters.get(user.getUserTier());   // look up by tier
        if (limiter == null) {
            throw new IllegalArgumentException("No limiter configured for tier " + user.getUserTier());
        }
        return limiter.allowRequest(user.getUserId());                // delegate; service knows no algorithm
    }
}
