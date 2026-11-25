#include <iostream>
#include <string>
#include <unordered_map>
#include <deque>
#include <vector>
#include <algorithm>
using namespace std;

/*
 * ALL 5 ALGORITHMS, SIMPLE VERSION.
 * Time is passed in as `now` (ms) so the demo is deterministic.
 * Real use: pass chrono::steady_clock time in ms.
 * Thread safety: add `mutex mtx;` + `lock_guard<mutex> lock(mtx);` at the top of allowRequest.
 *
 * TRICK: unordered_map::operator[] creates a default value (0 / empty deque) for a new key,
 * so "first time seeing this user" often needs no special code.
 *
 *  Algorithm        | Memory/key | Weakness
 *  -----------------|------------|-----------------------------------
 *  Fixed window     | O(1)       | 2x burst at window edge
 *  Sliding log      | O(limit)   | memory heavy, but exact
 *  Sliding counter  | O(1)       | approximate
 *  Token bucket     | O(1)       | allows bursts (usually a feature)
 *  Leaky bucket     | O(1)       | smooth rate, no extra bursts
 */
class RateLimiter {                                   // common interface (abstract base class)
public:
    virtual bool allowRequest(const string& key, long long now) = 0;
    virtual ~RateLimiter() {}
};

// 1. FIXED WINDOW: count requests in the current time slot; reset when slot changes.
class FixedWindow : public RateLimiter {
    int limit;
    long long windowMs;
    unordered_map<string, long long> windowId;        // slot this user is counting in (default 0)
    unordered_map<string, int> cnt;                   // requests in that slot (default 0)
public:
    FixedWindow(int limit, long long windowMs) : limit(limit), windowMs(windowMs) {}

    bool allowRequest(const string& key, long long now) override {
        long long slot = now / windowMs;              // 10s windows: 0-9999 -> slot 0
        if (windowId[key] != slot) {                  // new slot -> reset counter
            windowId[key] = slot;
            cnt[key] = 0;
        }
        if (cnt[key] < limit) { cnt[key]++; return true; }
        return false;
    }
};

// 2. SLIDING WINDOW LOG: remember the time of every accepted request.
class SlidingWindowLog : public RateLimiter {
    int limit;
    long long windowMs;
    unordered_map<string, deque<long long>> logs;
public:
    SlidingWindowLog(int limit, long long windowMs) : limit(limit), windowMs(windowMs) {}

    bool allowRequest(const string& key, long long now) override {
        deque<long long>& q = logs[key];              // reference, no copy
        while (!q.empty() && q.front() <= now - windowMs)
            q.pop_front();                            // drop requests older than the window
        if ((int)q.size() < limit) {
            q.push_back(now);                         // only log accepted requests
            return true;
        }
        return false;
    }
};

// 3. SLIDING WINDOW COUNTER: counts of this and previous slot, weighted.
//    estimate = prev * (1 - fractionOfCurrentSlotPassed) + curr
class SlidingWindowCounter : public RateLimiter {
    int limit;
    long long windowMs;
    unordered_map<string, long long> windowId;
    unordered_map<string, int> curr, prev;
public:
    SlidingWindowCounter(int limit, long long windowMs) : limit(limit), windowMs(windowMs) {}

    bool allowRequest(const string& key, long long now) override {
        long long slot = now / windowMs;
        if (windowId[key] != slot) {                  // moved to a new slot
            // exactly next slot: old current becomes previous; bigger gap: previous is stale
            prev[key] = (windowId[key] == slot - 1) ? curr[key] : 0;
            curr[key] = 0;
            windowId[key] = slot;
        }
        double fraction = (now % windowMs) / (double)windowMs;
        double estimate = prev[key] * (1 - fraction) + curr[key];
        if (estimate < limit) { curr[key]++; return true; }
        return false;
    }
};

// 4. TOKEN BUCKET: tokens refill over time, each request takes one.
class TokenBucket : public RateLimiter {
    int capacity;
    double refillPerSec;
    unordered_map<string, double> tokens;
    unordered_map<string, long long> last;
public:
    TokenBucket(int capacity, double refillPerSec) : capacity(capacity), refillPerSec(refillPerSec) {}

    bool allowRequest(const string& key, long long now) override {
        if (tokens.find(key) == tokens.end()) {       // new user -> full bucket
            tokens[key] = capacity;
            last[key] = now;
        }
        double elapsedSec = (now - last[key]) / 1000.0;
        double current = min((double)capacity, tokens[key] + elapsedSec * refillPerSec);
        last[key] = now;
        if (current >= 1) { tokens[key] = current - 1; return true; }
        tokens[key] = current;
        return false;
    }
};

// 5. LEAKY BUCKET (as a meter): each request adds 1 water, water leaks at a fixed rate.
//    Bucket full -> reject. Mirror image of token bucket.
class LeakyBucket : public RateLimiter {
    int capacity;
    double leakPerSec;
    unordered_map<string, double> water;              // default 0 = empty
    unordered_map<string, long long> last;
public:
    LeakyBucket(int capacity, double leakPerSec) : capacity(capacity), leakPerSec(leakPerSec) {}

    bool allowRequest(const string& key, long long now) override {
        if (last.find(key) == last.end()) last[key] = now;   // new user: start counting from now
        double elapsedSec = (now - last[key]) / 1000.0;
        double level = max(0.0, water[key] - elapsedSec * leakPerSec);
        last[key] = now;
        if (level + 1 <= capacity) { water[key] = level + 1; return true; }
        water[key] = level;
        return false;
    }
};

// send n requests for "u" at time t, return how many were allowed
int burst(RateLimiter& rl, int n, long long t) {
    int allowed = 0;
    for (int i = 0; i < n; i++) if (rl.allowRequest("u", t)) allowed++;
    return allowed;
}

int main() {
    // Limit: 5 requests per 10s. Send 5 right before the window edge, 5 right after.
    FixedWindow fw(5, 10000);
    SlidingWindowLog sl(5, 10000);
    SlidingWindowCounter sc(5, 10000);
    // NOTE: results go in variables first; the order of evaluation inside one cout chain is not guaranteed.
    int a = burst(fw, 5, 9900); a += burst(fw, 5, 10100);
    cout << "FixedWindow: " << a << " of 10 allowed\n";            // 10 (flaw)
    a = burst(sl, 5, 9900); a += burst(sl, 5, 10100);
    cout << "SlidingLog: " << a << " of 10 allowed\n";             // 5
    a = burst(sc, 5, 9900); a += burst(sc, 5, 10100);
    cout << "SlidingCounter: " << a << " of 10 allowed\n";         // 6 (approx)

    // Burst of 7 at t=0 with capacity 5 and rate 0.5/s, then 5 more at t=2s (1 unit freed)
    TokenBucket tb(5, 0.5);
    int first = burst(tb, 7, 0), second = burst(tb, 5, 2000);
    cout << "TokenBucket: " << first << " then " << second << "\n";  // 5 then 1
    LeakyBucket lb(5, 0.5);
    first = burst(lb, 7, 0); second = burst(lb, 5, 2000);
    cout << "LeakyBucket: " << first << " then " << second << "\n";  // 5 then 1
    return 0;
}
