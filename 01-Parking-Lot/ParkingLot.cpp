#include <iostream>
#include <string>
#include <vector>
#include <unordered_map>
#include <ctime>
using namespace std;

// ============================================================================
// Enums
// ============================================================================
// VehicleSize is shared by both Vehicle and ParkingSpot.
// We use getSize() instead of getType() -- the system cares about how much
// space a vehicle needs, not its exact type. A truck and a van are both
// LARGE and treated the same for parking purposes.
// ============================================================================

enum class VehicleSize { SMALL, MEDIUM, LARGE };

string sizeToString(VehicleSize s) {
    switch (s) {
        case VehicleSize::SMALL:  return "SMALL";
        case VehicleSize::MEDIUM: return "MEDIUM";
        case VehicleSize::LARGE:  return "LARGE";
    }
    return "UNKNOWN";
}

// ============================================================================
// Vehicle (Abstract Base Class / Interface)
// ============================================================================
// In C++, we use an abstract class with pure virtual methods to replicate
// Java's interface. Concrete classes (Motorcycle, Car, Truck) each define
// their own size. This is the foundation for spot assignment and fare calc.
// ============================================================================

class Vehicle {
public:
    virtual ~Vehicle() = default;
    virtual string getLicensePlate() const = 0;
    virtual VehicleSize getSize() const = 0;
};

class Motorcycle : public Vehicle {
    string plate;
public:
    Motorcycle(string p) : plate(p) {}
    string getLicensePlate() const override { return plate; }
    VehicleSize getSize() const override { return VehicleSize::SMALL; }
};

class Car : public Vehicle {
    string plate;
public:
    Car(string p) : plate(p) {}
    string getLicensePlate() const override { return plate; }
    VehicleSize getSize() const override { return VehicleSize::MEDIUM; }
};

class Truck : public Vehicle {
    string plate;
public:
    Truck(string p) : plate(p) {}
    string getLicensePlate() const override { return plate; }
    VehicleSize getSize() const override { return VehicleSize::LARGE; }
};

// ========================== ParkingSpot ==========================
// Abstract base class for parking spots. Each concrete type (CompactSpot,
// RegularSpot, OversizedSpot) defines its capacity via getSize().
// The spot only manages its own state (occupied or not).
// Complex allocation logic is handled by ParkingManager.

class ParkingSpot {
public:
    virtual ~ParkingSpot() = default;
    virtual bool isAvailable() const = 0;
    virtual void occupy(Vehicle* vehicle) = 0;
    virtual void vacate() = 0;
    virtual int getSpotNumber() const = 0;
    virtual VehicleSize getSize() const = 0;
};

// CompactSpot: fits SMALL vehicles (motorcycles).
// Can also be assigned to larger vehicles if no better spot is available
// (handled by ParkingManager's best-fit algorithm).
class CompactSpot : public ParkingSpot {
    int spotNumber;
    Vehicle* vehicle;
public:
    CompactSpot(int num) : spotNumber(num), vehicle(nullptr) {}
    int getSpotNumber() const override { return spotNumber; }
    bool isAvailable() const override { return vehicle == nullptr; }
    void occupy(Vehicle* v) override { if (isAvailable()) vehicle = v; }
    void vacate() override { vehicle = nullptr; }
    VehicleSize getSize() const override { return VehicleSize::SMALL; }
};

// RegularSpot: fits MEDIUM vehicles (cars).
class RegularSpot : public ParkingSpot {
    int spotNumber;
    Vehicle* vehicle;
public:
    RegularSpot(int num) : spotNumber(num), vehicle(nullptr) {}
    int getSpotNumber() const override { return spotNumber; }
    bool isAvailable() const override { return vehicle == nullptr; }
    void occupy(Vehicle* v) override { if (isAvailable()) vehicle = v; }
    void vacate() override { vehicle = nullptr; }
    VehicleSize getSize() const override { return VehicleSize::MEDIUM; }
};

// OversizedSpot: fits LARGE vehicles (trucks).
class OversizedSpot : public ParkingSpot {
    int spotNumber;
    Vehicle* vehicle;
public:
    OversizedSpot(int num) : spotNumber(num), vehicle(nullptr) {}
    int getSpotNumber() const override { return spotNumber; }
    bool isAvailable() const override { return vehicle == nullptr; }
    void occupy(Vehicle* v) override { if (isAvailable()) vehicle = v; }
    void vacate() override { vehicle = nullptr; }
    VehicleSize getSize() const override { return VehicleSize::LARGE; }
};

// ============================================================================
// Ticket
// ============================================================================
// Represents a parking ticket issued when a vehicle enters.
// Stores the vehicle, assigned spot, and entry/exit times.
// Duration is calculated in minutes using difftime().
// The Ticket is a data container -- fee calculation is delegated to
// FareCalculator (keeping Ticket simple and focused).
// ============================================================================

