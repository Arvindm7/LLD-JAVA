// ============================================================================
// FILE: src/service/RateLimiterService.cpp
// PURPOSE: IMPLEMENTATION of the service layer.
//
// This is the entry point the application uses.
// It wires together: UserTier → Config → Factory → Algorithm.
// The application just calls: service.allowRequest(user) — knows nothing else.
// ============================================================================

#include "service/RateLimiterService.h"
#include "factory/RateLimiterFactory.h"
#include "config/RateLimiterConfig.h"
#include "models/RateLimitType.h"
#include <stdexcept>

// ──────────────────────────────────────────────────────────────────────────────
// Constructor: configure which tier gets which algorithm and limits
// ──────────────────────────────────────────────────────────────────────────────
RateLimiterService::RateLimiterService() {

    // FREE tier: Token Bucket, 10 requests per 60 seconds
    // Token Bucket allows initial burst (good for page loads) + steady rate
    limiters_[static_cast<int>(UserTier::FREE)] =
        RateLimiterFactory::create(
            RateLimitType::TOKEN_BUCKET,        // Algorithm choice
            RateLimiterConfig(10, 60)           // 10 req/min
        );
    // static_cast<int> → convert enum class to int for map key
    // (enum class can't be hashed by unordered_map without a custom hasher)

    // PREMIUM tier: Fixed Window, 100 requests per 60 seconds
    // Higher limits for paid users
    limiters_[static_cast<int>(UserTier::PREMIUM)] =
        RateLimiterFactory::create(
            RateLimitType::FIXED_WINDOW,
            RateLimiterConfig(100, 60)          // 100 req/min
        );
}

// ──────────────────────────────────────────────────────────────────────────────
// allowRequest: look up the right limiter by tier, then delegate
// ──────────────────────────────────────────────────────────────────────────────
bool RateLimiterService::allowRequest(const User& user) {

    // Look up the limiter for this user's tier
    auto it = limiters_.find(static_cast<int>(user.getTier()));
    // auto: compiler deduces iterator type (verbose otherwise)

    if (it == limiters_.end()) {
        // No limiter for this tier → programming error, fail fast
        throw std::invalid_argument("No limiter configured for this tier");
    }

    // Delegate to the algorithm — the service doesn't know WHICH algorithm!
    // This is the Strategy Pattern in action.
    return it->second->allowRequest(user.getUserId());
    // it->second → the shared_ptr<RateLimiterInterface> (map value)
    // ->allowRequest → calls the virtual method → dispatches to correct algo
}

