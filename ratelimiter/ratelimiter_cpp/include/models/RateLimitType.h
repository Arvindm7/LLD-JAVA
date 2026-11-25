// ============================================================================
// FILE: include/models/RateLimitType.h
// PURPOSE: Enum listing all available rate limiting algorithms.
//
// USED BY: RateLimiterFactory to create the right algorithm via switch-case.
// HEADER-ONLY: enums have no implementation, no .cpp needed.
// ============================================================================

#ifndef RATE_LIMIT_TYPE_H
#define RATE_LIMIT_TYPE_H

enum class RateLimitType {
    FIXED_WINDOW,           // Algorithm 1: Fixed Window Counter
    SLIDING_WINDOW_LOG,     // Algorithm 2: Sliding Window Log
    SLIDING_WINDOW_COUNTER, // Algorithm 3: Sliding Window Counter (hybrid)
    TOKEN_BUCKET,           // Algorithm 4: Token Bucket ⭐ (default)
    LEAKY_BUCKET            // Algorithm 5: Leaky Bucket
};

#endif // RATE_LIMIT_TYPE_H

