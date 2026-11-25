// ============================================================================
// FILE: include/algorithms/FixedWindow.h
// PURPOSE: DECLARATION of the Fixed Window Counter algorithm.
//
// This file declares WHAT the class looks like (members, methods).
// The actual logic lives in src/algorithms/FixedWindow.cpp.
//
// WHY SEPARATE .h AND .cpp?
//   1. Compilation speed: changing .cpp doesn't recompile files that #include .h
//   2. Encapsulation: users see the interface, not the guts
//   3. Avoids ODR (One Definition Rule) violations in multi-file projects
//   4. Professional practice: this is how production C++ is written
// ============================================================================

#ifndef FIXED_WINDOW_H
#define FIXED_WINDOW_H

#include "interfaces/RateLimiterInterface.h"    // Base class
#include "config/RateLimiterConfig.h"           // Config
#include <unordered_map>    // Per-user state
#include <mutex>            // Thread safety
#include <string>           // std::string

class FixedWindow : public RateLimiterInterface {
    // "public RateLimiterInterface" → FixedWindow IS-A RateLimiterInterface

private:
    RateLimiterConfig config_;                              // Rule: N req per W sec
    std::unordered_map<std::string, long long> windowId_;   // User → current slot ID
    std::unordered_map<std::string, int> count_;            // User → count in that slot
    std::mutex mtx_;                                        // Protects both maps

public:
    // Constructor: declared here, defined in .cpp
    explicit FixedWindow(const RateLimiterConfig& config);
    // explicit → prevents accidental implicit conversions

    // Override the interface method: declared here, defined in .cpp
    bool allowRequest(const std::string& userId) override;
    // override → compiler verifies this actually overrides a base method
};

#endif // FIXED_WINDOW_H