class Ticket {
    string ticketId;
    Vehicle* vehicle;
    ParkingSpot* spot;
    time_t entryTime;
    time_t exitTime;
    bool exited;

public:
    Ticket(string id, Vehicle* v, ParkingSpot* s)
        : ticketId(id), vehicle(v), spot(s), entryTime(time(nullptr)), exitTime(0), exited(false) {}

    string getTicketId() const { return ticketId; }
    Vehicle* getVehicle() const { return vehicle; }
    ParkingSpot* getSpot() const { return spot; }
    time_t getEntryTime() const { return entryTime; }
    bool hasExited() const { return exited; }

    void setExitTime() {
        exitTime = time(nullptr);
        exited = true;
    }

    // Duration in minutes
    double getDurationMinutes() const {
        time_t end = exited ? exitTime : time(nullptr);
        return difftime(end, entryTime) / 60.0;
    }
};

// ============================================================================
// FareStrategy (Strategy Pattern) and Concrete Strategies
// ============================================================================
// The Strategy Pattern encapsulates different pricing rules into separate
// classes, making them interchangeable. Each strategy takes the current
// fare + ticket, and returns the modified fare.
//
// - BaseFareStrategy:      base fee = rate_per_minute * duration
// - PeakHoursFareStrategy: multiplies fare by 1.5x during peak hours
//
// Strategies are chained by FareCalculator in order (base first, then peak).
// To add a new rule (e.g., WeekendDiscount), just create a new subclass.
// ============================================================================

class FareStrategy {
public:
    virtual ~FareStrategy() = default;
    virtual double calculateFare(Ticket* ticket, double currentFare) = 0;
};

class BaseFareStrategy : public FareStrategy {
public:
    double calculateFare(Ticket* ticket, double currentFare) override {
        double rate;
        switch (ticket->getVehicle()->getSize()) {
            case VehicleSize::SMALL:  rate = 1.0; break;
            case VehicleSize::MEDIUM: rate = 2.0; break;
            case VehicleSize::LARGE:  rate = 3.0; break;
        }
        return currentFare + rate * ticket->getDurationMinutes();
    }
};

class PeakHoursFareStrategy : public FareStrategy {
    bool isPeakHour(time_t t) {
        tm* local = localtime(&t);
        int hour = local->tm_hour;
        return (hour >= 7 && hour <= 10) || (hour >= 16 && hour <= 19);
    }

public:
    double calculateFare(Ticket* ticket, double currentFare) override {
        if (isPeakHour(ticket->getEntryTime()))
            return currentFare * 1.5;
        return currentFare;
    }
};

// ============================================================================
// FareCalculator
// ============================================================================
// Orchestrates all fare strategies in sequence. Starts with fare = 0 and
// pipes through each strategy (e.g., base fare first, then peak adjustment).
// Order matters: BaseFareStrategy must come before PeakHoursFareStrategy.
// Uses a vector to preserve insertion order (unlike a set).
// ============================================================================

class FareCalculator {
    vector<FareStrategy*> strategies;

public:
    void addStrategy(FareStrategy* s) { strategies.push_back(s); }

    double calculateFare(Ticket* ticket) {
        double fare = 0.0;
        for (auto* strategy : strategies)
            fare = strategy->calculateFare(ticket, fare);
        return fare;
    }
};

// ============================================================================
// ParkingManager
// ============================================================================
// Manages spot allocation, tracking, and deallocation.
//
// Data structures:
//   - availableSpots[size] : list of free ParkingSpot* grouped by VehicleSize
//                            Allows quick lookup of spots that fit a vehicle.
//   - vehicleToSpot        : maps Vehicle* -> ParkingSpot* for O(1) lookup
//                            of which spot a vehicle is parked in.
//
// Allocation strategy (findSpot):
//   Iterates from the vehicle's size upward (SMALL -> MEDIUM -> LARGE),
//   returning the first available spot. This "best-fit" approach ensures
//   vehicles park in the smallest spot that fits them, optimizing space.
// ============================================================================

class ParkingManager {
    unordered_map<int, vector<ParkingSpot*>> availableSpots;
    unordered_map<Vehicle*, ParkingSpot*> vehicleToSpot;

public:
    void addSpot(ParkingSpot* spot) {
        availableSpots[static_cast<int>(spot->getSize())].push_back(spot);
    }

    // Find smallest available spot that fits the vehicle
    ParkingSpot* findSpot(Vehicle* vehicle) {
        int vehicleSize = static_cast<int>(vehicle->getSize());
        int largeSize   = static_cast<int>(VehicleSize::LARGE);

        for (int size = vehicleSize; size <= largeSize; size++) {
            for (auto* spot : availableSpots[size]) {
                if (spot->isAvailable())
                    return spot;
            }
        }
        return nullptr;
    }

