# Design a Parking Lot

## Problem Statement

Design an object-oriented parking lot system that handles vehicle entry, exit, spot allocation, ticketing, and fee calculation.

> "Imagine you're arriving at a busy parking lot. At the entrance, you're issued a ticket. You drive in, find a spot suited to your vehicle's size, and park. When you leave, you present your ticket, the system calculates your fee, and the spot is freed for the next vehicle."

---

## Requirements

### Functional Requirements

| # | Requirement |
|---|------------|
| 1 | The parking lot has multiple spot types: **compact**, **regular**, and **oversized** |
| 2 | Supports three vehicle types: **motorcycles**, **cars**, and **trucks** |
| 3 | Spots are assigned based on **vehicle size** (best-fit allocation) |
| 4 | A **ticket** is issued at entry with vehicle details and entry time |
| 5 | Fee is calculated at exit based on **duration**, **vehicle size**, and **time of day** |

### Non-Functional Requirements

- System must scale to support large parking lots
- Must reliably track spot assignments and ticket details

---

## Core Objects

```
+------------------+     +------------------+     +------------------+
|     Vehicle      |     |   ParkingSpot    |     |      Ticket      |
|------------------|     |------------------|     |------------------|
| - licensePlate   |     | - spotNumber     |     | - ticketId       |
| - size           |     | - vehicle        |     | - vehicle        |
|                  |     | - size           |     | - parkingSpot    |
| + getLicensePlate |     |                  |     | - entryTime      |
| + getSize        |     | + isAvailable    |     | - exitTime       |
+------------------+     | + occupy         |     |                  |
                         | + vacate         |     | + calcDuration   |
                         | + getSpotNumber  |     +------------------+
                         | + getSize        |
                         +------------------+

+------------------+     +------------------+
| ParkingManager   |     |   ParkingLot     |
|------------------|     |  (Facade)        |
| - availableSpots |     |------------------|
| - vehicleToSpot  |     | - parkingManager |
|                  |     | - fareCalculator |
| + parkVehicle    |     |                  |
| + unparkVehicle  |     | + enterVehicle   |
| + findSpot       |     | + leaveVehicle   |
+------------------+     +------------------+
```

---

## Class Diagram

### Vehicle Hierarchy

```
              +---------------------+         +--------------+
              |    <<interface>>    |         |  VehicleSize  |
              |      Vehicle        |         |   (enum)      |
              |---------------------|         |--------------|
              | + getLicensePlate() |         |  SMALL       |
              | + getSize()        |         |  MEDIUM      |
              +---------------------+         |  LARGE       |
                /       |        \            +--------------+
               /        |         \
    +-----------+  +-----------+  +-----------+
    | Motorcycle|  |    Car    |  |   Truck   |
    |-----------|  |-----------|  |-----------|
    | - plate   |  | - plate   |  | - plate   |
    | size=SMALL|  | size=MED  |  | size=LARGE|
    +-----------+  +-----------+  +-----------+
```

**Design choice**: We use `getSize()` instead of `getType()`. The parking lot cares about *how much space* a vehicle needs, not its exact type. A truck and a van are both LARGE -- treated the same for parking. Adding an electric scooter? Just mark it SMALL.

### ParkingSpot Hierarchy

```
              +---------------------+
              |    <<interface>>    |
              |    ParkingSpot      |
              |---------------------|
              | + isAvailable()     |
              | + occupy(vehicle)   |
              | + vacate()          |
              | + getSpotNumber()   |
              | + getSize()         |
              +---------------------+
                /       |        \
               /        |         \
    +-----------+  +-------------+  +--------------+
    |CompactSpot|  | RegularSpot |  |OversizedSpot |
    |-----------|  |-------------|  |--------------|
    | size=SMALL|  | size=MEDIUM |  | size=LARGE   |
    +-----------+  +-------------+  +--------------+
```

**Design choice**: ParkingSpot is intentionally simple -- only manages its own state. Complex allocation logic lives in ParkingManager.

### Fare Strategy (Strategy Pattern)

```
    +------------------+        +------------------------+
    |  FareCalculator  |        |    <<interface>>       |
    |------------------|        |    FareStrategy        |
    | - strategies[]   |------->|------------------------|
    |                  |        | + calculateFare(ticket, |
    | + calculateFare()|        |     inputFare)         |
    +------------------+        +------------------------+
                                    /              \
                                   /                \
                    +------------------+   +------------------------+
                    | BaseFareStrategy |   | PeakHoursFareStrategy  |
                    |------------------|   |------------------------|
                    | SMALL_RATE = 1.0 |   | MULTIPLIER = 1.5       |
                    | MED_RATE   = 2.0 |   |                        |
                    | LARGE_RATE = 3.0 |   | Peak: 7-10am, 4-7pm   |
                    |                  |   |                        |
                    | fare = rate *    |   | if peak:               |
                    |   duration       |   |   fare *= 1.5          |
                    +------------------+   +------------------------+
```

