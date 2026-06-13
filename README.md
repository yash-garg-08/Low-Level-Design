# Low-Level Design Problems in C++ & Python

A collection of classic **Low-Level Design (LLD)** / **Object-Oriented Design (OOD)** problems implemented in **C++17** and **Python 3**.

Each problem includes:
- A complete, runnable **implementation** (C++ or Python)
- A detailed **PROBLEM.md** with requirements, class diagrams (ASCII), design patterns used, and explanation

---

## Problems

| # | Problem | Design Patterns | Key Concepts |
|---|---------|----------------|--------------|
| 01 | [Parking Lot](01-Parking-Lot/) | Strategy, Facade | Enums, Polymorphism, HashMap |
| 02 | [Movie Ticket Booking](02-Movie-Ticket-Booking/) | Strategy, Facade, Composition | Nested Maps, Grid Layout, Order Management |
| 03 | [Vending Machine](03-Vending-Machine/) | State, Facade, SRP | State Machine, Inventory, Payment Processing |
| 04 | [Strategy Pattern](04-Strategy-Pattern/) | Strategy | Composition over Inheritance, Runtime Behavior Swapping |
| 05 | [Factory Pattern](05-Factory-Pattern/) | Simple Factory, Factory Method, Abstract Factory | Object Creation, Product Families, Open/Closed Principle |
| 06 | [Singleton Pattern](06-Singleton-Pattern/) | Singleton | Single Instance, Global Access Point, Lazy vs Eager Init |

---

## Repository Structure

```
Low-Level-Design/
├── README.md
├── 01-Parking-Lot/
│   ├── ParkingLot.cpp       # Complete C++ implementation
│   └── PROBLEM.md           # Problem explanation with diagrams
├── 02-Movie-Ticket-Booking/
│   ├── MovieTicketBooking.cpp
│   └── PROBLEM.md
├── 03-Vending-Machine/
│   ├── vending_machine.py     # Python implementation
│   └── PROBLEM.md
├── 04-Strategy-Pattern/
│   ├── StrategyDesignPattern.cpp
│   └── PROBLEM.md
├── 05-Factory-Pattern/
│   ├── SimpleFactory.cpp          # Simple Factory variant
│   ├── FactoryMethod.cpp          # Factory Method variant
│   ├── AbstractFactoryMethod.cpp  # Abstract Factory variant
│   └── PROBLEM.md
├── 06-Singleton-Pattern/
│   ├── Singleton.cpp
│   └── PROBLEM.md
└── ...
```

---

## How to Use

### C++ problems

```bash
cd 01-Parking-Lot
g++ -std=c++17 -o solution ParkingLot.cpp
./solution
```

### Python problems

```bash
cd 03-Vending-Machine
python3 vending_machine.py
```

### Multi-variant problems

Some folders demonstrate one pattern through several standalone files. Compile and run each independently:

```bash
cd 05-Factory-Pattern
g++ -std=c++17 SimpleFactory.cpp -o simple && ./simple
g++ -std=c++17 FactoryMethod.cpp -o factory_method && ./factory_method
g++ -std=c++17 AbstractFactoryMethod.cpp -o abstract_factory && ./abstract_factory
```

---

## Problem Wishlist

Here are classic LLD problems that can be added next:

- [ ] **Elevator System** -- State machine, scheduling algorithms
- [ ] **Library Management System** -- CRUD, search, observer pattern
- [ ] **Tic-Tac-Toe** -- Game loop, board state, strategy
- [x] **Vending Machine** -- State pattern, inventory management
- [ ] **Hotel Booking System** -- Reservation, concurrency
- [ ] **Snake and Ladder** -- Game simulation, random events
- [ ] **Chess** -- Piece hierarchy, move validation, command pattern
- [ ] **Splitwise (Expense Sharing)** -- Graph-based debt simplification
- [ ] **Online Shopping Cart** -- Cart management, discount strategies
- [x] **Movie Ticket Booking (BookMyShow)** -- Seat selection, concurrency
- [ ] **ATM Machine** -- State pattern, transaction chain
- [ ] **Car Rental System** -- Booking, pricing strategies
- [ ] **Food Delivery System (Zomato/Swiggy)** -- Order management, observer
- [x] **Singleton Pattern** -- lazy vs eager init, thread safety
- [ ] **Logging Framework** -- Singleton, chain of responsibility
- [ ] **Rate Limiter** -- Token bucket, sliding window

---

## Design Patterns Covered

As problems are added, this repo will cover:

| Pattern | Type | Used In |
|---------|------|---------|
| Strategy | Behavioral | Parking Lot (fare calculation), Movie Booking (seat pricing), Strategy Pattern (robot behaviors) |
| Facade | Structural | Parking Lot (ParkingLot class), Movie Booking (MovieBookingSystem), Vending Machine |
| Composition | Structural | Movie Booking (Cinema -> Room -> Layout -> Seat) |
| State | Behavioral | Vending Machine (NoMoney / MoneyInserted / Dispense states) |
| Simple Factory | Creational | Factory Pattern (BurgerFactory by type) |
| Factory Method | Creational | Factory Pattern (SinghBurger / KingBurger subclasses) |
| Abstract Factory | Creational | Factory Pattern (MealFactory — Burger + GarlicBread families) |
| Singleton | Creational | Singleton Pattern (lazy init, eager init, thread-safety tradeoffs) |
| Observer | Behavioral | *upcoming* |
| Command | Behavioral | *upcoming* |
| Chain of Responsibility | Behavioral | *upcoming* |

---

## Contributing

Feel free to add new problems! Each problem should follow this template:

1. Create a folder: `XX-Problem-Name/`
2. Add implementation file -- C++ (`.cpp`) or Python (`.py`) with a runnable demo
3. Add `PROBLEM.md` -- requirements, class diagrams, design patterns, explanation

---

## License

This repository is for educational purposes. Feel free to use it for interview preparation.
