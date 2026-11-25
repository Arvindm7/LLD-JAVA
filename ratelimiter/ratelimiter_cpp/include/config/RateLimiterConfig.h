// ============================================================================
// FILE: include/config/RateLimiterConfig.h
// PURPOSE: Immutable configuration — "at most maxRequests per windowInSeconds".
//
// WHY SEPARATE FROM ALGORITHMS?
//   - All 5 algorithms need the same config.
//   - Keeps config (data) separate from logic (algorithms).
//   - Prevents circular includes.
//
// WHY HEADER-ONLY?
//   Config is a tiny class with only inline methods (getters + constructor).
//   Inline methods inside the class body won't cause linker errors.
//   If the constructor had complex validation logic, we'd move it to .cpp.
//
// EXAMPLE: RateLimiterConfig(10, 60) → "10 requests per 60 seconds"
// ============================================================================

#ifndef RATE_LIMITER_CONFIG_H
#define RATE_LIMITER_CONFIG_H

#include <stdexcept>    // For std::invalid_argument

class RateLimiterConfig {
private:
    int maxRequests_;       // Maximum requests allowed in the window
    int windowInSeconds_;   // Duration of the time window (seconds)

public:
    // Constructor with validation
    RateLimiterConfig(int maxRequests, int windowInSeconds)
        : maxRequests_(maxRequests),        // Initializer list: idiomatic C++
          windowInSeconds_(windowInSeconds) {
        // Fail fast: reject bad config immediately
        if (maxRequests_ <= 0) {
            throw std::invalid_argument("maxRequests must be > 0");
        }
        if (windowInSeconds_ <= 0) {
            throw std::invalid_argument("windowInSeconds must be > 0");
        }
    }

    // Getters: all const, all inline (defined in class body)
    int getMaxRequests() const { return maxRequests_; }
    int getWindowInSeconds() const { return windowInSeconds_; }

    // Convenience: many algorithms need window in milliseconds
    long long getWindowInMs() const { return windowInSeconds_ * 1000LL; }
    // 1000LL → force long long multiplication to avoid int overflow
};

#endif // RATE_LIMITER_CONFIG_H

