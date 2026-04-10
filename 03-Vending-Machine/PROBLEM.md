# Design a Vending Machine

## Problem Statement

Design an object-oriented vending machine system that allows users to insert money, select products, dispense items, and receive change. The system should manage inventory, process payments, and enforce a strict action sequence.

> "Imagine you're at a vending machine. You insert some cash, select your favorite item, and within seconds it drops into the tray. The machine gives you the right change if needed. Behind the scenes, the system is tracking inventory, handling payments, and making sure everything runs efficiently."

---

## Requirements

### Functional Requirements

| # | Requirement |
|---|------------|
| 1 | Vending machine supports **different product types** (snacks, beverages, etc.) |
| 2 | Products are stored in **racks**, each rack holds one product type with a count |
| 3 | Each product has a **unique code**, description, and price |
| 4 | Machine accepts **cash payments** and calculates **change** |
| 5 | Users follow: **Insert Money -> Select Product -> Dispense** |
| 6 | System checks **stock availability** and **sufficient funds** before dispensing |
| 7 | **Admins** can add/remove/update products in racks |

### Non-Functional Requirements

- Intuitive flow with clear error messages at each stage
- Prevent unauthorized access (only admins modify inventory)
- Secure cash transaction handling

---

## Core Objects

```
+------------------+     +------------------+     +------------------+
|    Product       |     |      Rack        |     |   Transaction    |
|------------------|     |------------------|     |------------------|
| - productCode    |     | - rackCode       |     | - rack           |
| - description    |     | - product        |     | - product        |
| - unitPrice      |     | - count          |     | - totalAmount    |
+------------------+     +------------------+     +------------------+

+------------------+     +------------------+     +------------------+
|InventoryManager  |     |PaymentProcessor  |     | VendingMachine   |
|------------------|     |------------------|     |  (Facade)        |
| - racks (dict)   |     | - currentBalance |     |------------------|
|                  |     |                  |     | - inventoryMgr   |
| + getProduct()   |     | + addBalance()   |     | - paymentProc    |
| + dispense()     |     | + charge()       |     | - currentState   |
| + updateRacks()  |     | + returnChange() |     | + insertMoney()  |
+------------------+     +------------------+     | + selectProduct()|
                                                  | + confirm()      |
                                                  +------------------+
```

---

## Class Diagram

### Product

```
    +---------------------------+
    |         Product           |
    |---------------------------|
    | - product_code: str       |
    | - description: str        |
    | - unit_price: float       |
    +---------------------------+
```

**Design choice**: Inventory quantity is NOT in Product. Product holds innate properties (code, description, price). Stock levels belong in InventoryManager since they change frequently.

### Rack

```
    +---------------------------+
    |          Rack             |
    |---------------------------|
    | - rack_code: str          |
    | - product: Product        |
    | - count: int              |
    |                           |
    | + get_product()           |
    | + get_product_count()     |
    +---------------------------+
```

**Design choice**: Rack doesn't have a `dispense()` method. Dispensing is delegated to InventoryManager (SRP). Rack only tracks its own state.

### InventoryManager

```
    +------------------------------------------+
    |          InventoryManager                |
    |------------------------------------------|
    | - racks: dict[str, Rack]                 |
    |                                          |
    | + update_racks(racks)                    |
    | + get_rack(rack_code): Rack              |
    | + get_product_in_rack(code): Product     |
    | + dispense_product_from_rack(rack)       |
    +------------------------------------------+
```

**Design choice**: Uses a `dict` (HashMap) for O(1) rack lookup by code. `update_racks()` allows bulk admin updates. `dispense_product_from_rack()` decrements count and would interface with hardware in a real system.

### PaymentProcessor

```
    +----------------------------------+
    |        PaymentProcessor          |
    |----------------------------------|
    | - current_balance: float         |
    |                                  |
    | + add_balance(amount)            |
    | + charge(amount)                 |
    | + return_change(): float         |
    | + get_current_balance(): float   |
    +----------------------------------+
```

### Transaction

```
    +---------------------------+
    |       Transaction         |
    |---------------------------|
    | - rack: Rack              |
    | - product: Product        |
    | - total_amount: float     |
    +---------------------------+
```

**Design choice**: Transaction is a data structure that tracks in-progress purchases. It bridges InventoryManager and PaymentProcessor without coupling them -- both reference transaction details independently.

### VendingMachineState (State Pattern)

```
    +--------------------------------------------+
    |       <<abstract>>                         |
    |       VendingMachineState                  |
    |--------------------------------------------|
    | + insert_money(vm, amount)                 |
    | + select_product(vm, rack_code)            |
    | + dispense_product(vm)                     |
    | + get_state_description(): str             |
    +--------------------------------------------+
              /           |            \
             /            |             \
+-------------------+ +------------------+ +------------------+
|NoMoneyInserted    | |MoneyInserted     | |Dispense          |
|State              | |State             | |State             |
|-------------------| |------------------| |------------------|
| insert: OK        | | insert: OK       | | insert: ERROR    |
| select: ERROR     | | select: OK       | | select: ERROR    |
| dispense: ERROR   | | dispense: ERROR  | | dispense: OK     |
|                   | |                  | |                  |
| -> MoneyInserted  | | -> Dispense      | | -> NoMoney       |
+-------------------+ +------------------+ +------------------+
```

