import java.util.HashMap;
import java.util.Map;

/**
 * 20-MINUTE VERSION. Token bucket, no inner class.
 * State per user is kept in two parallel maps: tokens and lastRefill.
 */
public class SimpleRateLimiter {

    private final int capacity;          // max burst
    private final double refillPerSec;   // sustained rate
    private final Map<String, Double> tokens = new HashMap<>();     // key -> tokens left
    private final Map<String, Long> lastRefill = new HashMap<>();   // key -> last update time (ms)

    public SimpleRateLimiter(int capacity, double refillPerSec) {
        this.capacity = capacity;
        this.refillPerSec = refillPerSec;
    }

    // One lock for everything: simplest correct thread-safety.
    // Say: "I'd use ConcurrentHashMap + per-key lock for scale."
    public synchronized boolean allowRequest(String key) {
        long now = System.currentTimeMillis();

        // first time we see this user -> full bucket
        if (!tokens.containsKey(key)) {
            tokens.put(key, (double) capacity);
            lastRefill.put(key, now);
        }

        // refill lazily based on elapsed time, never above capacity
        double elapsedSec = (now - lastRefill.get(key)) / 1000.0;
        double current = Math.min(capacity, tokens.get(key) + elapsedSec * refillPerSec);
        lastRefill.put(key, now);

        if (current >= 1) {
            tokens.put(key, current - 1);
            return true;
        }
        tokens.put(key, current);
        return false;
    }

    public static void main(String[] args) throws InterruptedException {
        SimpleRateLimiter rl = new SimpleRateLimiter(3, 1); // burst 3, 1 token/sec

        for (int i = 1; i <= 5; i++) {
            System.out.println("req " + i + " -> " + rl.allowRequest("alice")); // T T T F F
        }
        System.out.println("bob   -> " + rl.allowRequest("bob"));   // true (separate bucket)
        Thread.sleep(1100);
        System.out.println("alice after 1.1s -> " + rl.allowRequest("alice")); // true (1 token refilled)
    }
}
