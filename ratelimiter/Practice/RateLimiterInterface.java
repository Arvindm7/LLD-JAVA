package ratelimiter.Practice;

/**
 * The contract every rate limiter must follow (Strategy pattern).
 * Callers depend ONLY on this interface, so algorithms can be swapped freely.
 *
 * FIX: an interface cannot hold "private final" instance fields (that is illegal Java).
 * State like config belongs in the abstract class (RateLimiter) instead.
 */
public interface RateLimiterInterface {

    /**
     * @param userId who is making the request (could also be an IP / API key / tenant)
     * @return true  -> request allowed
     *         false -> request rejected (caller should return HTTP 429)
     */
    boolean allowRequest(String userId);
}