**Design choice**: The State Pattern enforces the required action sequence:

```
Insert Money  ->  Select Product  ->  Dispense Product
     |                  |                    |
NoMoneyInserted   MoneyInserted        DispenseState
   State              State               |
                                    (resets to NoMoney)
```

Each state only allows its valid action and raises errors for everything else. This prevents:
- Selecting without paying
- Dispensing without selecting
- Double-inserting during dispense

### VendingMachine (Facade Pattern)

```
    +=============================================================+
    |              VendingMachine (Facade)                         |
    |=============================================================|
    |                                                             |
    |   insert_money(amt)   select_product(code)   confirm()     |
    |        |                    |                    |          |
    |        v                    v                    v          |
    |   +-----------------------------------------------------+  |
    |   |            Current VendingMachineState               |  |
    |   |  (delegates to NoMoney / MoneyInserted / Dispense)   |  |
    |   +-----------------------------------------------------+  |
    |        |                    |                    |          |
    |        v                    v                    v          |
    |  PaymentProcessor    InventoryManager    PaymentProcessor  |
    |  .add_balance()      .get_rack()         .charge()         |
    |                      .dispense()         .return_change()  |
    +=============================================================+
```

**Design choice**: VendingMachine is a lightweight facade. It delegates:
- Inventory -> InventoryManager
- Payments -> PaymentProcessor
- Action enforcement -> VendingMachineState

This prevents VendingMachine from becoming a "god object."

---

## Purchase Flow

```
User                    VendingMachine              State
  |                          |                        |
  |-- insert_money(2.00) -->|                        |
  |                          |-- insert_money() ---->| NoMoneyInserted
  |                          |   add_balance(2.00)   |
  |                          |   transition -------->| MoneyInserted
  |                          |                        |
  |-- select_product("A1")->|                        |
  |                          |-- select_product() -->| MoneyInserted
  |                          |   check stock: OK     |
  |                          |   check funds: OK     |
  |                          |   set transaction     |
  |                          |   transition -------->| Dispense
  |                          |                        |
  |-- confirm_transaction()->|                       |
  |                          |-- dispense_product()->| Dispense
  |                          |   charge(1.50)        |
  |                          |   dispense from rack  |
  |                          |   return change(0.50) |
  |                          |   save to history     |
  |                          |   transition -------->| NoMoneyInserted
  |                          |                        |
  |<-- product + change -----|                        |
```

---

## Error Handling

| Scenario | State | Error |
|----------|-------|-------|
| Select product without money | NoMoneyInserted | "Cannot select without inserting money" |
| Dispense without selection | MoneyInserted | "Please select a product first" |
| Insufficient funds | MoneyInserted | "Need $X, have $Y" |
| Out of stock | MoneyInserted | "Product is out of stock" |
| Insert money during dispense | Dispense | "Cannot insert during dispensing" |

---

## Data Structures Used

| Structure | Purpose | Time Complexity |
|-----------|---------|----------------|
| `dict[str, Rack]` | Rack lookup by code | O(1) |
| `list[Transaction]` | Transaction history | O(1) append |

---

## Design Patterns Used

### 1. State Pattern
Manages vending machine behavior as distinct states (NoMoneyInserted, MoneyInserted, Dispense). Each state defines which actions are valid, enforcing the required sequence and providing contextual error messages.

### 2. Facade Pattern
VendingMachine provides a simple API (insert_money, select_product, confirm_transaction) while hiding the complexity of InventoryManager, PaymentProcessor, and state management.

### 3. Single Responsibility Principle
- **Product**: stores product data only
- **Rack**: tracks count for one product type
- **InventoryManager**: manages all racks and dispensing
- **PaymentProcessor**: handles cash balance and change
- **VendingMachineState**: enforces action sequences

---

## Key Java vs Python Differences

| Concept | Java | Python |
|---------|------|--------|
| Interface | `interface VendingMachineState` | `ABC` with `@abstractmethod` |
| BigDecimal | `BigDecimal` for currency | `float` (sufficient for demo) |
| HashMap | `HashMap<String, Rack>` | `dict[str, Rack]` |
| Exception | `throw new IllegalStateException(...)` | `raise RuntimeError(...)` |
| Immutable list | `Collections.unmodifiableList()` | `list(self.history)` (shallow copy) |
| Access modifiers | `private`, `public` | Convention: `_prefix` for private |
| Enum | `enum` class | Not needed here (states are classes) |
| Null | `null` | `None` |

---

## Extensibility

| Extension | How to Add |
|-----------|-----------|
| Card payment | Add `CardPaymentProcessor` alongside `PaymentProcessor` |
| New product type | Just create a new `Product` instance -- no code changes |
| Discount/promo | Add a pricing strategy layer (Strategy Pattern on Product) |
| Multiple currencies | Extend `PaymentProcessor` with currency conversion |
| Audit logging | Add an observer on state transitions |
| Remote monitoring | Add network hooks in VendingMachine facade |

---

## How to Run

```bash
python3 vending_machine.py
```

---

## References

- ByteByteGo - Object-Oriented Design Interview
- Design Patterns: Elements of Reusable Object-Oriented Software (GoF)
