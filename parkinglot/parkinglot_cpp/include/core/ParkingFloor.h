// ============================================================================
// FILE: ParkingFloor.h
// PURPOSE: Represents one floor of the multi-story parking lot.
//
// CONCEPT: A ParkingFloor is like one level of a parking garage. Each floor
//          contains a mix of SMALL, MEDIUM, and LARGE spots. When a vehicle
//          needs to park, we search floor by floor (starting from floor 1)
//          to find the nearest available compatible spot.
//
//          The constructor takes how many of each spot type to create:
//              ParkingFloor(1, 5, 10, 3)  →  floor 1 with 5 small, 10 medium, 3 large
//
//          The floor OWNS its spots (stored by value in a vector), so when
//          the floor is destroyed, all spots are automatically cleaned up.
//
// KEY METHOD: findAvailableSpot() — scans through all spots on this floor
//             and returns a pointer to the first one that can fit the given
//             vehicle type. Returns nullptr if no compatible spot is free.
//
// DESIGN NOTE: Declaration only — implementation lives in src/ParkingFloor.cpp.
// ============================================================================

#ifndef PARKING_FLOOR_H   // Header guard: prevents double-inclusion
#define PARKING_FLOOR_H   // Define the guard macro

#include <vector>                    // For std::vector — stores all parking spots on this floor
#include "models/ParkingSpot.h"      // For ParkingSpot class — the spots this floor contains
#include "models/VehicleType.h"      // For VehicleType enum — used in findAvailableSpot()
#include "models/SpotType.h"         // For SpotType enum — used in getAvailableSpotCount()

// Represents one floor (level) of the parking lot, containing multiple spots
class ParkingFloor {

private:
    int floorNumber_;                // Which floor this is (1, 2, 3, etc.) — for display and tickets
    std::vector<ParkingSpot> spots_; // All parking spots on this floor, owned by this object
                                     // Stored by value — no manual memory management needed

public:
    // Constructor: creates a floor with the specified number of each spot type
    // @param floorNumber — the floor's level number (e.g., 1 for ground floor)
    // @param numSmall — how many SMALL (bike) spots to create on this floor
    // @param numMedium — how many MEDIUM (car) spots to create on this floor
    // @param numLarge — how many LARGE (truck) spots to create on this floor
    // Spot IDs are auto-assigned sequentially within the constructor
    ParkingFloor(int floorNumber, int numSmall, int numMedium, int numLarge);

    // Searches this floor for the first available spot that fits the given vehicle type
    // Returns a pointer to the matching spot, or nullptr if no compatible spot is free
    // The pointer points into the spots_ vector — valid as long as this floor exists
    ParkingSpot* findAvailableSpot(VehicleType vehicleType);

    // Prints the status of all spots on this floor (occupied vs free) to stdout
    // Useful for debugging and for the displayStatus() feature of ParkingLot
    void displayStatus() const;

    // Returns this floor's level number
    int getFloorNumber() const;

    // Counts how many spots of the given type are currently unoccupied on this floor
    // Useful for showing availability summaries (e.g., "3 car spots free on floor 2")
    int getAvailableSpotCount(SpotType type) const;

    // Finds and returns a pointer to the spot with the given ID on this floor
    // Returns nullptr if no spot with that ID exists on this floor
    ParkingSpot* getSpotById(int spotId);

    // Frees the parking spot with the given ID on this floor
    // Returns true if spot was found and freed, false if not found
    bool freeSpot(int spotId);
};

#endif // PARKING_FLOOR_H  // End of header guard
