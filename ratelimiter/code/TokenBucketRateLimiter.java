import java.util.concurrent.ConcurrentHashMap;
import java.util.function.LongSupplier;

/**
 * TOKEN BUCKET  (the recommended answer in the interview).
 *
 * Each key owns a bucket: holds up to `capacity` tokens, refilled at `refillPerSecond`.
 * A request consumes 1 token. No token -> rejected.
 * Refill is LAZY: computed from elapsed time when a request arrives (no timer threads).
 */
public class TokenBucketRateLimiter implements RateLimiter {

    /** Mutable per-key state. Guarded by synchronized(bucket). */
    private static final class Bucket {
        double tokens;          // double because refill is fractional (e.g. 0.3 tokens)
        long lastRefillMillis;  // when we last updated `tokens`

        Bucket(double tokens, long lastRefillMillis) {
            this.tokens = tokens;
            this.lastRefillMillis = lastRefillMillis;
        }
    }

    private final long capacity;
    private final double refillPerSecond;
    private final LongSupplier clockMillis; // injected clock => deterministic tests
    private final ConcurrentHashMap<String, Bucket> buckets = new ConcurrentHashMap<>();

    public TokenBucketRateLimiter(long capacity, double refillPerSecond, LongSupplier clockMillis) {
        if (capacity <= 0 || refillPerSecond < 0) {
            throw new IllegalArgumentException("capacity must be > 0 and refillPerSecond >= 0");
        }
        this.capacity = capacity;
        this.refillPerSecond = refillPerSecond;
        this.clockMillis = clockMillis;
    }

    /** Convenience constructor using the real clock. */
    public TokenBucketRateLimiter(long capacity, double refillPerSecond) {
        this(capacity, refillPerSecond, System::currentTimeMillis);
    }

    @Override
    public boolean allowRequest(String key) {
        long now = clockMillis.getAsLong();

        // Atomic "create if missing". New users start with a FULL bucket.
        Bucket bucket = buckets.computeIfAbsent(key, k -> new Bucket(capacity, now));

        // Lock ONLY this key's bucket -> different users never block each other.
        synchronized (bucket) {
            // 1. Lazy refill: add tokens for the time that has passed.
            long elapsedMillis = Math.max(0, now - bucket.lastRefillMillis); // max(): guards clock skew
            bucket.tokens = Math.min(capacity, bucket.tokens + (elapsedMillis / 1000.0) * refillPerSecond);
            bucket.lastRefillMillis = now;

            // 2. Try to consume.
            if (bucket.tokens >= 1) {
                bucket.tokens -= 1;
                return true;
            }
            return false;
        }
    }
}
