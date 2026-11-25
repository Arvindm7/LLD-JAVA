// ============================================================================
// FILE: src/pricing/PricingFactory.cpp
// PURPOSE: Implementation of PricingFactory (Factory Pattern).
//
// WHAT THIS CLASS DOES:
//   Instantiates and returns the appropriate concrete PricingStrategy
//   based on the given VehicleType:
//     - BIKE  -> BikePricing  ($10/hr)
//     - CAR   -> CarPricing   ($20/hr)
//     - TRUCK -> TruckPricing ($30/hr)
//
// WHY UNIQUE_PTR:
//   Caller receives full ownership of the strategy object,
//   and it is automatically deleted when out of scope (no memory leaks).
// ============================================================================

#include "pricing/PricingFactory.h" // Factory declaration
#include "pricing/BikePricing.h"    // Bike pricing strategy
#include "pricing/CarPricing.h"     // Car pricing strategy
#include "pricing/TruckPricing.h"   // Truck pricing strategy
#include <stdexcept>                // std::invalid_argument

// ────────────────────────────────────────────────────────────────────────────
// create: static factory method mapping VehicleType -> PricingStrategy
// ────────────────────────────────────────────────────────────────────────────
std::unique_ptr<PricingStrategy> PricingFactory::create(VehicleType type) {
    switch (type) {
        // Bike strategy
        case VehicleType::BIKE:
            return std::make_unique<BikePricing>();

        // Car strategy
        case VehicleType::CAR:
            return std::make_unique<CarPricing>();

        // Truck strategy
        case VehicleType::TRUCK:
            return std::make_unique<TruckPricing>();

        // Invalid or unexpected type
        default:
            throw std::invalid_argument("Unknown vehicle type for pricing");
    }
}

