// ============================================================================
// FILE: include/interfaces/RateLimiterInterface.h
// PURPOSE: Abstract base class that every rate limiter implements.
//
// WHY AN INTERFACE?
//   Strategy Pattern: callers depend on THIS, not on concrete algorithms.
//   You can swap TokenBucket ↔ FixedWindow without changing calling code.
//
// WHY HEADER-ONLY?
//   Pure virtual classes have NO implementation (= 0 methods).
//   There's nothing to put in a .cpp file. This is expected practice.
//
// RULE OF THUMB for "header-only vs .h + .cpp":
//   - Enums, pure interfaces, tiny inline getters → header-only ✅
//   - Classes with real logic (algorithms, services) → .h + .cpp ✅
// ============================================================================

#ifndef RATE_LIMITER_INTERFACE_H
#define RATE_LIMITER_INTERFACE_H

#include <string>   // For std::string

class RateLimiterInterface {
public:
    // The one question every rate limiter answers:
    // "Should I allow this request from this userId right now?"
    //
    // @param userId  — who is making the request
    // @return true   — ALLOW (proceed normally)
    //         false  — REJECT (caller returns HTTP 429)
    virtual bool allowRequest(const std::string& userId) = 0;
    // = 0 → pure virtual: subclasses MUST implement this
    // const std::string& → pass by reference, no copy

    // Virtual destructor: REQUIRED when you have virtual methods.
    // Without it, deleting derived object through base pointer = undefined behavior.
    virtual ~RateLimiterInterface() = default;
};

#endif // RATE_LIMITER_INTERFACE_H

