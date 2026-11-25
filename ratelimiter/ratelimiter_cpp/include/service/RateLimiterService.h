// ============================================================================
// FILE: include/service/RateLimiterService.h
// PURPOSE: DECLARATION of the service layer (entry point for the application).
//
// Implementation → src/service/RateLimiterService.cpp
//
// This class maps each UserTier to the right rate limiter.
// The application just calls: service.allowRequest(user)
// It knows nothing about algorithms, configs, or factories.
// ============================================================================

#ifndef RATE_LIMITER_SERVICE_H
#define RATE_LIMITER_SERVICE_H

#include "interfaces/RateLimiterInterface.h"
#include "models/User.h"
#include "models/UserTier.h"
#include <unordered_map>
#include <memory>           // shared_ptr

class RateLimiterService {
private:
    // Map: tier (as int) → rate limiter for that tier
    // We cast UserTier to int because enum class can't be hashed by default.
    // shared_ptr because map values need to be copyable during map operations.
    std::unordered_map<int, std::shared_ptr<RateLimiterInterface>> limiters_;

public:
    // Constructor: sets up limiters for each tier
    RateLimiterService();

    // The one method the application calls
    bool allowRequest(const User& user);
};

#endif // RATE_LIMITER_SERVICE_H

