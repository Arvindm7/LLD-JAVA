// ============================================================================
// FILE: include/factory/RateLimiterFactory.h
// PURPOSE: DECLARATION of the Factory class.
//
// Implementation → src/factory/RateLimiterFactory.cpp
//
// Factory Pattern: callers say WHICH algorithm via enum, never use `new`.
// Adding a new algorithm = add enum value + one case in .cpp. That's it.
// ============================================================================

#ifndef RATE_LIMITER_FACTORY_H
#define RATE_LIMITER_FACTORY_H

#include "interfaces/RateLimiterInterface.h"
#include "config/RateLimiterConfig.h"
#include "models/RateLimitType.h"
#include <memory>       // std::unique_ptr (smart pointer, auto-deletes)

class RateLimiterFactory {
public:
    // Static factory method: creates and returns a rate limiter
    //
    // Returns unique_ptr → caller OWNS the object, auto-deleted on scope exit
    // WHY static? No need to instantiate the factory itself.
    //   Just call: RateLimiterFactory::create(type, config);
    static std::unique_ptr<RateLimiterInterface> create(
        RateLimitType type,
        const RateLimiterConfig& config
    );
};

#endif // RATE_LIMITER_FACTORY_H