**Design choice**: Using the Strategy Pattern allows swapping/adding pricing rules without modifying existing code. Want a weekend discount? Just add a `WeekendDiscountStrategy`.

### Complete System (Facade Pattern)

```
    +================================================================+
    |                      ParkingLot (Facade)                        |
    |================================================================|
    |                                                                  |
    |   enterVehicle(vehicle)              leaveVehicle(ticket)        |
    |       |                                   |                      |
    |       v                                   v                      |
    |  +-----------------+              +-----------------+            |
    |  | ParkingManager  |              | ParkingManager  |            |
    |  | .parkVehicle()  |              | .unparkVehicle()|            |
    |  +-----------------+              +-----------------+            |
    |       |                                   |                      |
    |       v                                   v                      |
    |  Create Ticket                     +-----------------+           |
    |  (id, vehicle,                     | FareCalculator  |           |
    |   spot, time)                      | .calculateFare()|           |
    |                                    +-----------------+           |
    +================================================================+
```

**Design choice**: ParkingLot is a lightweight facade. It doesn't do spot allocation or fare math itself -- it delegates to specialized classes. This keeps each class focused on a single responsibility.

---

## Spot Allocation Algorithm

```
findSpotForVehicle(vehicle):
    vehicleSize = vehicle.getSize()

    for size in [SMALL, MEDIUM, LARGE]:      // iterate upward
        if size >= vehicleSize:
            for spot in availableSpots[size]:
                if spot.isAvailable():
                    return spot              // best-fit: smallest that works

    return null                              // parking lot full for this size
```

**Example**: A motorcycle (SMALL) first looks for compact spots, then regular, then oversized. A truck (LARGE) can only use oversized spots.

---

## Fee Calculation Flow

```
FareCalculator.calculateFare(ticket):

    fare = 0.0
        |
        v
    +-------------------+
    | BaseFareStrategy  |    fare = 0 + (rate * duration)
    | rate = 2.0/min    |    e.g., car parked 60 min -> fare = 120.0
    +-------------------+
        |
        v
    +------------------------+
    | PeakHoursFareStrategy  |    if entry during peak hours:
    | multiplier = 1.5       |        fare = 120.0 * 1.5 = 180.0
    +------------------------+    else:
        |                             fare = 120.0
        v
    Final fare returned
```

---

## Data Structures Used

| Structure | Purpose | Time Complexity |
|-----------|---------|----------------|
| `Map<VehicleSize, List<ParkingSpot>>` | Available spots grouped by size | O(1) size lookup |
| `Map<Vehicle, ParkingSpot>` | Which spot a vehicle occupies | O(1) vehicle lookup |
| `List<FareStrategy>` | Ordered chain of pricing rules | O(n) where n = strategies |

---

## Design Patterns Used

### 1. Strategy Pattern
Encapsulates pricing algorithms (BaseFareStrategy, PeakHoursFareStrategy) into interchangeable classes. FareCalculator chains them without knowing their internals.

### 2. Facade Pattern
ParkingLot provides a simple `enterVehicle()` / `leaveVehicle()` API while hiding the complexity of spot management, ticketing, and fare calculation behind the scenes.

---

## Key C++ vs Java Differences

| Concept | Java | C++ |
|---------|------|-----|
| Interface | `interface Vehicle` | `class Vehicle` with pure virtual methods |
| Polymorphism | Automatic via references | Raw pointers (`Vehicle*`) with non-owning semantics |
| Null | `null` | `nullptr` |
| DateTime | `LocalDateTime` | `time_t` with `difftime()` |
| BigDecimal | `BigDecimal` | `double` (sufficient for demo) |
| HashMap | `HashMap<K,V>` | `unordered_map<K,V>` |
| Enum | `enum VehicleSize` (unscoped) | `enum class VehicleSize` (scoped, type-safe, needs `VehicleSize::SMALL`) |
| Enum ordinal | `enum.ordinal()` | `static_cast<int>(VehicleSize::SMALL)` |
| Memory management | Garbage collected | Stack allocation + raw pointers (tickets use `new`/`delete`) |

---

## Extensibility

| Extension | How to Add |
|-----------|-----------|
| New vehicle type (e.g., Van) | Create `Van : Vehicle`, return `VehicleSize::LARGE` |
| New spot type (e.g., Handicapped) | Create `HandicappedSpot : ParkingSpot` |
| New fare rule (e.g., Weekend discount) | Create `WeekendFareStrategy : FareStrategy` |
| Multi-floor parking | Add a `Floor` class containing a `ParkingManager` per floor |

---

## How to Build and Run

```bash
# Compile
g++ -std=c++17 -o parking_lot ParkingLot.cpp

# Run
./parking_lot
```

---

## References

- ByteByteGo - Object-Oriented Design Interview
- Design Patterns: Elements of Reusable Object-Oriented Software (GoF)
