package ratelimiter.Practice;

/** Simple value object: who is calling, and which plan they are on. Immutable. */
public class User {
    // FIX: was int, but limiters take a String userId. Using String everywhere avoids conversions.
    private final String userId;
    private final UserTier userTier;

    public User(String userId, UserTier userTier) {
        this.userId = userId;
        this.userTier = userTier;
    }

    public String getUserId() {
        return userId;
    }

    public UserTier getUserTier() {
        return userTier;
    }
}
