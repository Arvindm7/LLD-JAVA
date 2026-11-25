// ============================================================================
// FILE: src/models/Ticket.cpp
// PURPOSE: Implementation of the Ticket class.
//
// WHAT THIS CLASS DOES:
//   Represents the receipt given to a driver upon entry.
//   It captures:
//     - Unique ticket ID
//     - Vehicle license plate and type
//     - Assigned floor and spot ID
//     - Precise entry timestamp (using std::chrono::steady_clock)
//
//   When exiting, getParkedDurationInMinutes() calculates how long
//   the vehicle stayed, which is used to calculate the parking fee.
// ============================================================================

#include "models/Ticket.h"          // Class declaration
#include <chrono>                   // chrono::steady_clock, duration_cast

// ────────────────────────────────────────────────────────────────────────────
// Default constructor: creates an invalid/empty ticket placeholder.
// Required by std::unordered_map::operator[] to compile.
// ────────────────────────────────────────────────────────────────────────────
Ticket::Ticket()
    : ticketId_(-1),                                // -1 indicates an invalid/unassigned ticket
      licensePlate_(""),                            // Empty license plate
      spotId_(-1),                                  // Invalid spot
      floorNumber_(-1),                             // Invalid floor
      vehicleType_(VehicleType::CAR),               // Default fallback type
      entryTime_(std::chrono::steady_clock::now()) {} // Current time as placeholder

// ────────────────────────────────────────────────────────────────────────────
// Parameterized constructor: issues a new ticket upon vehicle entry.
// Records the current moment as entryTime_ using steady_clock.
// ────────────────────────────────────────────────────────────────────────────
Ticket::Ticket(int ticketId, std::string licensePlate, int spotId, int floorNumber, VehicleType vehicleType)
    : ticketId_(ticketId),                          // Store auto-incremented ticket ID
      licensePlate_(std::move(licensePlate)),       // Move string into member for efficiency
      spotId_(spotId),                              // Store assigned spot ID
      floorNumber_(floorNumber),                    // Store assigned floor number
      vehicleType_(vehicleType),                    // Store vehicle type (BIKE / CAR / TRUCK)
      entryTime_(std::chrono::steady_clock::now()) {} // Record exact timestamp NOW

// ────────────────────────────────────────────────────────────────────────────
// Getters
// ────────────────────────────────────────────────────────────────────────────
int Ticket::getTicketId() const {
    return ticketId_;                               // Return ticket ID number
}

const std::string& Ticket::getLicensePlate() const {
    return licensePlate_;                           // Return const reference to license plate string
}

int Ticket::getSpotId() const {
    return spotId_;                                 // Return spot ID where car was parked
}

int Ticket::getFloorNumber() const {
    return floorNumber_;                            // Return floor number where car was parked
}

VehicleType Ticket::getVehicleType() const {
    return vehicleType_;                            // Return vehicle type (used for fee calculation)
}

// ────────────────────────────────────────────────────────────────────────────
// getParkedDurationInMinutes: calculates time elapsed since entry.
// Uses chrono::duration_cast<chrono::minutes>.
// ────────────────────────────────────────────────────────────────────────────
long long Ticket::getParkedDurationInMinutes() const {
    // Current time right now
    auto now = std::chrono::steady_clock::now();

    // Calculate time difference between now and entry timestamp
    auto duration = std::chrono::duration_cast<std::chrono::minutes>(now - entryTime_);

    // Return the count of whole minutes elapsed
    return duration.count();
}

