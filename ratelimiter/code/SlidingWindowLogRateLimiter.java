import java.util.ArrayDeque;
import java.util.Deque;
import java.util.concurrent.ConcurrentHashMap;
import java.util.function.LongSupplier;

/**
 * SLIDING WINDOW LOG. Exact, but stores up to `limit` timestamps per key => O(limit) memory.
 */
public class SlidingWindowLogRateLimiter implements RateLimiter {

    private final int limit;
    private final long windowMillis;
    private final LongSupplier clockMillis;
    private final ConcurrentHashMap<String, Deque<Long>> logs = new ConcurrentHashMap<>();

    public SlidingWindowLogRateLimiter(int limit, long windowMillis, LongSupplier clockMillis) {
        this.limit = limit;
        this.windowMillis = windowMillis;
        this.clockMillis = clockMillis;
    }

    @Override
    public boolean allowRequest(String key) {
        long now = clockMillis.getAsLong();
        Deque<Long> log = logs.computeIfAbsent(key, k -> new ArrayDeque<>());

        synchronized (log) {
            // Evict timestamps that fell out of the window (oldest are at the head).
            while (!log.isEmpty() && log.peekFirst() <= now - windowMillis) {
                log.pollFirst();
            }
            if (log.size() < limit) {
                log.addLast(now); // only ACCEPTED requests are logged
                return true;
            }
            return false;
        }
    }
}
