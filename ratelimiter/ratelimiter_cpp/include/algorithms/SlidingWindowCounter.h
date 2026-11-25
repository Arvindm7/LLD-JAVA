// ============================================================================
// FILE: include/algorithms/SlidingWindowCounter.h
// PURPOSE: DECLARATION of the Sliding Window Counter (hybrid) algorithm.
//
// Implementation → src/algorithms/SlidingWindowCounter.cpp
// ============================================================================

#ifndef SLIDING_WINDOW_COUNTER_H
#define SLIDING_WINDOW_COUNTER_H

#include "interfaces/RateLimiterInterface.h"
#include "config/RateLimiterConfig.h"
#include <unordered_map>
#include <mutex>
#include <string>

class SlidingWindowCounter : public RateLimiterInterface {

private:
    RateLimiterConfig config_;
    std::unordered_map<std::string, long long> windowId_;   // User → current slot ID
    std::unordered_map<std::string, int> currCount_;        // Count in current slot
    std::unordered_map<std::string, int> prevCount_;        // Count in previous slot
    // The "trick": estimate = prevCount × (1 - fraction) + currCount
    std::mutex mtx_;

public:
    explicit SlidingWindowCounter(const RateLimiterConfig& config);
    bool allowRequest(const std::string& userId) override;
};

#endif // SLIDING_WINDOW_COUNTER_H

