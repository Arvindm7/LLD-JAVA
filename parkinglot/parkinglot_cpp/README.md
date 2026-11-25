# Parking Lot System — Low Level Design (C++)

## Complete Object-Oriented Implementation

---

## 📁 Project Structure

```
parkinglot_cpp/
├── include/                            ← DECLARATIONS (.h headers)
│   ├── models/                         ← Data models & value objects
│   │   ├── VehicleType.h               ← BIKE, CAR, TRUCK enum
│   │   ├── SpotType.h                  ← SMALL, MEDIUM, LARGE enum
│   │   ├── Vehicle.h                   ← Vehicle value object
│   │   ├── ParkingSpot.h               ← ParkingSpot class declaration
│   │   └── Ticket.h                    ← Ticket class declaration
│   ├── pricing/                        ← Strategy Pattern for Pricing
│   │   ├── PricingStrategy.h           ← Pure virtual interface
│   │   ├── BikePricing.h               ← $10/hour strategy
│   │   ├── CarPricing.h                ← $20/hour strategy
│   │   ├── TruckPricing.h             ← $30/hour strategy
│   │   └── PricingFactory.h            ← Factory Pattern for pricing
│   └── core/                           ← Core System Classes
│       ├── ParkingFloor.h              ← Floor containing spots
│       └── ParkingLot.h                ← Central Singleton controller
│
├── src/                                ← IMPLEMENTATIONS (.cpp files)
│   ├── models/
│   │   ├── ParkingSpot.cpp
│   │   └── Ticket.cpp
│   ├── pricing/
│   │   ├── BikePricing.cpp
│   │   ├── CarPricing.cpp
│   │   ├── TruckPricing.cpp
│   │   └── PricingFactory.cpp
│   ├── core/
│   │   ├── ParkingFloor.cpp
│   │   └── ParkingLot.cpp
│   └── main.cpp                        ← Demo driver with test scenarios
│
└── README.md
```

---

## 🚀 How to Compile and Run

Run from the `parkinglot_cpp` directory:

```powershell
cd "d:\DSA and prep for placement\LLD\parkinglot\parkinglot_cpp"

g++ -std=c++17 -I include -o parkinglot.exe `
    src/models/ParkingSpot.cpp `
    src/models/Ticket.cpp `
    src/pricing/BikePricing.cpp `
    src/pricing/CarPricing.cpp `
    src/pricing/TruckPricing.cpp `
    src/pricing/PricingFactory.cpp `
    src/core/ParkingFloor.cpp `
    src/core/ParkingLot.cpp `
    src/main.cpp

.\parkinglot.exe
```

---

## 🎯 Design Patterns Used

| Pattern | Where Used | Why It's Used |
|---|---|---|
| **Singleton Pattern** | `ParkingLot` class | Only one parking lot facility exists in the entire system. `getInstance()` ensures a single global access point. |
| **Strategy Pattern** | `PricingStrategy` + `BikePricing`, `CarPricing`, `TruckPricing` | Decouples fee calculation algorithms from `ParkingLot`. New vehicle rates can be added without modifying existing code (Open/Closed Principle). |
| **Factory Pattern** | `PricingFactory` | Encapsulates the instantiation of pricing strategies based on `VehicleType`. Callers never use `new` directly. |
| **RAII (Resource Acquisition Is Initialization)** | `std::lock_guard<std::mutex>` | Automatically acquires locks on entry and safely releases them on exit (even if an exception occurs), preventing deadlocks. |
| **Composition** | `ParkingLot` contains `ParkingFloor`s, which contain `ParkingSpot`s | Clear lifecycle ownership. Floors and spots are cleaned up when the lot is destroyed. |

---

## 🧠 Mental Model & Key Flow

### 1. Parking a Vehicle (`parkVehicle`)
```
Vehicle arrives (LicensePlate + VehicleType)
   │
   ▼
Check if already parked? ──Yes──► Reject [ERROR]
   │ No
   ▼
Scan floors (Floor 1, 2...) for first available compatible spot
   │
   ├─► Found? ──► spot->parkVehicle()
   │              Issue Ticket (auto-incremented ID, current timestamp)
   │              activeTickets_[plate] = ticket
   │              Return &Ticket
   │
   └─► Not Found? ──► Return nullptr [FULL]
```

### 2. Unparking a Vehicle (`unparkVehicle`)
```
License plate provided
   │
   ▼
Check activeTickets_ map ──Not Found──► Return -1.0 [ERROR]
   │ Found
   ▼
Calculate duration: (now - entryTime) in minutes
   │
   ▼
PricingFactory::create(vehicleType) ──► PricingStrategy
   │
   ▼
strategy->calculateFee(duration)
   │
   ▼
Free the spot on the recorded floor (spot->removeVehicle())
   │
   ▼
Remove ticket from activeTickets_ map
   │
   ▼
Return calculated fee
```

---

## 💬 20-Minute Interview Cheat Sheet

When asked to design a Parking Lot in an interview, follow this order:

1. **Clarify Requirements (2 mins)**:
   - "How many floors? What vehicle types? (Bike, Car, Truck)"
   - "What are the spot sizes? (Small, Medium, Large)"
   - "How does pricing work? (Hourly by vehicle type)"
   - "Do we need thread safety? (Yes, via mutex)"
2. **List Entities & Design Patterns (2 mins)**:
   - Models: `VehicleType`, `SpotType`, `Vehicle`, `ParkingSpot`, `Ticket`
   - Strategy + Factory: `PricingStrategy`, `PricingFactory`
   - Core: `ParkingFloor`, `ParkingLot` (Singleton)
3. **Write Enums & Interfaces First (3 mins)**:
   - `VehicleType`, `SpotType`, `PricingStrategy`
4. **Write Models (4 mins)**:
   - `ParkingSpot` with `canFitVehicle()`, `parkVehicle()`, `removeVehicle()`
   - `Ticket` with `getParkedDurationInMinutes()`
5. **Write Floor & Lot Core Logic (7 mins)**:
   - `ParkingFloor::findAvailableSpot()`
   - `ParkingLot::parkVehicle()` & `ParkingLot::unparkVehicle()`
6. **Mention Trade-offs & Extensions (2 mins)**:
   - Multi-entry/exit gates
   - Payment processor integration (Credit Card, Cash, UPI)
   - Dynamic spot allocation (e.g., Bike taking empty Medium spot if allowed)
   - Database persistence

