// ============================================================================
// FILE: src/core/ParkingFloor.cpp
// PURPOSE: Implementation of ParkingFloor class.
//
// WHAT THIS CLASS DOES:
//   Represents one level/floor of the parking structure.
//   It:
//     - Constructs spots with sequential spot IDs (1, 2, 3...)
//     - Finds the first available spot that fits a given vehicle type
//     - Frees spots when vehicles exit
//     - Displays floor availability status
// ============================================================================

#include "core/ParkingFloor.h"      // Class declaration
#include <iostream>                 // std::cout

// ────────────────────────────────────────────────────────────────────────────
// Constructor: builds all spots for this floor.
// Sequential IDs are assigned across all spot types on this floor.
// ────────────────────────────────────────────────────────────────────────────
ParkingFloor::ParkingFloor(int floorNumber, int numSmall, int numMedium, int numLarge)
    : floorNumber_(floorNumber) {   // Store floor number

    int nextSpotId = 1;             // Start spot IDs at 1 on each floor

    // 1. Create SMALL spots (intended for BIKES)
    for (int i = 0; i < numSmall; ++i) {
        spots_.emplace_back(nextSpotId++, floorNumber_, SpotType::SMALL);
    }

    // 2. Create MEDIUM spots (intended for CARS)
    for (int i = 0; i < numMedium; ++i) {
        spots_.emplace_back(nextSpotId++, floorNumber_, SpotType::MEDIUM);
    }

    // 3. Create LARGE spots (intended for TRUCKS)
    for (int i = 0; i < numLarge; ++i) {
        spots_.emplace_back(nextSpotId++, floorNumber_, SpotType::LARGE);
    }
}

// ────────────────────────────────────────────────────────────────────────────
// findAvailableSpot: scans spots on this floor in order, returning the first
// compatible and unoccupied spot. Returns nullptr if none available.
// ────────────────────────────────────────────────────────────────────────────
ParkingSpot* ParkingFloor::findAvailableSpot(VehicleType vehicleType) {
    // Iterate by reference (&) to avoid copying the spot objects
    for (auto& spot : spots_) {
        // canFitVehicle() checks both that the spot is empty AND types match
        if (spot.canFitVehicle(vehicleType)) {
            return &spot;           // Return pointer directly into spots_ vector
        }
    }
    return nullptr;                 // No matching available spot on this floor
}

// ────────────────────────────────────────────────────────────────────────────
// getSpotById: finds a spot on this floor by its numeric ID.
// ────────────────────────────────────────────────────────────────────────────
ParkingSpot* ParkingFloor::getSpotById(int spotId) {
    for (auto& spot : spots_) {
        if (spot.getSpotId() == spotId) {
            return &spot;           // Found spot matching ID
        }
    }
    return nullptr;                 // Spot not found on this floor
}

// ────────────────────────────────────────────────────────────────────────────
// freeSpot: clears a parking spot on this floor by spot ID.
// ────────────────────────────────────────────────────────────────────────────
bool ParkingFloor::freeSpot(int spotId) {
    ParkingSpot* spot = getSpotById(spotId);
    if (spot != nullptr && spot->getIsOccupied()) {
        spot->removeVehicle();      // Reset spot to free state
        return true;                // Successfully freed
    }
    return false;                   // Spot wasn't found or wasn't occupied
}

// ────────────────────────────────────────────────────────────────────────────
// getAvailableSpotCount: counts unoccupied spots of a given SpotType.
// ────────────────────────────────────────────────────────────────────────────
int ParkingFloor::getAvailableSpotCount(SpotType type) const {
    int freeCount = 0;
    for (const auto& spot : spots_) {
        // Check if matching type and currently empty
        if (spot.getType() == type && !spot.getIsOccupied()) {
            freeCount++;
        }
    }
    return freeCount;
}

// ────────────────────────────────────────────────────────────────────────────
// getFloorNumber: returns this floor's number.
// ────────────────────────────────────────────────────────────────────────────
int ParkingFloor::getFloorNumber() const {
    return floorNumber_;
}

// ────────────────────────────────────────────────────────────────────────────
// displayStatus: prints floor summary showing available spots per type.
// ────────────────────────────────────────────────────────────────────────────
void ParkingFloor::displayStatus() const {
    int smallFree = getAvailableSpotCount(SpotType::SMALL);
    int medFree   = getAvailableSpotCount(SpotType::MEDIUM);
    int largeFree = getAvailableSpotCount(SpotType::LARGE);

    std::cout << "  Floor " << floorNumber_ << " Available: "
              << "[Small (Bike): " << smallFree << "] "
              << "[Medium (Car): " << medFree << "] "
              << "[Large (Truck): " << largeFree << "]"
              << std::endl;
}

