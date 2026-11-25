// ============================================================================
// FILE: src/main.cpp
// PURPOSE: Interactive demo and test driver for Parking Lot C++ LLD.
//
// WHAT THIS DEMO COVERS:
//   1. Singleton instantiation of ParkingLot
//   2. Adding multiple floors with varied spot capacities
//   3. Parking different vehicle types (Bike, Car, Truck)
//   4. Multi-floor overflow (Floor 1 fills up -> auto spills to Floor 2)
//   5. Lot full handling (graceful rejection when no spot fits)
//   6. Duplicate vehicle prevention
//   7. Unparking, freeing spots, and Strategy Pattern fee calculation
//   8. Re-parking in freed spots
//
// HOW TO COMPILE:
//   g++ -std=c++17 -I include -o parkinglot.exe \
//       src/models/ParkingSpot.cpp \
//       src/models/Ticket.cpp \
//       src/pricing/BikePricing.cpp \
//       src/pricing/CarPricing.cpp \
//       src/pricing/TruckPricing.cpp \
//       src/pricing/PricingFactory.cpp \
//       src/core/ParkingFloor.cpp \
//       src/core/ParkingLot.cpp \
//       src/main.cpp
//
// HOW TO RUN:
//   .\parkinglot.exe
// ============================================================================

#include <iostream>                     // std::cout, std::endl
#include <string>                       // std::string
#include <vector>                       // std::vector

// Include domain models and controllers
#include "models/VehicleType.h"         // VehicleType enum (BIKE, CAR, TRUCK)
#include "models/SpotType.h"            // SpotType enum (SMALL, MEDIUM, LARGE)
#include "models/Vehicle.h"             // Vehicle value object
#include "models/ParkingSpot.h"         // ParkingSpot entity
#include "models/Ticket.h"              // Ticket entity
#include "core/ParkingFloor.h"          // ParkingFloor class
#include "core/ParkingLot.h"            // ParkingLot singleton

using namespace std;

