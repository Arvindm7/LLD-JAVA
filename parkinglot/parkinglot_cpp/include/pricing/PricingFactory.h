// ============================================================================
// FILE: PricingFactory.h
// PURPOSE: Factory that creates the correct PricingStrategy for a vehicle type.
//
// CONCEPT: This is the FACTORY PATTERN. Instead of writing switch-case
//          logic everywhere you need a pricing strategy, you centralize
//          the creation in one place:
//
//              auto strategy = PricingFactory::create(VehicleType::CAR);
//              double fee = strategy->calculateFee(durationMinutes);
//
//          Analogy: Think of PricingFactory like a vending machine —
//          you insert a VehicleType coin, and it dispenses the matching
//          PricingStrategy object. You don't need to know which concrete
//          class was created; you just use the interface.
//
// WHY STATIC: The create() method is static because PricingFactory is
//             stateless — it doesn't hold any data. It's purely a
//             creation utility, so there's no need to instantiate it.
//
// WHY unique_ptr: The factory returns a unique_ptr to ensure the caller
//                 owns the strategy and it's automatically cleaned up.
//                 No manual delete needed — modern C++ memory safety.
//
// DESIGN NOTE: Declaration only — implementation lives in src/PricingFactory.cpp.
// ============================================================================

#ifndef PRICING_FACTORY_H   // Header guard: prevents double-inclusion
#define PRICING_FACTORY_H   // Define the guard macro

#include <memory>                    // For std::unique_ptr — safe ownership of the created strategy
#include "models/VehicleType.h"      // For VehicleType enum — input to decide which strategy to create
#include "pricing/PricingStrategy.h"  // For PricingStrategy base class — the return type

// Factory class that creates the appropriate PricingStrategy based on vehicle type
// Uses the Factory Pattern to encapsulate object creation logic in one place
class PricingFactory {

public:
    // Creates and returns a PricingStrategy matching the given vehicle type
    // BIKE → BikePricing ($10/hr), CAR → CarPricing ($20/hr), TRUCK → TruckPricing ($30/hr)
    // Returns a unique_ptr so the caller owns the object and it auto-deletes when done
    // Static because this is a stateless utility — no need to create a PricingFactory instance
    static std::unique_ptr<PricingStrategy> create(VehicleType type);
};

#endif // PRICING_FACTORY_H  // End of header guard
