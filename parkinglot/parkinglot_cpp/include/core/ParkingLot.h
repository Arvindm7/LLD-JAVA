// ============================================================================
// FILE: ParkingLot.h
// PURPOSE: The central class of the entire system — manages the parking lot.
//
// CONCEPT: ParkingLot is the "brain" that ties everything together. It:
//            1. Holds all floors (which hold all spots)
//            2. Issues tickets when vehicles enter (parkVehicle)
//            3. Calculates fees when vehicles leave (unparkVehicle)
//            4. Tracks all active tickets in a hashmap for O(1) lookup
//
// SINGLETON PATTERN: There's only ONE parking lot in the system, just like
//          a real building. We enforce this with a private constructor and
//          a static getInstance() method. The first call creates the lot;
//          all subsequent calls return the same instance.
//
//          Usage:
//              auto* lot = ParkingLot::getInstance("City Mall Parking");
//              lot->addFloor(5, 10, 3);   // 5 small, 10 medium, 3 large
//              lot->parkVehicle(&myCar);
//
// THREAD SAFETY: A mutex protects parkVehicle() and unparkVehicle() so
//                multiple threads (e.g., multiple entry/exit gates) don't
//                corrupt shared state.
//
// FLOW:
//   ENTRY: vehicle arrives → search floors for available spot → park it →
//          create ticket → store in activeTickets_ map → return ticket
//
//   EXIT:  license plate given → look up ticket in map → calculate duration →
//          use PricingFactory to get fee → remove vehicle from spot →
//          remove ticket from map → return fee
//
// DESIGN NOTE: Declaration only — implementation lives in src/ParkingLot.cpp.
// ============================================================================

#ifndef PARKING_LOT_H   // Header guard: prevents double-inclusion
#define PARKING_LOT_H   // Define the guard macro

#include <string>            // For std::string — parking lot name and license plate keys
#include <vector>            // For std::vector — stores all ParkingFloor objects
#include <unordered_map>     // For std::unordered_map — O(1) lookup of active tickets by plate
#include <mutex>             // For std::mutex — thread safety for concurrent park/unpark calls

#include "core/ParkingFloor.h"   // For ParkingFloor class — the floors this lot contains
#include "models/Ticket.h"       // For Ticket class — stored in the active tickets map
#include "models/Vehicle.h"      // For Vehicle class — the vehicles being parked

// The central parking lot class — Singleton pattern ensures only one instance exists
class ParkingLot {

private:
    std::string name_;       // Human-readable name of this parking lot (e.g., "City Mall Parking")

    std::vector<ParkingFloor> floors_;  // All floors in the lot, searched top-to-bottom for spots
                                        // Stored by value — lot owns its floors

    std::unordered_map<std::string, Ticket> activeTickets_;  // Maps license plate → active ticket
                                                              // Enables O(1) lookup when vehicle exits
                                                              // Key = licensePlate, Value = Ticket object

    int nextTicketId_;   // Auto-incrementing counter for assigning unique ticket IDs
                         // Starts at 1 and increases with each new ticket issued

    std::mutex mtx_;     // Mutex for thread safety — protects park/unpark from race conditions
                         // Needed because multiple entry/exit gates may operate concurrently

    // --- Singleton infrastructure ---

    // Private constructor: prevents external code from creating ParkingLot instances directly
    // Only getInstance() can call this — that's how Singleton enforces single-instance
    ParkingLot(const std::string& name);

    // Static pointer to the single ParkingLot instance (initialized to nullptr)
    // Lives for the entire program lifetime once created
    static ParkingLot* instance_;

public:
    // --- Singleton access ---

    // Returns the single ParkingLot instance, creating it on first call
    // @param name — name of the lot (only used during first creation, ignored after)
    // Default parameter allows calling getInstance() without arguments after first creation
    static ParkingLot* getInstance(const std::string& name = "Default Parking Lot");

    // --- Core operations ---

    // Attempts to park a vehicle in the lot
    // Searches all floors for the first available compatible spot
    // If found: parks the vehicle, creates a ticket, stores it, returns pointer to ticket
    // If lot is full (no compatible spot): returns nullptr
    // Thread-safe: locks the mutex during the entire operation
    Ticket* parkVehicle(Vehicle* vehicle);

    // Removes a vehicle from the lot and returns the calculated fee
    // Looks up the ticket by license plate, computes duration, applies pricing strategy
    // If found: removes vehicle from spot, deletes ticket from map, returns fee (>= 0)
    // If not found: returns -1.0 to indicate the license plate has no active ticket
    // Thread-safe: locks the mutex during the entire operation
    double unparkVehicle(const std::string& licensePlate);

    // Prints the status of all floors and spots to stdout
    // Shows which spots are occupied vs free — useful for monitoring and debugging
    void displayStatus() const;

    // Adds a new floor to the parking lot with the specified spot configuration
    // @param numSmall — number of SMALL (bike) spots on the new floor
    // @param numMedium — number of MEDIUM (car) spots on the new floor
    // @param numLarge — number of LARGE (truck) spots on the new floor
    // Floor number is auto-assigned based on current floor count + 1
    void addFloor(int numSmall, int numMedium, int numLarge);

    // --- Deleted copy/move operations (Singleton enforcement) ---

    // Delete copy constructor: prevents making copies of the singleton
    // Without this, someone could write: ParkingLot copy = *getInstance(); which breaks Singleton
    ParkingLot(const ParkingLot&) = delete;

    // Delete copy assignment: prevents assigning one lot to another
    // Without this, someone could write: lot1 = lot2; which breaks Singleton
    ParkingLot& operator=(const ParkingLot&) = delete;
};

#endif // PARKING_LOT_H  // End of header guard
