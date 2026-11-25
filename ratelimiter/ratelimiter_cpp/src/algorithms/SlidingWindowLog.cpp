// ============================================================================
// FILE: src/algorithms/SlidingWindowLog.cpp
// PURPOSE: IMPLEMENTATION of the Sliding Window Log algorithm.
//
// HOW IT WORKS:
//   - Store the timestamp of every ACCEPTED request in a deque.
//   - On each new request, remove timestamps older than (now - windowMs).
//   - If deque size < limit → ALLOW and add timestamp.
//   - Otherwise → REJECT.
//
// ANALOGY: A bouncer with a notepad writing down every entry time.
//          "I look back exactly 60 seconds. If I see 100 entries, you wait."
//
// PROS: Perfectly accurate. No boundary burst problem.
// CONS: O(limit) memory per user → if limit=10000 × 1M users = disaster.
//
// BEST FOR: Low limits (5 login attempts/min) where exactness matters.
// MEMORY PER KEY: O(limit) — one timestamp per accepted request in window.
// ============================================================================

#include "algorithms/SlidingWindowLog.h"
#include <chrono>

// ──────────────────────────────────────────────────────────────────────────────
// Constructor
// ──────────────────────────────────────────────────────────────────────────────
SlidingWindowLog::SlidingWindowLog(const RateLimiterConfig& config)
    : config_(config) {}

// ──────────────────────────────────────────────────────────────────────────────
// allowRequest: check the sliding window of timestamps
// ──────────────────────────────────────────────────────────────────────────────
bool SlidingWindowLog::allowRequest(const std::string& userId) {

    std::lock_guard<std::mutex> lock(mtx_);     // Thread safety (RAII)

    // Step 1: Get current time in milliseconds
    long long now = std::chrono::duration_cast<std::chrono::milliseconds>(
        std::chrono::steady_clock::now().time_since_epoch()
    ).count();

    // Step 2: Get a REFERENCE to this user's timestamp log
    std::deque<long long>& q = logs_[userId];
    // & → reference: we modify q directly, NOT a copy
    // operator[] creates empty deque for new users → no special "first time" code

    // Step 3: Evict all timestamps outside the sliding window
    while (!q.empty() && q.front() <= now - config_.getWindowInMs()) {
        q.pop_front();      // Remove the oldest timestamp
    }
    // After this loop, q only has timestamps within [now - windowMs, now]
    // Complexity: O(number of expired entries), amortized O(1) per call
    // because each timestamp is pushed once and popped once across its lifetime

    // Step 4: Check if there's room for one more request
    if ((int)q.size() < config_.getMaxRequests()) {
        q.push_back(now);   // Log this request's timestamp
        return true;        // ALLOW
    }

    return false;           // REJECT: window is full
    // Note: we do NOT log rejected requests. Only accepted ones count.
    // This is important: logging rejects would make the window never clear.
}

