// ============================================================================
// FILE: include/algorithms/LeakyBucket.h
// PURPOSE: DECLARATION of the Leaky Bucket algorithm.
//
// Implementation → src/algorithms/LeakyBucket.cpp
//
// MIRROR IMAGE of Token Bucket:
//   Token Bucket: starts FULL, requests REMOVE tokens, tokens REFILL
//   Leaky Bucket: starts EMPTY, requests ADD water, water LEAKS out
// ============================================================================

#ifndef LEAKY_BUCKET_H
#define LEAKY_BUCKET_H

#include "interfaces/RateLimiterInterface.h"
#include "config/RateLimiterConfig.h"
#include <unordered_map>
#include <mutex>
#include <string>

class LeakyBucket : public RateLimiterInterface {

private:
    RateLimiterConfig config_;
    std::unordered_map<std::string, double> water_;         // User → current water level
    std::unordered_map<std::string, long long> lastTime_;   // User → last check time (ms)
    // New user: water_ defaults to 0.0 (empty bucket) — correct behavior
    std::mutex mtx_;

public:
    explicit LeakyBucket(const RateLimiterConfig& config);
    bool allowRequest(const std::string& userId) override;
};

#endif // LEAKY_BUCKET_H

