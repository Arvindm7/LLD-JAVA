// ============================================================================
// FILE: src/main.cpp
// PURPOSE: Demo and test driver for the Rate Limiter LLD.
//          Shows all 5 algorithms + factory + service in action.
//
// COMPILE (from ratelimiter_cpp/ folder):
//   g++ -std=c++17 -I include -o ratelimiter.exe   \
//       src/algorithms/FixedWindow.cpp              \
//       src/algorithms/SlidingWindowLog.cpp         \
//       src/algorithms/SlidingWindowCounter.cpp     \
//       src/algorithms/TokenBucket.cpp              \
//       src/algorithms/LeakyBucket.cpp              \
//       src/factory/RateLimiterFactory.cpp          \
//       src/service/RateLimiterService.cpp          \
//       src/main.cpp
//
// RUN: .\ratelimiter.exe
// ============================================================================

#include <iostream>     // cout
#include <thread>       // sleep_for (to test time-based refill)
#include <chrono>       // milliseconds, seconds
#include <string>       // string

// Include only the headers we need (proper include discipline)
#include "config/RateLimiterConfig.h"
#include "interfaces/RateLimiterInterface.h"
#include "algorithms/FixedWindow.h"
#include "algorithms/SlidingWindowLog.h"
#include "algorithms/SlidingWindowCounter.h"
#include "algorithms/TokenBucket.h"
#include "algorithms/LeakyBucket.h"
#include "factory/RateLimiterFactory.h"
#include "service/RateLimiterService.h"
#include "models/User.h"
#include "models/UserTier.h"
#include "models/RateLimitType.h"

using namespace std;

// ──────────────────────────────────────────────────────────────────────────────
// Helper: send n requests for a user and print each result
// ──────────────────────────────────────────────────────────────────────────────
void testAlgorithm(const string& name, RateLimiterInterface& limiter, int numRequests) {
    cout << "\n--- " << name << " ---" << endl;
    cout << "Sending " << numRequests << " requests for user 'alice':" << endl;

    int allowed = 0;        // Track how many passed
    int rejected = 0;       // Track how many failed

    for (int i = 1; i <= numRequests; i++) {
        bool result = limiter.allowRequest("alice");
        cout << "  Request " << i << " -> " << (result ? "ALLOWED" : "REJECTED") << endl;
        if (result) allowed++;
        else rejected++;
    }

    cout << "Summary: " << allowed << " allowed, " << rejected << " rejected\n" << endl;
}

// ══════════════════════════════════════════════════════════════════════════════
// DEMO 1: Test each algorithm individually
// ══════════════════════════════════════════════════════════════════════════════
void demo1_individual_algorithms() {
    cout << "\n========================================================" << endl;
    cout << "DEMO 1: Each Algorithm (limit = 3 req per 10 sec)" << endl;
    cout << "========================================================" << endl;

    // Config: 3 requests per 10-second window
    RateLimiterConfig config(3, 10);

    // Create each algorithm and send 5 requests → expect 3 ALLOW, 2 REJECT
    FixedWindow fw(config);
    testAlgorithm("1. Fixed Window", fw, 5);

    SlidingWindowLog sl(config);
    testAlgorithm("2. Sliding Window Log", sl, 5);

    SlidingWindowCounter sc(config);
    testAlgorithm("3. Sliding Window Counter", sc, 5);

    TokenBucket tb(config);
    testAlgorithm("4. Token Bucket", tb, 5);

    LeakyBucket lb(config);
    testAlgorithm("5. Leaky Bucket", lb, 5);
}

// ══════════════════════════════════════════════════════════════════════════════
// DEMO 2: Factory Pattern — create limiters without knowing concrete classes
// ══════════════════════════════════════════════════════════════════════════════
void demo2_factory_pattern() {
    cout << "\n========================================================" << endl;
    cout << "DEMO 2: Factory Pattern" << endl;
    cout << "========================================================" << endl;

    RateLimiterConfig config(3, 10);

    // Create via factory — caller only knows the interface, not the concrete class
    auto limiter = RateLimiterFactory::create(RateLimitType::TOKEN_BUCKET, config);
    // auto → unique_ptr<RateLimiterInterface>

    testAlgorithm("Factory-created Token Bucket", *limiter, 5);
    // *limiter → dereference unique_ptr to get reference
}

