// ============================================================================
// FILE: PricingStrategy.h
// PURPOSE: Abstract interface for calculating parking fees.
//
// CONCEPT: This is the STRATEGY PATTERN in action. Instead of writing a
//          giant if-else block like:
//              if (type == BIKE) fee = duration * 10;
//              else if (type == CAR) fee = duration * 20;
//              ...
//
//          We create a family of interchangeable algorithms (strategies),
//          each encapsulated in its own class. The system picks the right
//          strategy at runtime via the PricingFactory.
//
//          Analogy: Think of PricingStrategy like a "calculator plugin".
//          The parking lot says "here's the duration, tell me the fee",
//          and different plugins (BikePricing, CarPricing, TruckPricing)
//          each compute it differently.
//
// WHY PURE VIRTUAL: Making calculateFee() = 0 forces every subclass to
//                   provide its own implementation. You can't accidentally
//                   create a PricingStrategy without defining the fee logic.
//
// DESIGN NOTE: Header-only file — interfaces have no .cpp implementation.
//              The virtual destructor ensures proper cleanup when deleting
//              through a base pointer (e.g., unique_ptr<PricingStrategy>).
// ============================================================================

#ifndef PRICING_STRATEGY_H   // Header guard: prevents double-inclusion
#define PRICING_STRATEGY_H   // Define the guard macro

// Abstract base class (interface) for all pricing strategies
// Each vehicle type gets its own concrete strategy with different rates
class PricingStrategy {

public:
    // Pure virtual method: calculates the parking fee for a given duration
    // @param durationMinutes — how long the vehicle was parked, in minutes
    // @return the fee in dollars (e.g., 20.0 for one hour of car parking)
    // = 0 makes this pure virtual — subclasses MUST override this method
    virtual double calculateFee(long long durationMinutes) const = 0;

    // Virtual destructor: ensures derived class destructors are called properly
    // Without this, deleting a BikePricing via a PricingStrategy* would leak memory
    // = default tells the compiler to generate the standard destructor body
    virtual ~PricingStrategy() = default;
};

#endif // PRICING_STRATEGY_H  // End of header guard
