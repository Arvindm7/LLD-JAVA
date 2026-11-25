// ============================================================================
// FILE: src/algorithms/TokenBucket.cpp
// PURPOSE: IMPLEMENTATION of the Token Bucket algorithm ⭐ (THE DEFAULT ANSWER)
//
// HOW IT WORKS:
//   - Each user has a "bucket" holding tokens (max = capacity).
//   - Tokens refill at a constant rate (refillRate = maxReq / windowSec).
//   - Each request costs 1 token. If ≥ 1 token → ALLOW (consume one).
//   - No tokens → REJECT.
//   - New users start with a FULL bucket (allows initial burst).
//
// KEY TRICK — LAZY REFILL:
//   We do NOT run a background timer to add tokens.
//   On each request, we calculate:
//     "How many seconds passed? Add (elapsed × refillRate) tokens."
//   This is O(1) and needs zero background threads.
//
// ANALOGY: A water tank that slowly fills. Each request drains 1 liter.
//          If empty, you must wait for it to refill.
//
// PROS: O(1) memory, allows bursts, no threads. Used by AWS/Stripe/Google.
// CONS: Two parameters to tune (capacity + refill rate).
//
// INTERVIEW LINE: "Token bucket — O(1) memory, allows bursts which real
//   clients need, and lazy refill means no timer threads."
//
// MEMORY PER KEY: O(1) — one double + one long long.
// ============================================================================

#include "algorithms/TokenBucket.h"
#include <chrono>
#include <algorithm>    // std::min

// ──────────────────────────────────────────────────────────────────────────────
// Constructor
// ──────────────────────────────────────────────────────────────────────────────
TokenBucket::TokenBucket(const RateLimiterConfig& config)
    : config_(config) {}

// ──────────────────────────────────────────────────────────────────────────────
// allowRequest: lazy refill + consume one token
// ──────────────────────────────────────────────────────────────────────────────
bool TokenBucket::allowRequest(const std::string& userId) {

    std::lock_guard<std::mutex> lock(mtx_);

    // Step 1: Get current time
    long long now = std::chrono::duration_cast<std::chrono::milliseconds>(
        std::chrono::steady_clock::now().time_since_epoch()
    ).count();

    // Step 2: Derive capacity and refill rate from config
    int capacity = config_.getMaxRequests();     // Max tokens = max burst size
    double refillPerSec = (double)config_.getMaxRequests() / config_.getWindowInSeconds();
    // e.g., 10 req per 60 sec → refill = 10/60 = 0.167 tokens/sec

    // Step 3: Handle first-time user → give them a FULL bucket
    if (tokens_.find(userId) == tokens_.end()) {
        tokens_[userId] = capacity;     // Start with full bucket
        lastTime_[userId] = now;        // Mark this moment as last refill
        // WHY full? So users can burst immediately (e.g., page load = many requests)
    }

    // Step 4: LAZY REFILL — calculate tokens to add based on elapsed time
    double elapsedSec = (now - lastTime_[userId]) / 1000.0;
    // 1000.0 → floating-point division! (1000 alone would do integer division → wrong)
    // Example: 1500ms → 1.5 seconds

    double refilled = tokens_[userId] + elapsedSec * refillPerSec;
    // Add tokens proportional to time passed
    // Example: had 2 tokens, 3 sec passed, rate = 0.167/sec → 2 + 0.5 = 2.5 tokens

    double current = std::min((double)capacity, refilled);
    // Cap at capacity: bucket NEVER holds more than capacity tokens
    // Example: capacity = 10, refilled = 15 → current = 10

    lastTime_[userId] = now;    // Always update time, even if we reject
    // WHY? So next refill calculation starts from THIS moment

    // Step 5: Try to consume one token
    if (current >= 1.0) {
        tokens_[userId] = current - 1;  // Take one token
        return true;                    // ALLOW
    }

    // Not enough tokens — save the fractional value so it accumulates
    tokens_[userId] = current;
    return false;                       // REJECT (HTTP 429)
}

