// ============================================================================
// FILE: BikePricing.h
// PURPOSE: Concrete pricing strategy for BIKE vehicles.
//
// CONCEPT: Bikes are the cheapest to park — $10 per hour, with a minimum
//          charge of 1 hour. Even if you park for 5 minutes, you pay $10.
//
//          This class is one "plugin" in the Strategy Pattern family.
//          The PricingFactory creates a BikePricing when the vehicle is a BIKE,
//          and the ParkingLot calls calculateFee() without knowing which
//          concrete strategy it's using — that's polymorphism at work.
//
// FEE FORMULA (in .cpp):
//     hours = ceil(durationMinutes / 60.0)   ← round up to next full hour
//     if (hours < 1) hours = 1               ← minimum 1 hour charge
//     fee = hours * 10.0                     ← $10 per hour
//
// DESIGN NOTE: Declaration only — implementation lives in src/BikePricing.cpp.
// ============================================================================

#ifndef BIKE_PRICING_H   // Header guard: prevents double-inclusion
#define BIKE_PRICING_H   // Define the guard macro

#include "pricing/PricingStrategy.h"  // Base class — provides the interface we're implementing

// Concrete strategy for calculating bike parking fees ($10/hour)
// Inherits from PricingStrategy and provides the bike-specific fee logic
class BikePricing : public PricingStrategy {

public:
    // Calculates the parking fee for a bike based on duration
    // Rate: $10 per hour, minimum charge of 1 hour
    // override keyword: tells compiler this MUST match a virtual method in the base class
    // const: this method doesn't modify any member variables
    double calculateFee(long long durationMinutes) const override;
};

#endif // BIKE_PRICING_H  // End of header guard
