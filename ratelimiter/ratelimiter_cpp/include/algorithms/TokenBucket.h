// ============================================================================
// FILE: include/algorithms/TokenBucket.h
// PURPOSE: DECLARATION of the Token Bucket algorithm ⭐ (THE DEFAULT ANSWER)
//
// Implementation → src/algorithms/TokenBucket.cpp
//
// WHY THIS IS THE DEFAULT:
//   - O(1) memory per user
//   - Allows controlled bursts (real clients need this)
//   - Lazy refill → no background threads
//   - Used by AWS, Stripe, Google Cloud
// ============================================================================

#ifndef TOKEN_BUCKET_H
#define TOKEN_BUCKET_H

#include "interfaces/RateLimiterInterface.h"
#include "config/RateLimiterConfig.h"
#include <unordered_map>
#include <mutex>
#include <string>

class TokenBucket : public RateLimiterInterface {

private:
    RateLimiterConfig config_;
    std::unordered_map<std::string, double> tokens_;        // User → current token count
    std::unordered_map<std::string, long long> lastTime_;   // User → last refill time (ms)
    // WHY double for tokens?
    //   Refill produces fractions: 0.3 sec × 2 tokens/sec = 0.6 tokens
    //   We accumulate these fractions until they reach ≥ 1.0
    std::mutex mtx_;

public:
    explicit TokenBucket(const RateLimiterConfig& config);
    bool allowRequest(const std::string& userId) override;
};

#endif // TOKEN_BUCKET_H

