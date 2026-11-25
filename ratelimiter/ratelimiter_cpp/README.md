# Rate Limiter LLD — C++ Implementation

## Proper Project Structure (include/ + src/)

### Folder Structure
```
ratelimiter_cpp/
├── include/                        ← DECLARATIONS (.h headers)
│   ├── models/                     ← Enums + value objects (no .cpp needed)
│   │   ├── UserTier.h              ← FREE / PREMIUM enum
│   │   ├── RateLimitType.h         ← Algorithm selection enum
│   │   └── User.h                  ← User value object
│   ├── config/
│   │   └── RateLimiterConfig.h     ← "N requests per W seconds"
│   ├── interfaces/
│   │   └── RateLimiterInterface.h  ← Abstract base class (Strategy Pattern)
│   ├── algorithms/                 ← 5 algorithm declarations
│   │   ├── FixedWindow.h
│   │   ├── SlidingWindowLog.h
│   │   ├── SlidingWindowCounter.h
│   │   ├── TokenBucket.h           ← ⭐ Default choice
│   │   └── LeakyBucket.h
│   ├── factory/
│   │   └── RateLimiterFactory.h    ← Factory pattern declaration
│   └── service/
│       └── RateLimiterService.h    ← Service layer declaration
│
├── src/                            ← IMPLEMENTATIONS (.cpp source)
│   ├── algorithms/                 ← 5 algorithm implementations
│   │   ├── FixedWindow.cpp
│   │   ├── SlidingWindowLog.cpp
│   │   ├── SlidingWindowCounter.cpp
│   │   ├── TokenBucket.cpp
│   │   └── LeakyBucket.cpp
│   ├── factory/
│   │   └── RateLimiterFactory.cpp
│   ├── service/
│   │   └── RateLimiterService.cpp
│   └── main.cpp                    ← Entry point + 5 demo scenarios
│
└── README.md
```

### Why .h + .cpp separation?

| Concern | Header-only (.h) | Separated (.h + .cpp) |
|---------|-------------------|-----------------------|
| **Enums, interfaces, tiny getters** | ✅ Correct | Overkill |
| **Classes with real logic** | ❌ Bad practice | ✅ Correct |
| **Compile speed** | Slow (recompiles everything) | Fast (only changed .cpp) |
| **Encapsulation** | Users see implementation | Users see only interface |
| **Linker errors (ODR)** | Risky in multi-file | Safe |

### Compile & Run

```powershell
cd "d:\DSA and prep for placement\LLD\ratelimiter\ratelimiter_cpp"

g++ -std=c++17 -I include -o ratelimiter.exe `
    src/algorithms/FixedWindow.cpp `
    src/algorithms/SlidingWindowLog.cpp `
    src/algorithms/SlidingWindowCounter.cpp `
    src/algorithms/TokenBucket.cpp `
    src/algorithms/LeakyBucket.cpp `
    src/factory/RateLimiterFactory.cpp `
    src/service/RateLimiterService.cpp `
    src/main.cpp

.\ratelimiter.exe
```

### Design Patterns
- **Strategy** — swap algorithms via `RateLimiterInterface`
- **Factory** — `RateLimiterFactory::create(enum, config)` → no `new` in caller
- **RAII** — `lock_guard<mutex>` for automatic unlock
- **Lazy Evaluation** — refill tokens on demand, no background threads
