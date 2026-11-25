// ============================================================================
// FILE: include/models/UserTier.h
// PURPOSE: Enum for user subscription tiers.
//
// WHY HEADER-ONLY?
//   Enums have NO implementation — they're just named constants.
//   No .cpp file needed. This is the one case header-only is correct.
//
// WHY enum class (not plain enum)?
//   1. Scoped: UserTier::FREE, not just FREE (avoids name clashes)
//   2. Type-safe: no implicit conversion to int
// ============================================================================

#ifndef USER_TIER_H
#define USER_TIER_H

enum class UserTier {
    FREE,       // Free tier: lower rate limits
    PREMIUM     // Paid tier: higher rate limits
};

#endif // USER_TIER_H