// ══════════════════════════════════════════════════════════════════════════════
// DEMO 3: Service with tier-based limiting (full integration)
// ══════════════════════════════════════════════════════════════════════════════
void demo3_service_integration() {
    cout << "\n========================================================" << endl;
    cout << "DEMO 3: Service (FREE vs PREMIUM)" << endl;
    cout << "========================================================" << endl;

    RateLimiterService service;     // Sets up limiters per tier

    User freeUser("alice", UserTier::FREE);       // FREE: 10 req/min
    User premiumUser("bob", UserTier::PREMIUM);   // PREMIUM: 100 req/min

    cout << "\nFREE user 'alice' (10 req/min):" << endl;
    for (int i = 1; i <= 12; i++) {
        bool result = service.allowRequest(freeUser);
        cout << "  Request " << i << " -> " << (result ? "ALLOWED" : "REJECTED") << endl;
    }

    cout << "\nPREMIUM user 'bob' (100 req/min):" << endl;
    for (int i = 1; i <= 12; i++) {
        bool result = service.allowRequest(premiumUser);
        cout << "  Request " << i << " -> " << (result ? "ALLOWED" : "REJECTED") << endl;
    }
}

// ══════════════════════════════════════════════════════════════════════════════
// DEMO 4: Token refill over time (proves lazy refill works)
// ══════════════════════════════════════════════════════════════════════════════
void demo4_refill_over_time() {
    cout << "\n========================================================" << endl;
    cout << "DEMO 4: Token Bucket Refill Over Time" << endl;
    cout << "========================================================" << endl;

    // 3 tokens, refill = 3/10 = 0.3 tokens/sec
    RateLimiterConfig config(3, 10);
    TokenBucket tb(config);

    cout << "\nExhaust all 3 tokens:" << endl;
    for (int i = 1; i <= 4; i++) {
        cout << "  Request " << i << " -> "
             << (tb.allowRequest("alice") ? "ALLOWED" : "REJECTED") << endl;
    }

    cout << "\nWaiting 4 seconds for refill..." << endl;
    this_thread::sleep_for(chrono::seconds(4));
    // 4 sec × 0.3 tokens/sec = 1.2 tokens refilled → ≥ 1 → should ALLOW

    cout << "After 4s wait:" << endl;
    cout << "  Request -> " << (tb.allowRequest("alice") ? "ALLOWED" : "REJECTED") << endl;
}

// ══════════════════════════════════════════════════════════════════════════════
// DEMO 5: Per-user isolation (users have separate buckets)
// ══════════════════════════════════════════════════════════════════════════════
void demo5_per_user_isolation() {
    cout << "\n========================================================" << endl;
    cout << "DEMO 5: Per-User Isolation" << endl;
    cout << "========================================================" << endl;

    RateLimiterConfig config(2, 10);    // Only 2 requests allowed
    TokenBucket tb(config);

    cout << "\nAlice exhausts her 2 tokens:" << endl;
    cout << "  alice req1 -> " << (tb.allowRequest("alice") ? "ALLOWED" : "REJECTED") << endl;
    cout << "  alice req2 -> " << (tb.allowRequest("alice") ? "ALLOWED" : "REJECTED") << endl;
    cout << "  alice req3 -> " << (tb.allowRequest("alice") ? "ALLOWED" : "REJECTED") << endl;

    cout << "\nBob is unaffected (separate bucket):" << endl;
    cout << "  bob req1   -> " << (tb.allowRequest("bob") ? "ALLOWED" : "REJECTED") << endl;
    cout << "  bob req2   -> " << (tb.allowRequest("bob") ? "ALLOWED" : "REJECTED") << endl;
    cout << "  bob req3   -> " << (tb.allowRequest("bob") ? "ALLOWED" : "REJECTED") << endl;
}

// ──────────────────────────────────────────────────────────────────────────────
// MAIN: run all demos
// ──────────────────────────────────────────────────────────────────────────────
int main() {
    cout << "==========================================================" << endl;
    cout << "   RATE LIMITER — Low Level Design in C++" << endl;
    cout << "   5 Algorithms + Factory + Service Layer" << endl;
    cout << "==========================================================" << endl;

    demo1_individual_algorithms();
    demo2_factory_pattern();
    demo3_service_integration();
    demo4_refill_over_time(); 
    demo5_per_user_isolation();
    
    cout << "\nAll demos complete!" << endl;
    return 0;
}

