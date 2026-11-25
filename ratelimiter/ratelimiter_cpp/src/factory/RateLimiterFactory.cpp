// ============================================================================
// FILE: src/factory/RateLimiterFactory.cpp
// PURPOSE: IMPLEMENTATION of the Factory class.
//
// Factory Pattern: one switch-case maps enum → concrete class.
//   Adding a new algorithm = add enum value + one case here. That's it.
//   Callers never see the concrete class names (decoupled).
// ============================================================================

#include "factory/RateLimiterFactory.h"

// Include all concrete algorithm headers (factory needs to know about them)
#include "algorithms/FixedWindow.h"
#include "algorithms/SlidingWindowLog.h"
#include "algorithms/SlidingWindowCounter.h"
#include "algorithms/TokenBucket.h"
#include "algorithms/LeakyBucket.h"

#include <stdexcept>    // std::invalid_argument

// ──────────────────────────────────────────────────────────────────────────────
// create: static factory method
// ──────────────────────────────────────────────────────────────────────────────
std::unique_ptr<RateLimiterInterface> RateLimiterFactory::create(
    RateLimitType type,
    const RateLimiterConfig& config
) {
    switch (type) {

        case RateLimitType::FIXED_WINDOW:
            return std::make_unique<FixedWindow>(config);
            // make_unique: allocates on heap, wraps in unique_ptr
            // Preferred over `new` because it's exception-safe

        case RateLimitType::SLIDING_WINDOW_LOG:
            return std::make_unique<SlidingWindowLog>(config);

        case RateLimitType::SLIDING_WINDOW_COUNTER:
            return std::make_unique<SlidingWindowCounter>(config);

        case RateLimitType::TOKEN_BUCKET:
            return std::make_unique<TokenBucket>(config);

        case RateLimitType::LEAKY_BUCKET:
            return std::make_unique<LeakyBucket>(config);

        default:
            throw std::invalid_argument("Unknown RateLimitType");
            // Fail fast: invalid enum → crash with a clear message
    }
}

