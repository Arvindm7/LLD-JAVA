// ============================================================================
// FILE: ParkingSpot.h
// PURPOSE: Represents a single parking bay on a floor of the parking lot.
//
// CONCEPT: A ParkingSpot is like a numbered bay in a real garage. Each bay:
//            - Has a unique ID and belongs to a specific floor
//            - Has a size (SMALL / MEDIUM / LARGE) painted on the ground
//            - Is either empty (available) or occupied by a vehicle
//
//          The spot does NOT own the vehicle parked in it — it just holds a
//          pointer. Think of it like a parking bay holding a car: if the car
//          drives away, the bay still exists. The vehicle's lifetime is
//          managed elsewhere.
//
// KEY METHOD: canFitVehicle() checks if a vehicle type matches the spot type
//             (BIKE→SMALL, CAR→MEDIUM, TRUCK→LARGE). This is the core
//             matching logic of the entire system.
//
// DESIGN NOTE: Declaration only — implementation lives in src/ParkingSpot.cpp.
// ============================================================================

#ifndef PARKING_SPOT_H   // Header guard: prevents double-inclusion
#define PARKING_SPOT_H   // Define the guard macro

#include "models/SpotType.h"     // For SpotType enum — the size of this parking spot
#include "models/VehicleType.h"  // For VehicleType enum — used in canFitVehicle()

// Forward declaration: tells compiler Vehicle exists without needing its full definition
// Why? ParkingSpot only holds a pointer to Vehicle, so it doesn't need the full class
class Vehicle;

// Represents one physical parking bay on a floor of the lot
class ParkingSpot {

private:
    int spotId_;               // Unique identifier for this spot (e.g., spot #5 on floor 2)
    int floorNumber_;          // Which floor this spot is on (for display and ticket info)
    SpotType type_;            // Size category of this spot (SMALL, MEDIUM, or LARGE)
    bool isOccupied_;          // True if a vehicle is currently parked here, false if free
    Vehicle* parkedVehicle_;   // Pointer to the vehicle parked here (nullptr if empty)
                               // Raw pointer because ParkingSpot does NOT own the vehicle

public:
    // Constructor: creates an empty spot with given ID, floor, and size type
    ParkingSpot(int spotId, int floorNumber, SpotType type);

    // Checks if a vehicle of the given type can park in this spot
    // Returns true only if spot is free AND the types are compatible
    // Matching rule: BIKE→SMALL, CAR→MEDIUM, TRUCK→LARGE
    bool canFitVehicle(VehicleType vehicleType) const;

    // Parks the given vehicle in this spot (sets occupied flag and stores pointer)
    // Precondition: spot must be empty and vehicle type must be compatible
    void parkVehicle(Vehicle* vehicle);

    // Removes the vehicle from this spot (clears occupied flag and nulls pointer)
    // Called when a vehicle exits the parking lot
    void removeVehicle();

    // --- Getters: simple accessors for spot properties ---

    // Returns the unique spot ID number
    int getSpotId() const;

    // Returns which floor this spot is on
    int getFloorNumber() const;

    // Returns the size type of this spot (SMALL, MEDIUM, or LARGE)
    SpotType getType() const;

    // Returns whether this spot currently has a vehicle parked in it
    bool getIsOccupied() const;

    // Returns pointer to the parked vehicle, or nullptr if spot is empty
    Vehicle* getParkedVehicle() const;
};

#endif // PARKING_SPOT_H  // End of header guard
