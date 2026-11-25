// ============================================================================
// FILE: src/pricing/CarPricing.cpp
// PURPOSE: Implementation of Car pricing strategy ($20/hour).
//
// PRICING RULE:
//   - Rate = $20.0 per hour
//   - Minimum charge = 1 hour
//   - Partial hours are rounded UP using ceil (e.g., 65 mins = 2 hours = $40)
// ============================================================================

#include "pricing/CarPricing.h"     // Class declaration
#include <cmath>                    // std::ceil for rounding up hours

// ────────────────────────────────────────────────────────────────────────────
// calculateFee: computes total parking fee in dollars for a car.
// @param durationMinutes: total minutes parked
// @return fee in dollars ($20 per hour, minimum $20)
// ────────────────────────────────────────────────────────────────────────────
double CarPricing::calculateFee(long long durationMinutes) const {
    // Convert minutes to fractional hours and round up
    double hours = std::ceil(durationMinutes / 60.0);

    // Enforce minimum 1-hour charge
    if (hours < 1.0) {
        hours = 1.0;
    }

    // Multiply by hourly car rate ($20/hr)
    return hours * 20.0;
}

