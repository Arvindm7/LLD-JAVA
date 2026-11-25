// ============================================================================
// FILE: src/algorithms/LeakyBucket.cpp
// PURPOSE: IMPLEMENTATION of the Leaky Bucket algorithm.
//
// HOW IT WORKS:
//   - Each user has a "bucket" holding water (requests).
//   - Each request ADDS 1 unit of water.
//   - Water LEAKS out at a constant rate (leakRate = maxReq / windowSec).
//   - If adding 1 would exceed capacity → REJECT.
//
// KEY DIFFERENCE FROM TOKEN BUCKET:
//   ┌──────────────┬──────────────────────────┬──────────────────────────┐
//   │              │ Token Bucket             │ Leaky Bucket             │
//   ├──────────────┼──────────────────────────┼──────────────────────────┤
//   │ Starts       │ FULL (all tokens)        │ EMPTY (no water)         │
//   │ Request does │ REMOVES a token          │ ADDS water               │
//   │ Over time    │ Tokens REFILL            │ Water LEAKS out          │
//   │ Bursts       │ ALLOWS bursts            │ SMOOTHS output           │
//   └──────────────┴──────────────────────────┴──────────────────────────┘
//
// ANALOGY: A bucket with a hole. You pour water in. It drains constantly.
//          If it overflows → request rejected.
//
// BEST FOR: Traffic shaping (steady output rate to downstream).
// MEMORY PER KEY: O(1) — one double + one long long.
// ============================================================================

#include "algorithms/LeakyBucket.h"
#include <chrono>
#include <algorithm>    // std::max

// ──────────────────────────────────────────────────────────────────────────────
// Constructor
// ──────────────────────────────────────────────────────────────────────────────
LeakyBucket::LeakyBucket(const RateLimiterConfig& config)
    : config_(config) {}

// ──────────────────────────────────────────────────────────────────────────────
// allowRequest: lazy leak + try to add one unit of water
// ──────────────────────────────────────────────────────────────────────────────
bool LeakyBucket::allowRequest(const std::string& userId) {

    std::lock_guard<std::mutex> lock(mtx_);

    // Step 1: Get current time
    long long now = std::chrono::duration_cast<std::chrono::milliseconds>(
        std::chrono::steady_clock::now().time_since_epoch()
    ).count();

    // Step 2: Derive capacity and leak rate from config
    int capacity = config_.getMaxRequests();     // Max water bucket can hold
    double leakPerSec = (double)config_.getMaxRequests() / config_.getWindowInSeconds();
    // e.g., 10 req per 60 sec → leak = 10/60 = 0.167 units/sec

    // Step 3: Handle first-time user → just record start time
    if (lastTime_.find(userId) == lastTime_.end()) {
        lastTime_[userId] = now;
        // water_ defaults to 0.0 for new keys → empty bucket → correct!
    }

    // Step 4: LAZY LEAK — calculate how much water drained since last check
    double elapsedSec = (now - lastTime_[userId]) / 1000.0;
    // Example: 2 seconds passed

    double leaked = water_[userId] - elapsedSec * leakPerSec;
    // Subtract water that leaked during elapsed time
    // Example: had 4 units, 2s passed, rate = 1/sec → 4 - 2 = 2 units

    double level = std::max(0.0, leaked);
    // Water can't go below 0 (no such thing as negative water)

    lastTime_[userId] = now;    // Update the check time

    // Step 5: Try to add one unit of water (= accept one request)
    if (level + 1 <= capacity) {
        water_[userId] = level + 1;     // Pour in one unit
        return true;                    // ALLOW: bucket hasn't overflowed
    }

    water_[userId] = level;             // Save leaked level (don't add request)
    return false;                       // REJECT: bucket is full (HTTP 429)
}

