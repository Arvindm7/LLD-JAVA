// ============================================================================
// FILE: src/models/ParkingSpot.cpp
// PURPOSE: Implementation of the ParkingSpot class.
//
// WHAT THIS CLASS DOES:
//   Represents one physical parking space on a floor.
//   It knows:
//     - Its ID and floor number
//     - Its size category (SMALL, MEDIUM, LARGE)
//     - Whether it's currently occupied
//     - A pointer to the vehicle parked here (if any)
//
// MATCHING RULE:
//   BIKE  -> SMALL spot
//   CAR   -> MEDIUM spot
//   TRUCK -> LARGE spot
// ============================================================================

#include "models/ParkingSpot.h"     // Class declaration
#include "models/Vehicle.h"         // Vehicle class (for getParkedVehicle return type)

// ────────────────────────────────────────────────────────────────────────────
// Constructor: initialize spot with ID, floor number, and size type.
// Starts out EMPTY (isOccupied = false, parkedVehicle = nullptr).
// ────────────────────────────────────────────────────────────────────────────
ParkingSpot::ParkingSpot(int spotId, int floorNumber, SpotType type)
    : spotId_(spotId),              // Store unique spot identifier
      floorNumber_(floorNumber),    // Store which floor this spot belongs to
      type_(type),                  // Store spot size category (SMALL / MEDIUM / LARGE)
      isOccupied_(false),           // Initially the spot is empty
      parkedVehicle_(nullptr) {}    // No vehicle is parked yet (null pointer)

// ────────────────────────────────────────────────────────────────────────────
// canFitVehicle: checks if this spot can accommodate the given vehicle type.
// Returns true ONLY IF:
//   1. The spot is currently FREE (!isOccupied_)
//   2. The vehicle type matches this spot type
// ────────────────────────────────────────────────────────────────────────────
bool ParkingSpot::canFitVehicle(VehicleType vehicleType) const {
    // If someone is already parked here, reject immediately
    if (isOccupied_) {
        return false;
    }

    // Strict 1-to-1 matching rule:
    // Bikes go to SMALL spots
    if (vehicleType == VehicleType::BIKE && type_ == SpotType::SMALL) {
        return true;
    }

    // Cars go to MEDIUM spots
    if (vehicleType == VehicleType::CAR && type_ == SpotType::MEDIUM) {
        return true;
    }

    // Trucks go to LARGE spots
    if (vehicleType == VehicleType::TRUCK && type_ == SpotType::LARGE) {
        return true;
    }

    // Incompatible size (e.g. truck trying to fit in a small spot)
    return false;
}

// ────────────────────────────────────────────────────────────────────────────
// parkVehicle: marks the spot as occupied and stores pointer to vehicle.
// Precondition: caller already verified canFitVehicle() is true.
// ────────────────────────────────────────────────────────────────────────────
void ParkingSpot::parkVehicle(Vehicle* vehicle) {
    isOccupied_ = true;             // Mark the spot as occupied
    parkedVehicle_ = vehicle;       // Store pointer to vehicle (we do NOT own it)
}

// ────────────────────────────────────────────────────────────────────────────
// removeVehicle: clears the spot when the vehicle exits.
// Resets occupied flag and sets vehicle pointer back to nullptr.
// ────────────────────────────────────────────────────────────────────────────
void ParkingSpot::removeVehicle() {
    isOccupied_ = false;            // Mark spot as available again
    parkedVehicle_ = nullptr;       // Clear the pointer
}

// ────────────────────────────────────────────────────────────────────────────
// Getters: simple accessors for spot properties
// ────────────────────────────────────────────────────────────────────────────
int ParkingSpot::getSpotId() const {
    return spotId_;                 // Return spot ID number
}

int ParkingSpot::getFloorNumber() const {
    return floorNumber_;            // Return floor number
}

SpotType ParkingSpot::getType() const {
    return type_;                   // Return spot size type
}

bool ParkingSpot::getIsOccupied() const {
    return isOccupied_;             // Return true if occupied, false if free
}

Vehicle* ParkingSpot::getParkedVehicle() const {
    return parkedVehicle_;          // Return pointer to vehicle (or nullptr)
}

