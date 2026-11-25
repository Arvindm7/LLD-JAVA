// ============================================================================
// FILE: Ticket.h
// PURPOSE: Represents the parking ticket issued when a vehicle enters the lot.
//
// CONCEPT: Think of this like the paper ticket you get at a mall parking.
//          It records:
//            - A unique ticket number (auto-incremented by the system)
//            - Which vehicle (license plate) parked
//            - Where it parked (spot ID + floor number)
//            - When it entered (entry timestamp)
//
//          When the vehicle exits, we use the entry time to calculate how
//          long it was parked, and then apply the pricing strategy to
//          compute the fee.
//
// KEY METHOD: getParkedDurationInMinutes() computes the time elapsed from
//             entry to NOW using C++ chrono. This duration is fed into
//             the PricingStrategy to get the final fee.
//
// DESIGN NOTE: Declaration only — implementation lives in src/Ticket.cpp.
// ============================================================================

#ifndef TICKET_H   // Header guard: prevents double-inclusion
#define TICKET_H   // Define the guard macro
    
#include <string>               // For std::string — stores the license plate
#include <chrono>               // For steady_clock and time_point — tracks entry time precisely
#include "models/VehicleType.h" // For VehicleType enum — records what kind of vehicle was parked

// Represents a parking ticket issued to a vehicle upon entry
class Ticket {

private:
    int ticketId_;              // Unique ticket number (auto-assigned by ParkingLot)
    std::string licensePlate_;  // License plate of the vehicle this ticket belongs to
    int spotId_;                // ID of the parking spot assigned to this vehicle
    int floorNumber_;           // Floor number where the assigned spot is located
    VehicleType vehicleType_;   // Type of vehicle (BIKE, CAR, TRUCK) — used to calculate fee on exit

    // Entry timestamp: recorded the moment the ticket is created
    // Using steady_clock because it's monotonic (never goes backward, unlike system_clock)
    // This ensures accurate duration calculation even if system time changes
    std::chrono::time_point<std::chrono::steady_clock> entryTime_;

public:
    // Default constructor: creates an empty/invalid ticket (needed for std::unordered_map::operator[])
    Ticket();

    // Constructor: creates a ticket and automatically records the current time as entry
    // @param ticketId — unique ticket number
    // @param licensePlate — vehicle license plate
    // @param spotId — assigned spot ID
    // @param floorNumber — assigned floor number
    // @param vehicleType — type of vehicle (BIKE, CAR, TRUCK)
    Ticket(int ticketId, std::string licensePlate, int spotId, int floorNumber, VehicleType vehicleType = VehicleType::CAR);

    // --- Getters: return ticket information ---

    // Returns the unique ticket ID number
    int getTicketId() const;

    // Returns the license plate string of the parked vehicle
    const std::string& getLicensePlate() const;

    // Returns the ID of the assigned parking spot
    int getSpotId() const;

    // Returns the floor number where the vehicle is parked
    int getFloorNumber() const;

    // Returns the type of vehicle (BIKE, CAR, TRUCK) stored on this ticket
    VehicleType getVehicleType() const;

    // Calculates how many minutes the vehicle has been parked
    // Computes: (current time - entry time) converted to minutes
    // This value is used by PricingStrategy to calculate the parking fee
    long long getParkedDurationInMinutes() const;
};

#endif // TICKET_H  // End of header guard
