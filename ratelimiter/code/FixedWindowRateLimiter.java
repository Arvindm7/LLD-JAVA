import java.util.concurrent.ConcurrentHashMap;
import java.util.function.LongSupplier;

/**
 * FIXED WINDOW COUNTER. Simplest algorithm; has the boundary-burst flaw
 * (up to 2x limit around a window edge). Good to code first if you are short on time.
 */
public class FixedWindowRateLimiter implements RateLimiter {

    private static final class Window {
        long windowStart;
        int count;
    }

    private final int limit;
    private final long windowMillis;
    private final LongSupplier clockMillis;
    private final ConcurrentHashMap<String, Window> windows = new ConcurrentHashMap<>();

    public FixedWindowRateLimiter(int limit, long windowMillis, LongSupplier clockMillis) {
        this.limit = limit;
        this.windowMillis = windowMillis;
        this.clockMillis = clockMillis;
    }

    @Override
    public boolean allowRequest(String key) {
        long now = clockMillis.getAsLong();
        long currentWindowStart = now - (now % windowMillis); // align to window boundary

        Window w = windows.computeIfAbsent(key, k -> new Window());
        synchronized (w) {
            if (w.windowStart != currentWindowStart) { // new window -> reset counter
                w.windowStart = currentWindowStart;
                w.count = 0;
            }
            if (w.count < limit) {
                w.count++;
                return true;
            }
            return false;
        }
    }
}
