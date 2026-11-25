// ============================================================================
// FILE: src/algorithms/SlidingWindowCounter.cpp
// PURPOSE: IMPLEMENTATION of the Sliding Window Counter (hybrid) algorithm.
//
// HOW IT WORKS:
//   Keep counts for CURRENT and PREVIOUS window. Estimate:
//
//     estimate = prevCount × (1 - fraction) + currCount
//
//   where fraction = how far into the current window (0.0 to 1.0).
//
// EXAMPLE:
//   Previous window had 80 requests. We're 40% into current window.
//   Current window has 30 requests so far.
//   estimate = 80 × (1 - 0.4) + 30 = 80 × 0.6 + 30 = 48 + 30 = 78
//   If limit = 100 → 78 < 100 → ALLOW
//
// WHY IT WORKS:
//   Assumes requests in the previous window were evenly distributed.
//   Cloudflare tested this: only ~0.003% error rate. Good enough.
//
// PROS: O(1) memory, smooths boundary bursts.
// CONS: Approximate, not exact.
// BEST FOR: High-scale APIs (Cloudflare, Stripe use variants of this).
// MEMORY PER KEY: O(1) — two ints and a slot ID.
// ============================================================================

#include "algorithms/SlidingWindowCounter.h"
#include <chrono>

// ──────────────────────────────────────────────────────────────────────────────
// Constructor
// ──────────────────────────────────────────────────────────────────────────────
SlidingWindowCounter::SlidingWindowCounter(const RateLimiterConfig& config)
    : config_(config) {}

// ──────────────────────────────────────────────────────────────────────────────
// allowRequest: weighted estimate of requests in the sliding window
// ──────────────────────────────────────────────────────────────────────────────
bool SlidingWindowCounter::allowRequest(const std::string& userId) {

    std::lock_guard<std::mutex> lock(mtx_);

    // Step 1: Get current time
    long long now = std::chrono::duration_cast<std::chrono::milliseconds>(
        std::chrono::steady_clock::now().time_since_epoch()
    ).count();

    // Step 2: Which time slot are we in?
    long long slot = now / config_.getWindowInMs();

    // Step 3: If we moved to a new slot, shift the counters
    if (windowId_[userId] != slot) {

        if (windowId_[userId] == slot - 1) {
            // Adjacent slot: the old "current" becomes the new "previous"
            prevCount_[userId] = currCount_[userId];
        } else {
            // Gap > 1 slot: previous data is too old, discard it
            prevCount_[userId] = 0;
        }

        currCount_[userId] = 0;     // Start fresh for the new slot
        windowId_[userId] = slot;   // Remember we're in this slot now
    }

    // Step 4: Calculate the weighted estimate
    double fraction = (now % config_.getWindowInMs()) / (double)config_.getWindowInMs();
    // now % windowMs → how many ms have passed in the current slot
    // / windowMs     → normalize to 0.0 - 1.0
    //
    // fraction = 0.0 → start of slot (100% of prev window is relevant)
    // fraction = 0.5 → middle of slot (50% of prev window is relevant)
    // fraction = 1.0 → end of slot (0% of prev window is relevant)

    double estimate = prevCount_[userId] * (1.0 - fraction) + currCount_[userId];
    // This approximates: "how many requests are in the last windowMs milliseconds?"

    // Step 5: Allow or reject
    if (estimate < config_.getMaxRequests()) {
        currCount_[userId]++;       // Count this request in the current slot
        return true;                // ALLOW
    }

    return false;                   // REJECT
}

