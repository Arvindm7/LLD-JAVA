// ============================================================================
// FILE: src/pricing/BikePricing.cpp
// PURPOSE: Implementation of Bike pricing strategy ($10/hour).
//
// PRICING RULE:
//   - Rate = $10.0 per hour
//   - Minimum charge = 1 hour (even 5 minutes is charged for 1 full hour)
//   - Partial hours are rounded UP using ceil (e.g., 65 mins = 2 hours)
// ============================================================================

#include "pricing/BikePricing.h"    // Class declaration
#include <cmath>                    // std::ceil for rounding up hours

// ────────────────────────────────────────────────────────────────────────────
// calculateFee: computes total parking fee in dollars for a bike.
// @param durationMinutes: total minutes parked
// @return fee in dollars ($10 per hour, minimum $10)
// ────────────────────────────────────────────────────────────────────────────
double BikePricing::calculateFee(long long durationMinutes) const {
    // Convert minutes to fractional hours and round up:
    // e.g., 10 mins / 60.0 = 0.166... -> ceil() gives 1.0 hour
    // e.g., 75 mins / 60.0 = 1.25 -> ceil() gives 2.0 hours
    double hours = std::ceil(durationMinutes / 60.0);

    // Enforce minimum 1-hour charge (handles 0 minutes edge case in fast tests)
    if (hours < 1.0) {
        hours = 1.0;
    }

    // Multiply by hourly bike rate ($10/hr)
    return hours * 10.0;
}

