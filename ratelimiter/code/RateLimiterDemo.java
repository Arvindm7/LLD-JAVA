import java.util.concurrent.CountDownLatch;
import java.util.concurrent.ExecutorService;
import java.util.concurrent.Executors;
import java.util.concurrent.atomic.AtomicInteger;
import java.util.function.LongSupplier;

/** Plain-Java tests (no JUnit needed). Uses a FAKE clock so tests are instant & deterministic. */
public class RateLimiterDemo {

    /** Controllable clock. */
    static class FakeClock implements LongSupplier {
        volatile long now = 1_000_000; // arbitrary start, aligned to 1s/10s boundaries
        @Override public long getAsLong() { return now; }
        void advance(long ms) { now += ms; }
    }

    static int failures = 0;

    static void check(String name, boolean cond) {
        System.out.println((cond ? "PASS  " : "FAIL  ") + name);
        if (!cond) failures++;
    }

    public static void main(String[] args) throws Exception {
        tokenBucketBurstAndRefill();
        tokenBucketPerKeyIsolation();
        fixedWindowBoundaryBurst();
        slidingLogNoBoundaryBurst();
        slidingCounterSmoothing();
        concurrencyExactlyLimitAllowed();
        System.out.println(failures == 0 ? "\nALL TESTS PASSED" : "\n" + failures + " FAILED");
    }

    static void tokenBucketBurstAndRefill() {
        FakeClock clock = new FakeClock();
        RateLimiter rl = new TokenBucketRateLimiter(5, 1.0, clock); // burst 5, 1 token/sec

        int allowed = 0;
        for (int i = 0; i < 10; i++) if (rl.allowRequest("u1")) allowed++;
        check("TokenBucket: burst of 10 allows exactly capacity (5)", allowed == 5);

        clock.advance(2000); // 2 tokens refilled
        allowed = 0;
        for (int i = 0; i < 5; i++) if (rl.allowRequest("u1")) allowed++;
        check("TokenBucket: after 2s, 2 more allowed", allowed == 2);

        clock.advance(60_000); // long idle -> must cap at capacity, not 60 tokens
        allowed = 0;
        for (int i = 0; i < 20; i++) if (rl.allowRequest("u1")) allowed++;
        check("TokenBucket: tokens capped at capacity after long idle", allowed == 5);
    }

    static void tokenBucketPerKeyIsolation() {
        FakeClock clock = new FakeClock();
        RateLimiter rl = new TokenBucketRateLimiter(1, 0, clock);
        check("TokenBucket: user A first request allowed", rl.allowRequest("A"));
        check("TokenBucket: user A second request blocked", !rl.allowRequest("A"));
        check("TokenBucket: user B unaffected by A", rl.allowRequest("B"));
    }

    static void fixedWindowBoundaryBurst() {
        FakeClock clock = new FakeClock(); // now % 10_000 == 0
        RateLimiter rl = new FixedWindowRateLimiter(5, 10_000, clock);
        clock.advance(9_900); // end of window
        int allowed = 0;
        for (int i = 0; i < 5; i++) if (rl.allowRequest("u")) allowed++;
        clock.advance(200);   // just into next window
        for (int i = 0; i < 5; i++) if (rl.allowRequest("u")) allowed++;
        check("FixedWindow: BOUNDARY FLAW - 10 allowed within 200ms (limit is 5/10s)", allowed == 10);
    }

    static void slidingLogNoBoundaryBurst() {
        FakeClock clock = new FakeClock();
        RateLimiter rl = new SlidingWindowLogRateLimiter(5, 10_000, clock);
        clock.advance(9_900);
        int allowed = 0;
        for (int i = 0; i < 5; i++) if (rl.allowRequest("u")) allowed++;
        clock.advance(200);
        for (int i = 0; i < 5; i++) if (rl.allowRequest("u")) allowed++;
        check("SlidingLog: only 5 allowed across the boundary", allowed == 5);

        clock.advance(10_000); // everything expired
        check("SlidingLog: allowed again after window passes", rl.allowRequest("u"));
    }

    static void slidingCounterSmoothing() {
        FakeClock clock = new FakeClock();
        RateLimiter rl = new SlidingWindowCounterRateLimiter(10, 10_000, clock);
        clock.advance(9_000);
        int allowed = 0;
        for (int i = 0; i < 10; i++) if (rl.allowRequest("u")) allowed++;
        clock.advance(1_000); // 10% into next window: estimate = 10*0.9 = 9 -> only 1 more
        int next = 0;
        for (int i = 0; i < 10; i++) if (rl.allowRequest("u")) next++;
        check("SlidingCounter: boundary burst smoothed (only ~1 extra, not 10)", allowed == 10 && next <= 2);
    }

    static void concurrencyExactlyLimitAllowed() throws Exception {
        FakeClock clock = new FakeClock(); // frozen time, no refill
        RateLimiter rl = new TokenBucketRateLimiter(10, 0, clock);
        int threads = 50;
        ExecutorService pool = Executors.newFixedThreadPool(threads);
        CountDownLatch start = new CountDownLatch(1);
        CountDownLatch done = new CountDownLatch(threads);
        AtomicInteger allowed = new AtomicInteger();
        for (int i = 0; i < threads; i++) {
            pool.submit(() -> {
                try {
                    start.await();
                    for (int j = 0; j < 20; j++) if (rl.allowRequest("shared")) allowed.incrementAndGet();
                } catch (InterruptedException ignored) {
                } finally { done.countDown(); }
            });
        }
        start.countDown();
        done.await();
        pool.shutdown();
        check("Concurrency: 1000 racing requests, exactly 10 allowed (got " + allowed.get() + ")", allowed.get() == 10);
    }
}
