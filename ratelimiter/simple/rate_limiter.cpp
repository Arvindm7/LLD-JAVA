#include <iostream>       // cout
#include <string>
#include <unordered_map>    // O(1) average lookup per user
#include <mutex>            // thread safety
#include <chrono>           // clock
#include <algorithm>        // std::min
#include <thread>           // sleep in main
using namespace std;

class RateLimiter {
private:
    int capacity;                              // max tokens in a bucket = max burst size
    double refillPerSec;                       // tokens added per second = sustained rate
    unordered_map<string, double> tokens;      // user -> tokens currently in bucket
    unordered_map<string, long long> lastRefill; // user -> time (ms) of last update
    mutex mtx;                                 // protects both maps

public:
    // Constructor: store the config. Initializer list is the idiomatic C++ way.
    RateLimiter(int capacity, double refillPerSec)
        : capacity(capacity), refillPerSec(refillPerSec) {}

    bool allowRequest(const string& key) {     // const& -> no string copy
        lock_guard<mutex> lock(mtx);           // lock now, auto-unlock when function returns (RAII)

        // current time in milliseconds; steady_clock never goes backwards (unlike system_clock)
        long long now = chrono::duration_cast<chrono::milliseconds>(
                            chrono::steady_clock::now().time_since_epoch()).count();

        // first request from this user -> give a FULL bucket
        if (tokens.find(key) == tokens.end()) {
            tokens[key] = capacity;
            lastRefill[key] = now;
        }

        // how many seconds passed since we last touched this bucket
        double elapsedSec = (now - lastRefill[key]) / 1000.0;   // 1000.0 -> floating division, not int

        // lazy refill: add tokens for elapsed time, but never exceed capacity
        double current = min((double)capacity, tokens[key] + elapsedSec * refillPerSec);

        lastRefill[key] = now;                 // always update, even if we reject

        if (current >= 1) {                    // at least one whole token available
            tokens[key] = current - 1;         // consume one
            return true;                       // ALLOW
        }
        tokens[key] = current;                 // save the refilled (fractional) value
        return false;                          // REJECT (HTTP 429)
    }
};

int main() {
    RateLimiter rl(3, 1);                      // burst of 3, refill 1 token/sec

    for (int i = 1; i <= 5; i++) {
        cout << "req " << i << " -> " << (rl.allowRequest("alice") ? "true" : "false") << "\n"; // T T T F F
    }
    cout << "bob   -> " << (rl.allowRequest("bob") ? "true" : "false") << "\n";  // separate bucket

    this_thread::sleep_for(chrono::milliseconds(1100));                         // wait for refill
    cout << "alice after 1.1s -> " << (rl.allowRequest("alice") ? "true" : "false") << "\n"; // true
    return 0;
}
