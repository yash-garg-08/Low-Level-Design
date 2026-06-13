# Factory Pattern

**Type:** Creational

## Intent

Separate **object creation** from **business logic**. Instead of scattering `new ConcreteClass()` calls throughout the code, a dedicated factory decides which concrete object to instantiate, so the client depends only on an abstract interface.

This folder builds the idea up in three progressive variants:

1. **Simple Factory** — a single factory class with a method that picks the concrete type.
2. **Factory Method** — an abstract factory interface with subclasses, each deciding what to create.
3. **Abstract Factory** — a factory that creates **families** of related products.

## Example

A burger shop. `Burger` is the abstract product; `StandardBurger`, `PremiumBurger`, `WheatStandardBurger`, `WheatPremiumBurger` are concrete products. The Abstract Factory variant adds a second product family, `GarlicBread`.

### 1. Simple Factory — `SimpleFactory.cpp`

One `BurgerFactory` with a `CreateBurger(type)` method. The client asks the factory for a burger by string type and never touches `new` directly.

- **Pro:** creation logic lives in one place.
- **Con:** adding a new burger means editing the factory (violates Open/Closed).

### 2. Factory Method — `FactoryMethod.cpp`

`BurgerFactory` becomes an **abstract** base. Concrete factories `SinghBurger` and `KingBurger` each override `CreateBurger()` and decide which product line to build (regular vs. wheat). New shops = new factory subclass, no edits to existing code.

### 3. Abstract Factory — `AbstractFactoryMethod.cpp`

`MealFactory` produces a **family** of related products — both a `Burger` and a `GarlicBread`. Each concrete factory (`SinghBurger`, `KingBurger`) guarantees the products it makes are consistent with each other (all regular, or all wheat).

## Class Diagrams

### Simple Factory

```
                +------------------+
                |  BurgerFactory   |
                +------------------+
                | + CreateBurger() |---- creates ----+
                +------------------+                 |
                                                     v
                +------------------+        +------------------+
                |     Burger       |        |     Burger       |
                | <<abstract>>     |        |  (abstract base) |
                +------------------+        +------------------+
                | + prepare()      |
                +------------------+
                   ^          ^
                   |          |
          +----------------+ +----------------+
          | StandardBurger | | PremiumBurger  |
          +----------------+ +----------------+
```

### Factory Method

```
        +---------------------+           +------------------+
        |   BurgerFactory     |           |     Burger       |
        |   <<abstract>>      |           |  <<abstract>>    |
        +---------------------+           +------------------+
        | + CreateBurger()=0  |  creates  | + prepare()      |
        +---------------------+ - - - - ->+------------------+
              ^          ^                   ^   ^   ^   ^
              |          |                   |   |   |   |
     +-------------+ +------------+   +--------+  |   |  +-----------------+
     | SinghBurger | | KingBurger |   |Standard|  |   |  |WheatPremiumBurger|
     +-------------+ +------------+   |Burger  |  |   |  +-----------------+
     | makes       | | makes      |   +--------+  |   |
     | regular     | | wheat      |       +-------+   +-------+
     | line        | | line       |       |Premium|   |WheatStandard|
     +-------------+ +------------+       |Burger |   |Burger       |
                                          +-------+   +-------------+
```

### Abstract Factory

```
                  +----------------------------+
                  |        MealFactory         |
                  |        <<abstract>>        |
                  +----------------------------+
                  | + CreateBurger()      = 0  |
                  | + CreateGarlicBread() = 0  |
                  +----------------------------+
                        ^                ^
                        |                |
              +-------------+      +------------+
              | SinghBurger |      | KingBurger |
              +-------------+      +------------+
                  | creates a consistent FAMILY |
                  v                             v
        +------------------+          +------------------+
        |     Burger       |          |   GarlicBread    |
        |   <<abstract>>   |          |   <<abstract>>   |
        +------------------+          +------------------+
        | + prepare()      |          | + prepare()      |
        +------------------+          +------------------+
           ^   ^   ^   ^                 ^   ^   ^   ^
        (Standard/Premium/            (Standard/Premium/
         Wheat variants)               Wheat variants)
```

## How to Run

```bash
# 1. Simple Factory
g++ -std=c++17 SimpleFactory.cpp -o simple && ./simple

# 2. Factory Method
g++ -std=c++17 FactoryMethod.cpp -o factory_method && ./factory_method

# 3. Abstract Factory
g++ -std=c++17 AbstractFactoryMethod.cpp -o abstract_factory && ./abstract_factory
```

Expected output:

```
# SimpleFactory  (type = "standard")
Standard burger .....

# FactoryMethod  (KingBurger, type = "standard")
Standard wheat burger ....

# AbstractFactoryMethod  (KingBurger, type = "standard")
Standard wheat burger ....
Wheat standard garlic bread ....
```