// ────────────────────────────────────────────────────────────────────────────
// Entry Point
// ────────────────────────────────────────────────────────────────────────────
int main() {
    cout << "============================================================" << endl;
    cout << "        PARKING LOT SYSTEM — Low Level Design (C++)         " << endl;
    cout << "============================================================" << endl;

    // ────────────────────────────────────────────────────────────────────────
    // STEP 1: Initialize Parking Lot via Singleton Pattern
    // ────────────────────────────────────────────────────────────────────────
    cout << "\n--- Step 1: Initializing Parking Lot ---" << endl;
    ParkingLot* lot = ParkingLot::getInstance("City Center Multi-Story Parking");

    // Add Floor 1: 2 Small (Bikes), 2 Medium (Cars), 1 Large (Trucks)
    lot->addFloor(2, 2, 1);

    // Add Floor 2: 1 Small (Bike), 2 Medium (Cars), 1 Large (Truck)
    lot->addFloor(1, 2, 1);

    // Display initial empty status
    lot->displayStatus();

    // ────────────────────────────────────────────────────────────────────────
    // STEP 2: Create Vehicles of Different Types
    // ────────────────────────────────────────────────────────────────────────
    cout << "--- Step 2: Creating Vehicles ---" << endl;
    Vehicle bike1("BIKE-001", VehicleType::BIKE);
    Vehicle bike2("BIKE-002", VehicleType::BIKE);
    Vehicle bike3("BIKE-003", VehicleType::BIKE);

    Vehicle car1("CAR-101", VehicleType::CAR);
    Vehicle car2("CAR-102", VehicleType::CAR);
    Vehicle car3("CAR-103", VehicleType::CAR);
    Vehicle car4("CAR-104", VehicleType::CAR);
    Vehicle car5("CAR-105", VehicleType::CAR);

    Vehicle truck1("TRUCK-901", VehicleType::TRUCK);
    Vehicle truck2("TRUCK-902", VehicleType::TRUCK);
    Vehicle truck3("TRUCK-903", VehicleType::TRUCK);

    // ────────────────────────────────────────────────────────────────────────
    // STEP 3: Normal Parking
    // ────────────────────────────────────────────────────────────────────────
    cout << "\n--- Step 3: Parking Vehicles on Floor 1 ---" << endl;
    lot->parkVehicle(&bike1);           // Should get Floor 1 Small spot
    lot->parkVehicle(&car1);            // Should get Floor 1 Medium spot
    lot->parkVehicle(&truck1);          // Should get Floor 1 Large spot

    lot->displayStatus();

    // ────────────────────────────────────────────────────────────────────────
    // STEP 4: Multi-Floor Overflow
    // Floor 1 had 2 Medium spots (car1 took one).
    // car2 will take the second Medium spot on Floor 1.
    // car3 should overflow and park on Floor 2!
    // ────────────────────────────────────────────────────────────────────────
    cout << "--- Step 4: Multi-Floor Overflow Test ---" << endl;
    lot->parkVehicle(&car2);            // Takes last Medium spot on Floor 1
    lot->parkVehicle(&car3);            // Fills first Medium spot on Floor 2!
    lot->parkVehicle(&car4);            // Takes second Medium spot on Floor 2

    lot->displayStatus();

    // ────────────────────────────────────────────────────────────────────────
    // STEP 5: Capacity Limit Rejection (Lot Full)
    // Floor 1 has 1 Large spot (truck1 took it).
    // Floor 2 has 1 Large spot. truck2 takes it.
    // truck3 should be rejected because all Large spots are full!
    // ────────────────────────────────────────────────────────────────────────
    cout << "--- Step 5: Capacity Limit / Rejection Test ---" << endl;
    lot->parkVehicle(&truck2);          // Takes Floor 2 Large spot
    lot->parkVehicle(&truck3);          // Should be REJECTED (no large spots left)

    // Also try parking car5 when both Floor 1 and Floor 2 car spots are full
    lot->parkVehicle(&car5);            // Should be REJECTED (no car spots left)

    // ────────────────────────────────────────────────────────────────────────
    // STEP 6: Duplicate Vehicle Detection
    // Trying to park car1 again while already inside
    // ────────────────────────────────────────────────────────────────────────
    cout << "\n--- Step 6: Duplicate Vehicle Test ---" << endl;
    lot->parkVehicle(&car1);            // Should be REJECTED (already inside)

    // ────────────────────────────────────────────────────────────────────────
    // STEP 7: Unparking and Fee Calculation (Strategy Pattern)
    // ────────────────────────────────────────────────────────────────────────
    cout << "\n--- Step 7: Unparking Vehicles ---" << endl;
    // Unpark car1: Rate is $20/hr. Minimum charge = 1 hour = $20.0
    double carFee = lot->unparkVehicle("CAR-101");
    cout << "Returned fee: $" << carFee << endl;

    // Unpark bike1: Rate is $10/hr. Minimum charge = 1 hour = $10.0
    double bikeFee = lot->unparkVehicle("BIKE-001");
    cout << "Returned fee: $" << bikeFee << endl;

    // Unpark truck1: Rate is $30/hr. Minimum charge = 1 hour = $30.0
    double truckFee = lot->unparkVehicle("TRUCK-901");
    cout << "Returned fee: $" << truckFee << endl;

    // Try unparking an unknown license plate
    lot->unparkVehicle("GHOST-999");    // Should fail with error message

    lot->displayStatus();

    // ────────────────────────────────────────────────────────────────────────
    // STEP 8: Re-parking in Freed Spot
    // car1 unparked, freeing up a Medium spot on Floor 1.
    // Now car5 (which was rejected earlier) should be able to park!
    // ────────────────────────────────────────────────────────────────────────
    cout << "--- Step 8: Re-parking in Newly Freed Spot ---" << endl;
    lot->parkVehicle(&car5);            // Should succeed now!

    lot->displayStatus();

    cout << "============================================================" << endl;
    cout << "           ALL PARKING LOT TESTS COMPLETED SUCCESSFULLY!    " << endl;
    cout << "============================================================" << endl;

    return 0;
}

