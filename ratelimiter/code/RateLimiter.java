/**
 * The contract. Strategy pattern: callers depend on this interface only,
 * so we can swap Token Bucket / Sliding Window without touching callers.
 */
public interface RateLimiter {

    /**
     * @param key identity being limited (userId, apiKey, tenantId, IP ...)
     * @return true if the request is allowed, false if it must be rejected (HTTP 429)
     */
    boolean allowRequest(String key);
}
