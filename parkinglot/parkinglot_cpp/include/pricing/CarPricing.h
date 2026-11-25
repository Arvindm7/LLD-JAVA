// ============================================================================
// FILE: CarPricing.h
// PURPOSE: Concrete pricing strategy for CAR vehicles.
//
// CONCEPT: Cars are the mid-tier pricing — $20 per hour, with a minimum
//          charge of 1 hour. This is double the bike rate because cars
//          take up more space (MEDIUM spots vs SMALL spots).
//
//          Just like BikePricing, this is a "plugin" in the Strategy Pattern.
//          The ParkingLot doesn't care which pricing class it's using — it
//          just calls calculateFee() and gets the right answer.
//
// FEE FORMULA (in .cpp):
//     hours = ceil(durationMinutes / 60.0)   ← round up to next full hour
//     if (hours < 1) hours = 1               ← minimum 1 hour charge
//     fee = hours * 20.0                     ← $20 per hour
//
// DESIGN NOTE: Declaration only — implementation lives in src/CarPricing.cpp.
// ============================================================================

#ifndef CAR_PRICING_H   // Header guard: prevents double-inclusion
#define CAR_PRICING_H   // Define the guard macro

#include "pricing/PricingStrategy.h"  // Base class — provides the interface we're implementing

// Concrete strategy for calculating car parking fees ($20/hour)
// Inherits from PricingStrategy and provides the car-specific fee logic
class CarPricing : public PricingStrategy {

public:
    // Calculates the parking fee for a car based on duration
    // Rate: $20 per hour, minimum charge of 1 hour
    // override keyword: ensures this matches the base class virtual method signature
    // const: fee calculation is a read-only operation, no state changes
    double calculateFee(long long durationMinutes) const override;
};

#endif // CAR_PRICING_H  // End of header guard
