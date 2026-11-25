// ============================================================================
// FILE: SpotType.h
// PURPOSE: Defines the types of parking spots available on each floor.
//
// CONCEPT: Each physical parking spot in the lot has a size category.
//          Think of it like T-shirt sizes — SMALL fits bikes, MEDIUM fits
//          cars, LARGE fits trucks. A vehicle can ONLY park in its matching
//          spot type. This keeps the mapping simple and predictable.
//
// MAPPING:
//     SMALL  ↔ BIKE    (compact spot, just enough for a two-wheeler)
//     MEDIUM ↔ CAR     (standard car-sized bay)
//     LARGE  ↔ TRUCK   (wide bay for trucks and large vehicles)
//
// DESIGN NOTE: Header-only file — enums don't need a .cpp implementation.
// ============================================================================

#ifndef SPOT_TYPE_H   // Header guard: prevents double-inclusion of this file
#define SPOT_TYPE_H   // Define the guard macro so subsequent includes are skipped

// Enum class representing the physical size categories of parking spots
enum class SpotType {
    SMALL,   // Compact spot — designed to fit bikes only
    MEDIUM,  // Standard spot — designed to fit cars only
    LARGE    // Wide spot — designed to fit trucks only
};

#endif // SPOT_TYPE_H  // End of header guard
