// ============================================================================
// FILE: TruckPricing.h
// PURPOSE: Concrete pricing strategy for TRUCK vehicles.
//
// CONCEPT: Trucks are the most expensive to park — $30 per hour, with a
//          minimum charge of 1 hour. They occupy LARGE spots which take
//          up the most real estate in the lot, hence the premium rate.
//
//          This completes the Strategy Pattern trio: BikePricing ($10),
//          CarPricing ($20), TruckPricing ($30). The PricingFactory
//          creates the right one based on VehicleType.
//
// FEE FORMULA (in .cpp):
//     hours = ceil(durationMinutes / 60.0)   ← round up to next full hour
//     if (hours < 1) hours = 1               ← minimum 1 hour charge
//     fee = hours * 30.0                     ← $30 per hour
//
// DESIGN NOTE: Declaration only — implementation lives in src/TruckPricing.cpp.
// ============================================================================

#ifndef TRUCK_PRICING_H   // Header guard: prevents double-inclusion
#define TRUCK_PRICING_H   // Define the guard macro

#include "pricing/PricingStrategy.h"  // Base class — provides the interface we're implementing

// Concrete strategy for calculating truck parking fees ($30/hour)
// Inherits from PricingStrategy and provides the truck-specific fee logic
class TruckPricing : public PricingStrategy {

public:
    // Calculates the parking fee for a truck based on duration
    // Rate: $30 per hour, minimum charge of 1 hour
    // override keyword: compiler checks this matches base class virtual signature
    // const: calculation is pure — doesn't modify any member state
    double calculateFee(long long durationMinutes) const override;
};

#endif // TRUCK_PRICING_H  // End of header guard
