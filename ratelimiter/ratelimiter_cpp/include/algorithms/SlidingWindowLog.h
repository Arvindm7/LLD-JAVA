// ============================================================================
// FILE: include/algorithms/SlidingWindowLog.h
// PURPOSE: DECLARATION of the Sliding Window Log algorithm.
//
// Implementation → src/algorithms/SlidingWindowLog.cpp
// ============================================================================

#ifndef SLIDING_WINDOW_LOG_H
#define SLIDING_WINDOW_LOG_H

#include "interfaces/RateLimiterInterface.h"
#include "config/RateLimiterConfig.h"
#include <unordered_map>    // Per-user state
#include <deque>            // Timestamp log (O(1) pop_front + push_back)
#include <mutex>            // Thread safety
#include <string>

class SlidingWindowLog : public RateLimiterInterface {

private:
    RateLimiterConfig config_;
    std::unordered_map<std::string, std::deque<long long>> logs_;   // User → timestamps
    // WHY deque?
    //   - pop_front() is O(1) → remove old timestamps efficiently
    //   - push_back() is O(1) → add new timestamps
    //   - vector::erase(begin) is O(n) → too slow for this use case
    std::mutex mtx_;

public:
    explicit SlidingWindowLog(const RateLimiterConfig& config);
    bool allowRequest(const std::string& userId) override;
};

#endif // SLIDING_WINDOW_LOG_H

