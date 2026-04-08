# Design a Movie Ticket Booking System

## Problem Statement

Design an object-oriented movie ticket booking system that handles cinema management, screening schedules, seat selection with tiered pricing, and ticket booking.

> "Imagine you're trying to book tickets for a blockbuster movie on a busy weekend. You browse available showtimes, select your preferred seats, and proceed to book them. Behind the scenes, the system is managing seat availability, tracking screenings, and calculating ticket costs."

---

## Requirements

### Functional Requirements

| # | Requirement |
|---|------------|
| 1 | Each cinema has a location and contains **multiple rooms** |
| 2 | Movies can have **multiple screenings** across different rooms, cinemas, and time slots |
| 3 | Each room has a **grid of seats** available for booking |
| 4 | Seats have varying **pricing strategies** (Normal, Premium, VIP) |
| 5 | Users can **search** for screenings of a movie and **book** available seats |
| 6 | A **ticket** represents a specific seat for a specific screening |
| 7 | Users can book **multiple tickets** in a single **order** |
| 8 | **Total cost** = sum of all selected seat prices |

### Non-Functional Requirements

- Fast searches for screenings
- Prevent double-booking of the same seat

---

## Core Objects

```
+----------+     +--------+     +--------+     +--------+
|  Movie   |     | Cinema |---->|  Room  |---->| Layout |
|----------|     |--------|     |--------|     |--------|
| - title  |     | - name |     | - num  |     | - rows |
| - genre  |     | - loc  |     | - layout|    | - cols |
| - duration|    | - rooms|     +--------+     | - seats|
+----------+     +--------+                    +--------+

+-----------+     +--------+     +--------+     +--------+
| Screening |---->| Ticket |---->| Order  |     |  Seat  |
|-----------|     |--------|     |--------|     |--------|
| - movie   |     | - scr  |     | - tickets|  | - num  |
| - room    |     | - seat |     | - date |    | - pricing|
| - start   |     | - price|     | - total|    +--------+
| - end     |     +--------+     +--------+
+-----------+

+------------------+     +--------------------+
| ScreeningManager |     | MovieBookingSystem |
|------------------|     |  (Facade)          |
| - byMovie map    |     |--------------------|
| - byScreening map|     | - movies, cinemas  |
| + addScreening   |     | - screeningManager |
| + getAvailable   |     | + bookTicket       |
+------------------+     +--------------------+
```

---

## Class Diagram

### Movie

```
    +---------------------------+
    |          Movie            |
    |---------------------------|
    | - title: string           |
    | - genre: string           |
    | - durationInMinutes: int  |
    |                           |
    | + getTitle()              |
    | + getGenre()              |
    | + getDurationInMinutes()  |
    +---------------------------+
```

**Design choice**: Movie is a standalone, immutable entity. The same Movie can be reused across multiple Screenings without data duplication.

### Seat + PricingStrategy (Strategy Pattern)

```
    +-------------------+         +------------------------+
    |       Seat        |         |   <<interface>>        |
    |-------------------|         |   PricingStrategy      |
    | - seatNumber      |-------->|------------------------|
    | - pricingStrategy  |        | + getPrice(): double   |
    |                   |         | + getTierName(): string|
    | + getSeatNumber() |         +------------------------+
    | + setPricing()    |            /       |        \
    +-------------------+           /        |         \
                          +----------+ +------------+ +---------+
                          |NormalRate| |PremiumRate | | VIPRate |
                          |----------|  |------------|  |---------|
                          | price=10 |  | price=15   |  | price=25|
                          +----------+  +------------+  +---------+
```

**Design choice**: Using the Strategy Pattern for pricing means:
- Adding a new tier (e.g., StudentRate) = one new class, zero changes to Seat
- The same seat can have different pricing at runtime (e.g., promotions)
- Alternative: embed price directly in Seat -- simpler but inflexible

### Layout

```
    +------------------------------------------+
    |                Layout                     |
    |------------------------------------------|
    | - rows: int                               |
    | - columns: int                            |
    | - seatsByNumber: Map<string, Seat*>       |
    | - seatsByPosition: Map<int, Map<int, Seat*>> |
    |                                          |
    | + getSeatByNumber(num): Seat*            |
    | + getSeatByPosition(row, col): Seat*     |
    | + getAllSeats(): vector<Seat*>            |
    +------------------------------------------+
```

**Design choice**: Uses nested map instead of 2D array for flexibility with irregular layouts (e.g., missing seats, variable row sizes). Both lookup methods provide O(1) access.

### Cinema and Room

```
    +------------------+         +------------------+
    |     Cinema       |         |      Room        |
    |------------------|         |------------------|
    | - name           |-------->| - roomNumber     |
    | - location       |  1..*   | - layout: Layout*|
    | - rooms          |         |                  |
    |                  |         | + getRoomNumber() |
    | + addRoom()      |         | + getLayout()    |
    +------------------+         +------------------+
```

**Design choice**: Composition -- Cinema contains Rooms, each Room contains a Layout. Separating Room from Layout allows rooms to share or customize seating arrangements.

### Screening

```
    +---------------------------+
    |        Screening          |
    |---------------------------|
    | - movie: Movie*           |
    | - room: Room*             |
    | - startTime: time_t       |
    | - endTime: time_t         |
    |                           |
    | + getMovie()              |
    | + getRoom()               |
    | + getDurationMinutes()    |
    +---------------------------+
```

**Design choice**: Screening combines *what* (Movie), *where* (Room), and *when* (start/end time) into a single entity. Separated from Movie because the same film can have many different showings.

### Ticket and Order