    ParkingSpot* parkVehicle(Vehicle* vehicle) {
        ParkingSpot* spot = findSpot(vehicle);
        if (!spot) return nullptr;

        spot->occupy(vehicle);
        vehicleToSpot[vehicle] = spot;

        // Remove from available list
        auto& spots = availableSpots[static_cast<int>(spot->getSize())];
        spots.erase(remove(spots.begin(), spots.end(), spot), spots.end());
        return spot;
    }

    void unparkVehicle(Vehicle* vehicle) {
        auto it = vehicleToSpot.find(vehicle);
        if (it == vehicleToSpot.end()) return;

        ParkingSpot* spot = it->second;
        spot->vacate();
        availableSpots[static_cast<int>(spot->getSize())].push_back(spot);
        vehicleToSpot.erase(it);
    }
};

// ============================================================================
// ParkingLot (Facade Pattern)
// ============================================================================
// Acts as the central entry point for the system. Clients only interact
// with ParkingLot -- it delegates:
//   - Spot allocation/deallocation -> ParkingManager
//   - Fee computation              -> FareCalculator
//
// This keeps the facade lightweight and each subsystem focused on one job
// (Single Responsibility Principle). Adding features (e.g., multi-floor)
// only requires changes in the subsystems, not the facade.
// ============================================================================

class ParkingLot {
    ParkingManager manager;
    FareCalculator calculator;
    int ticketCounter = 0;

public:
    void addSpot(ParkingSpot* spot) { manager.addSpot(spot); }
    void addFareStrategy(FareStrategy* s) { calculator.addStrategy(s); }

    Ticket* enterVehicle(Vehicle* vehicle) {
        ParkingSpot* spot = manager.parkVehicle(vehicle);
        if (!spot) {
            cout << "[ENTRY FAILED] No spot for " << vehicle->getLicensePlate() << endl;
            return nullptr;
        }

        string id = "TKT-" + to_string(++ticketCounter);
        Ticket* ticket = new Ticket(id, vehicle, spot);

        cout << "[ENTRY] " << vehicle->getLicensePlate()
             << " (" << sizeToString(vehicle->getSize()) << ")"
             << " -> Spot #" << spot->getSpotNumber()
             << " | " << id << endl;
        return ticket;
    }

    double leaveVehicle(Ticket* ticket) {
        if (!ticket || ticket->hasExited()) {
            cout << "[EXIT FAILED] Invalid ticket" << endl;
            return 0.0;
        }

        ticket->setExitTime();
        manager.unparkVehicle(ticket->getVehicle());
        double fare = calculator.calculateFare(ticket);

        cout << "[EXIT]  " << ticket->getVehicle()->getLicensePlate()
             << " | Duration: " << ticket->getDurationMinutes() << " min"
             << " | Fare: $" << fare
             << " | " << ticket->getTicketId() << endl;
        return fare;
    }
};

// ============================================================================
// Main -- Demo
// ============================================================================

int main() {
    // Create spots (using the 3 concrete subclasses)
    vector<CompactSpot> compactSpots;
    for (int i = 1; i <= 5; i++) compactSpots.emplace_back(i);

    vector<RegularSpot> regularSpots;
    for (int i = 6; i <= 10; i++) regularSpots.emplace_back(i);

    vector<OversizedSpot> oversizedSpots;
    for (int i = 11; i <= 13; i++) oversizedSpots.emplace_back(i);

    // Create fare strategies
    BaseFareStrategy baseFare;
    PeakHoursFareStrategy peakFare;

    // Assemble parking lot
    ParkingLot lot;
    for (auto& s : compactSpots)  lot.addSpot(&s);
    for (auto& s : regularSpots)  lot.addSpot(&s);
    for (auto& s : oversizedSpots) lot.addSpot(&s);
    lot.addFareStrategy(&baseFare);
    lot.addFareStrategy(&peakFare);

    // Create vehicles
    Motorcycle moto("MOTO-001");
    Car car1("CAR-001"), car2("CAR-002");
    Truck truck("TRUCK-001");

    cout << "========== Parking Lot Simulation ==========" << endl << endl;

    // Enter
    Ticket* t1 = lot.enterVehicle(&moto);
    Ticket* t2 = lot.enterVehicle(&car1);
    Ticket* t3 = lot.enterVehicle(&car2);
    Ticket* t4 = lot.enterVehicle(&truck);
    cout << endl;

    // Exit
    lot.leaveVehicle(t1);
    lot.leaveVehicle(t2);
    lot.leaveVehicle(t3);
    lot.leaveVehicle(t4);
    cout << endl;

    // Re-enter after spots freed
    Car car3("CAR-003");
    Ticket* t5 = lot.enterVehicle(&car3);
    lot.leaveVehicle(t5);

    cout << endl << "========== Simulation Complete ==========" << endl;

    // Cleanup tickets
    delete t1; delete t2; delete t3; delete t4; delete t5;
    return 0;
}
