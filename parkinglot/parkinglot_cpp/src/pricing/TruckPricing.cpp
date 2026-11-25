// ============================================================================
// FILE: src/pricing/TruckPricing.cpp
// PURPOSE: Implementation of Truck pricing strategy ($30/hour).
//
// PRICING RULE:
//   - Rate = $30.0 per hour
//   - Minimum charge = 1 hour
//   - Partial hours are rounded UP using ceil (e.g., 65 mins = 2 hours = $60)
// ============================================================================

#include "pricing/TruckPricing.h"   // Class declaration
#include <cmath>                    // std::ceil for rounding up hours

// ────────────────────────────────────────────────────────────────────────────
// calculateFee: computes total parking fee in dollars for a truck.
// @param durationMinutes: total minutes parked
// @return fee in dollars ($30 per hour, minimum $30)
// ────────────────────────────────────────────────────────────────────────────
double TruckPricing::calculateFee(long long durationMinutes) const {
    // Convert minutes to fractional hours and round up
    double hours = std::ceil(durationMinutes / 60.0);

    // Enforce minimum 1-hour charge
    if (hours < 1.0) {
        hours = 1.0;
    }

    // Multiply by hourly truck rate ($30/hr)
    return hours * 30.0;
}

