// ============================================================================
// FILE: src/core/ParkingLot.cpp
// PURPOSE: Implementation of ParkingLot class (Singleton Pattern).
//
// WHAT THIS CLASS DOES:
//   The central manager for the entire parking system.
//   It:
//     - Enforces Singleton pattern (only one instance of ParkingLot)
//     - Maintains multi-floor layout
//     - Manages vehicle parking and issuing tickets
//     - Manages vehicle unparking, spot cleanup, and fee calculation
//     - Ensures thread safety using std::mutex and lock_guard
// ============================================================================

#include "core/ParkingLot.h"        // Class declaration
#include "pricing/PricingFactory.h" // Factory to create vehicle pricing strategies
#include <iostream>                 // std::cout

// ────────────────────────────────────────────────────────────────────────────
// Static member initialization
// ────────────────────────────────────────────────────────────────────────────
ParkingLot* ParkingLot::instance_ = nullptr;    // Starts as null before first call

// ────────────────────────────────────────────────────────────────────────────
// Private Constructor: enforces Singleton pattern
// ────────────────────────────────────────────────────────────────────────────
ParkingLot::ParkingLot(const std::string& name)
    : name_(name),                              // Set parking lot name
      nextTicketId_(1) {}                       // Start ticket counter from #1

// ────────────────────────────────────────────────────────────────────────────
// getInstance: static access point for the Singleton
// Creates the instance upon first invocation; returns existing pointer thereafter.
// ────────────────────────────────────────────────────────────────────────────
ParkingLot* ParkingLot::getInstance(const std::string& name) {
    if (instance_ == nullptr) {                 // Check if already created
        instance_ = new ParkingLot(name);       // Allocate on heap
    }
    return instance_;                           // Return single shared instance
}

// ────────────────────────────────────────────────────────────────────────────
// addFloor: adds a new level to the parking facility.
// ────────────────────────────────────────────────────────────────────────────
void ParkingLot::addFloor(int numSmall, int numMedium, int numLarge) {
    // Floor numbers are auto-assigned as 1, 2, 3...
    int newFloorNumber = static_cast<int>(floors_.size()) + 1;

    // Emplace a new floor with the given counts of small, medium, and large spots
    floors_.emplace_back(newFloorNumber, numSmall, numMedium, numLarge);

    std::cout << "Added Floor " << newFloorNumber << " with: "
              << numSmall << " Small, " << numMedium << " Medium, "
              << numLarge << " Large spots." << std::endl;
}

// ────────────────────────────────────────────────────────────────────────────
// parkVehicle: finds an available compatible spot across all floors.
// If found:
//   - Marks spot as occupied
//   - Creates and stores a Ticket in activeTickets_
//   - Returns pointer to the ticket
// If no spot is available or vehicle is invalid: returns nullptr.
// Thread-safe via lock_guard<mutex>.
// ────────────────────────────────────────────────────────────────────────────
Ticket* ParkingLot::parkVehicle(Vehicle* vehicle) {
    // RAII lock ensures thread safety during concurrent parking attempts
    std::lock_guard<std::mutex> lock(mtx_);

    // Guard against null pointers
    if (vehicle == nullptr) {
        std::cout << "[ERROR] Cannot park null vehicle!" << std::endl;
        return nullptr;
    }

    // Check if vehicle with this license plate is already parked
    if (activeTickets_.find(vehicle->getLicensePlate()) != activeTickets_.end()) {
        std::cout << "[ERROR] Vehicle " << vehicle->getLicensePlate()
                  << " is already inside the parking lot!" << std::endl;
        return nullptr;
    }

    // Search floors in sequential order (Floor 1 first, then Floor 2, etc.)
    for (auto& floor : floors_) {
        // Ask the floor for the first available spot matching the vehicle type
        ParkingSpot* spot = floor.findAvailableSpot(vehicle->getType());

        if (spot != nullptr) {
            // 1. Assign the vehicle to the spot
            spot->parkVehicle(vehicle);

            // 2. Generate a new ticket with auto-incremented ID
            int ticketId = nextTicketId_++;
            Ticket ticket(ticketId, vehicle->getLicensePlate(),
                          spot->getSpotId(), floor.getFloorNumber(),
                          vehicle->getType());

            // 3. Store ticket in activeTickets_ map for fast lookup on exit
            activeTickets_[vehicle->getLicensePlate()] = ticket;

            std::cout << "[PARKED] Vehicle " << vehicle->getLicensePlate()
                      << " -> Floor " << floor.getFloorNumber()
                      << ", Spot #" << spot->getSpotId()
                      << " (Ticket #" << ticketId << ")" << std::endl;

            // Return pointer to stored ticket in hashmap
            return &activeTickets_[vehicle->getLicensePlate()];
        }
    }

    // No compatible spot found on any floor
    std::cout << "[FULL] No available spot for vehicle "
              << vehicle->getLicensePlate() << std::endl;
    return nullptr;
}

// ────────────────────────────────────────────────────────────────────────────
// unparkVehicle: handles vehicle exit.
//   1. Finds ticket by license plate
//   2. Computes parked duration
//   3. Uses PricingFactory to calculate parking fee
//   4. Frees the assigned parking spot
//   5. Removes ticket from activeTickets_ map
//   6. Returns calculated fee (or -1.0 if not found)
// Thread-safe via lock_guard<mutex>.
// ────────────────────────────────────────────────────────────────────────────
double ParkingLot::unparkVehicle(const std::string& licensePlate) {
    // RAII lock for thread-safe exit processing
    std::lock_guard<std::mutex> lock(mtx_);

    // 1. Look up active ticket by license plate
    auto it = activeTickets_.find(licensePlate);
    if (it == activeTickets_.end()) {
        std::cout << "[ERROR] No active ticket found for license plate: "
                  << licensePlate << std::endl;
        return -1.0;                            // Indicator of failure
    }

    // Copy ticket details before removing from map
    Ticket ticket = it->second;

    // 2. Calculate duration parked
    long long durationMinutes = ticket.getParkedDurationInMinutes();

    // 3. Create pricing strategy using Factory Pattern
    auto pricingStrategy = PricingFactory::create(ticket.getVehicleType());

    // 4. Calculate total fee
    double fee = pricingStrategy->calculateFee(durationMinutes);

    // 5. Free the spot on the recorded floor
    for (auto& floor : floors_) {
        if (floor.getFloorNumber() == ticket.getFloorNumber()) {
            floor.freeSpot(ticket.getSpotId());
            break;                              // Spot freed, stop searching floors
        }
    }

    // 6. Remove ticket from active map
    activeTickets_.erase(it);

    std::cout << "[UNPARKED] Vehicle " << licensePlate
              << " vacated Floor " << ticket.getFloorNumber()
              << ", Spot #" << ticket.getSpotId()
              << " | Duration: " << durationMinutes << " min"
              << " | Total Fee: $" << fee << std::endl;

    return fee;                                 // Return fee to caller
}

// ────────────────────────────────────────────────────────────────────────────
// displayStatus: prints a summary of all floors and available spots.
// ────────────────────────────────────────────────────────────────────────────
void ParkingLot::displayStatus() const {
    std::cout << "\n================ " << name_ << " Status ================" << std::endl;
    std::cout << "Active Vehicles Parked: " << activeTickets_.size() << std::endl;
    for (const auto& floor : floors_) {
        floor.displayStatus();                  // Print floor-level breakdown
    }
    std::cout << "========================================================\n" << std::endl;
}

