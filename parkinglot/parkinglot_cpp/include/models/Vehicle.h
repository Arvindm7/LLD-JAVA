// ============================================================================
// FILE: Vehicle.h
// PURPOSE: Represents a vehicle entering the parking lot.
//
// CONCEPT: A Vehicle is the "customer" of our parking system. Every vehicle
//          has two key properties:
//            1. A license plate (unique ID, like "KA-01-AB-1234")
//            2. A type (BIKE / CAR / TRUCK) that determines which spot it gets
//
//          Think of Vehicle as a person's ID card — it tells the system who
//          you are (license plate) and what category you belong to (type).
//
// DESIGN NOTE: This is a tiny data class, so inline getters are fine in the
//              header. No .cpp file needed — keeps things simple for interviews.
// ============================================================================

#ifndef VEHICLE_H   // Header guard: prevents this file from being included twice
#define VEHICLE_H   // Define the guard macro to mark this file as already included

#include <string>              // For std::string — stores the license plate text
#include "models/VehicleType.h" // For VehicleType enum — BIKE, CAR, or TRUCK

// Represents a single vehicle that wants to park in the lot
class Vehicle {

private:
    std::string licensePlate_;  // Unique identifier for the vehicle (e.g., "KA-01-AB-1234")
    VehicleType type_;          // What kind of vehicle this is (BIKE, CAR, or TRUCK)

public:
    // Constructor: creates a vehicle with its license plate and type
    // Uses std::move on the string to avoid unnecessary copying of the plate text
    Vehicle(std::string licensePlate, VehicleType type)
        : licensePlate_(std::move(licensePlate)),  // Move the string into the member (efficient)
          type_(type) {}                           // Copy the enum value (tiny, no need to move)

    // Returns a const reference to the license plate string
    // Why const ref? Avoids copying the string every time someone reads it
    const std::string& getLicensePlate() const {
        return licensePlate_;  // Return reference to the stored license plate
    }

    // Returns the type of vehicle (BIKE, CAR, or TRUCK)
    // Returns by value because enums are tiny (just an int under the hood)
    VehicleType getType() const {
        return type_;  // Return the vehicle's type for spot-matching logic
    }
};

#endif // VEHICLE_H  // End of header guard
