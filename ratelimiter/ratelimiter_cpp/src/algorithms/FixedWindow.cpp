// ============================================================================
// FILE: src/algorithms/FixedWindow.cpp
// PURPOSE: IMPLEMENTATION of the Fixed Window Counter algorithm.
//
// HOW IT WORKS:
//   - Divide time into fixed slots (e.g., each minute: 0-59s, 60-119s, ...).
//   - Count requests per user in the current slot.
//   - If count >= limit → REJECT.
//   - When the slot changes → reset the counter to 0.
//
// ANALOGY: A nightclub bouncer who counts from scratch every hour.
//          "I let in 100 people this hour. New hour? Counter goes to zero."
//
// PROS: Simple, O(1) memory per user, O(1) time.
// CONS: Boundary burst → 100 reqs at 12:00:59 + 100 at 12:01:00
//       = 200 in 2 seconds, both "legal" per their windows.
//
// MEMORY PER KEY: O(1) — just a slot ID and a counter.
// ============================================================================

#include "algorithms/FixedWindow.h"     // Our header (declaration)
#include <chrono>                       // Getting current time

// ──────────────────────────────────────────────────────────────────────────────
// Constructor: just copy the config into our member variable
// ──────────────────────────────────────────────────────────────────────────────
FixedWindow::FixedWindow(const RateLimiterConfig& config)
    : config_(config) {}
// Initializer list: constructs config_ directly from config
// This is more efficient than: config_ = config; (which default-constructs then assigns)

// ──────────────────────────────────────────────────────────────────────────────
// allowRequest: the core logic — should we allow this request?
// ──────────────────────────────────────────────────────────────────────────────
bool FixedWindow::allowRequest(const std::string& userId) {

    // Step 1: Lock the mutex for thread safety (RAII pattern)
    std::lock_guard<std::mutex> lock(mtx_);
    // lock_guard: locks on construction, unlocks on destruction (end of function)
    // Even if an exception is thrown, the lock is released → no deadlocks

    // Step 2: Get the current time in milliseconds
    long long now = std::chrono::duration_cast<std::chrono::milliseconds>(
        std::chrono::steady_clock::now().time_since_epoch()
    ).count();
    // steady_clock → monotonic, never goes backward (unlike system_clock)
    // time_since_epoch → time since some fixed point → gives us a raw number
    // duration_cast<milliseconds> → convert to ms for easier math

    // Step 3: Calculate which time slot we're in
    long long slot = now / config_.getWindowInMs();
    // Example: windowMs = 60000 (1 min)
    //   time 59999ms  → slot 0
    //   time 60000ms  → slot 1 (new window!)
    //   time 119999ms → slot 1
    //   time 120000ms → slot 2 (new window!)

    // Step 4: If user moved to a new slot, reset their counter
    if (windowId_[userId] != slot) {
        // operator[] on unordered_map: creates default (0) for new keys
        // So "first time user" automatically gets windowId=0, count=0
        windowId_[userId] = slot;       // Remember which slot we're now in
        count_[userId] = 0;             // Reset the counter for the new slot
    }

    // Step 5: Check if user is under the limit
    if (count_[userId] < config_.getMaxRequests()) {
        count_[userId]++;               // Consume one from the allowance
        return true;                    // ALLOW the request
    }

    return false;                       // REJECT: limit reached for this window
}

