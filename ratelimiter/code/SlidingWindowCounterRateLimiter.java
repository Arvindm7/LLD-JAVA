import java.util.concurrent.ConcurrentHashMap;
import java.util.function.LongSupplier;

/**
 * SLIDING WINDOW COUNTER (hybrid). O(1) memory, approximates a true sliding window:
 *   estimate = prevCount * (1 - elapsedFractionOfCurrentWindow) + currCount
 */
public class SlidingWindowCounterRateLimiter implements RateLimiter {

    private static final class State {
        long currStart;
        int currCount;
        int prevCount;
    }

    private final int limit;
    private final long windowMillis;
    private final LongSupplier clockMillis;
    private final ConcurrentHashMap<String, State> states = new ConcurrentHashMap<>();

    public SlidingWindowCounterRateLimiter(int limit, long windowMillis, LongSupplier clockMillis) {
        this.limit = limit;
        this.windowMillis = windowMillis;
        this.clockMillis = clockMillis;
    }

    @Override
    public boolean allowRequest(String key) {
        long now = clockMillis.getAsLong();
        long windowStart = now - (now % windowMillis);

        State s = states.computeIfAbsent(key, k -> new State());
        synchronized (s) {
            // Roll windows forward if needed.
            if (windowStart != s.currStart) {
                // Exactly one window later -> current becomes previous; otherwise prev is stale (0).
                s.prevCount = (windowStart - s.currStart == windowMillis) ? s.currCount : 0;
                s.currCount = 0;
                s.currStart = windowStart;
            }
            double elapsedFraction = (now - s.currStart) / (double) windowMillis;
            double estimate = s.prevCount * (1.0 - elapsedFraction) + s.currCount;

            if (estimate < limit) {
                s.currCount++;
                return true;
            }
            return false;
        }
    }
}
