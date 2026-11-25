// ============================================================================
// FILE: VehicleType.h
// PURPOSE: Defines the types of vehicles our parking lot can handle.
//
// CONCEPT: Think of this as a label on each vehicle telling the system
//          what kind it is — like a sticker that says "I'm a bike" or
//          "I'm a truck". The parking lot uses this label to find a
//          compatible spot (bike→small, car→medium, truck→large).
//
// WHY ENUM CLASS: enum class is type-safe — you can't accidentally mix
//                 VehicleType with integers or other enums. This prevents
//                 bugs like passing a random int where a vehicle type is
//                 expected.
//
// DESIGN NOTE: This is a header-only file because enums have no .cpp logic.
// ============================================================================

#ifndef VEHICLE_TYPE_H   // Header guard: prevents this file from being included twice
#define VEHICLE_TYPE_H   // If not yet defined, define it now and include the content

// Enum class representing the three kinds of vehicles the lot supports
enum class VehicleType {
    BIKE,    // Two-wheeler — gets the smallest and cheapest spot ($10/hr)
    CAR,     // Standard four-wheeler — gets a medium spot ($20/hr)
    TRUCK    // Large vehicle — needs the biggest spot ($30/hr)
};

#endif // VEHICLE_TYPE_H  // End of header guard
