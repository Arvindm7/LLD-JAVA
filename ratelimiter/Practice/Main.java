package ratelimiter.Practice;

/**
 * Quick demo. Compile and run from the LLD folder (parent of `ratelimiter`):
 *   javac ratelimiter/Practice/*.java
 *   java ratelimiter.Practice.Main
 */
public class Main {
    public static void main(String[] args) {
        RateLimiterService service = new RateLimiterService();

        User free = new User("alice", UserTier.FREE);        // limit: 10 / min
        User premium = new User("bob", UserTier.PREMIUM);    // limit: 100 / min

        int allowed = 0;
        for (int i = 0; i < 15; i++) if (service.allowRequest(free)) allowed++;
        System.out.println("FREE    sent 15, allowed " + allowed);      // expect 10

        allowed = 0;
        for (int i = 0; i < 120; i++) if (service.allowRequest(premium)) allowed++;
        System.out.println("PREMIUM sent 120, allowed " + allowed);     // expect 100 (or less if a slot rolled over)

        // Other algorithms through the factory: 3 per 60s.
        for (RateLimitType type : RateLimitType.values()) {
            RateLimiter rl = RateLimiterFactory.createRateLimiter(type, new RateLimitConfig(3, 60));
            int ok = 0;
            for (int i = 0; i < 6; i++) if (rl.allowRequest("u1")) ok++;
            System.out.println(type + " sent 6, allowed " + ok);        // expect 3
        }
    }
}