```
    +------------------+         +---------------------------+
    |     Ticket       |         |          Order            |
    |------------------|         |---------------------------|
    | - screening      |         | - tickets: vector<Ticket*>|
    | - seat           |         | - orderDate: time_t       |
    | - price (frozen) |         |                           |
    |                  |         | + addTicket()             |
    | + getPrice()     |         | + calculateTotalPrice()   |
    +------------------+         +---------------------------+
```

**Design choice**: Price is captured at booking time and stored in the Ticket, so it stays fixed even if the seat's pricing strategy changes later.

### ScreeningManager

```
    +-----------------------------------------------------+
    |              ScreeningManager                        |
    |-----------------------------------------------------|
    | - screeningsByMovie: Map<Movie*, vector<Screening*>> |
    | - ticketsByScreening: Map<Screening*, vector<Ticket*>>|
    |                                                     |
    | + addScreening(movie, screening)                    |
    | + addTicket(screening, ticket)                      |
    | + getScreeningsForMovie(movie): vector<Screening*>  |
    | + getTicketsForScreening(scr): vector<Ticket*>      |
    | + getAvailableSeats(scr): vector<Seat*>             |
    +-----------------------------------------------------+
```

**Design choice**: Centralized coordinator for scheduling and booking. Keeps Cinema class focused on static attributes (location, rooms) rather than coupling it with dynamic booking logic.

### MovieBookingSystem (Facade Pattern)

```
    +=============================================================+
    |              MovieBookingSystem (Facade)                     |
    |=============================================================|
    |                                                             |
    |   addMovie / addCinema          bookTicket(screening, seat) |
    |        |                              |                     |
    |        v                              v                     |
    |   Internal lists              +------------------+          |
    |   (movies, cinemas)           | ScreeningManager |          |
    |                               | .getAvailableSeats|         |
    |                               | .addTicket        |         |
    |                               +------------------+          |
    |                                       |                     |
    |                                       v                     |
    |                               Create Ticket with            |
    |                               frozen price from             |
    |                               seat.getPricingStrategy()     |
    +=============================================================+
```

**Design choice**: Facade pattern provides a single entry point for all client operations. Clients don't need to know about ScreeningManager or Layout internals.

---

## Seat Availability Algorithm

```
getAvailableSeats(screening):
    allSeats  = screening.room.layout.getAllSeats()
    booked    = ticketsByScreening[screening]

    available = []
    for seat in allSeats:
        if seat NOT IN booked tickets' seats:
            available.add(seat)

    return available
```

---

## Booking Flow

```
User selects a screening and a seat
        |
        v
MovieBookingSystem.bookTicket(screening, seat)
        |
        v
    Is seat in getAvailableSeats()?
       / \
     No   Yes
      |     |
      v     v
   FAIL   price = seat.pricingStrategy.getPrice()
           ticket = new Ticket(screening, seat, price)
           screeningManager.addTicket(screening, ticket)
           return ticket
```

---

## Data Structures Used

| Structure | Purpose | Time Complexity |
|-----------|---------|----------------|
| `Map<Movie*, vector<Screening*>>` | Screenings grouped by movie | O(1) movie lookup |
| `Map<Screening*, vector<Ticket*>>` | Tickets grouped by screening | O(1) screening lookup |
| `Map<string, Seat*>` | Seat lookup by number | O(1) |
| `Map<int, Map<int, Seat*>>` | Seat lookup by row/col position | O(1) |

---

## Design Patterns Used

### 1. Strategy Pattern
Encapsulates seat pricing (NormalRate, PremiumRate, VIPRate) into interchangeable classes. Adding a new pricing tier requires zero changes to existing code.

### 2. Facade Pattern
MovieBookingSystem provides a simple API (`bookTicket`, `getScreeningsForMovie`, `getAvailableSeats`) while hiding the complexity of ScreeningManager, Layout lookups, and ticket management.

### 3. Composition
Cinema contains Rooms, Rooms contain Layouts, Layouts contain Seats. Each level can be modified independently.

---

## Key C++ vs Java Differences

| Concept | Java | C++ |
|---------|------|-----|
| Interface | `interface PricingStrategy` | Abstract class with pure virtual methods |
| Collections | `ArrayList`, `HashMap` | `vector`, `unordered_map` |
| Null safety | `null` | `nullptr` |
| DateTime | `LocalDateTime`, `Duration` | `time_t` with `difftime()` |
| BigDecimal | `BigDecimal` | `double` |
| Stream API | `tickets.stream().map(...).reduce(...)` | Manual loop to sum prices |
| computeIfAbsent | `map.computeIfAbsent(key, k -> new ArrayList<>())` | `map[key].push_back(...)` (auto-creates entry) |
| Memory | Garbage collected | Stack allocation + raw pointers, `new`/`delete` for tickets |

---

## Extensibility

| Extension | How to Add |
|-----------|-----------|
| New pricing tier (e.g., Student) | Create `StudentRate : PricingStrategy` |
| Payment processing | Add `PaymentProcessor` class called from `bookTicket()` |
| Concurrency (double-booking) | Add `SeatLockManager` with pessimistic/optimistic locking |
| Multiple cinemas | Already supported -- add more Cinema objects with their own Rooms |
| Seat categories (wheelchair, couple) | Add properties to Seat or new Seat subclasses |

---

## How to Build and Run

```bash
g++ -std=c++17 -o movie_booking MovieTicketBooking.cpp
./movie_booking
```

---

## References

- ByteByteGo - Object-Oriented Design Interview
- Design Patterns: Elements of Reusable Object-Oriented Software (GoF)
