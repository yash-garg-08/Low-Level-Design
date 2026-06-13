# Singleton Design Pattern

**Type:** Creational

## Intent

Ensure a class has **only one instance** and provide a global point of access to it.

> "Some things in a system should exist exactly once — a config loader, a logger, a connection pool. The Singleton pattern guarantees that no matter how many times you ask for the object, you always get back the same one."

---

## When to Use

- A single shared resource must be coordinated system-wide (e.g., logger, thread pool, config)
- Instantiation is expensive and the object should be reused
- A global access point is needed without polluting the global namespace

---

## Variants

### 1. Lazy Initialization (implemented here)

The instance is created **only on first use**. If the class is never accessed, no memory is allocated.

```
getInstance():
    if instance == nullptr:
        instance = new Singleton()   // created only once
    return instance
```

- **Pro:** no allocation if never used
- **Con:** not thread-safe without additional locking

### 2. Eager Initialization

The instance is created at **program startup** (static variable initialization).

```cpp
Singleton* Singleton::instance = new Singleton();
```

- **Pro:** simple, inherently thread-safe at the static init level
- **Con:** allocates memory even if the class is never used

---

## Class Diagram

```
    +----------------------------------+
    |           Singleton              |
    |----------------------------------|
    | - instance: Singleton*  (static) |
    |                                  |
    | - Singleton()  (private ctor)    |
    |                                  |
    | + getInstance(): Singleton*      |
    +----------------------------------+
```

**Design choices:**

- **Private constructor** — prevents any code outside the class from calling `new Singleton()`.
- **Static instance pointer** — lives at class level, not per-object, so there is exactly one slot to hold the shared instance.
- **Static `getInstance()`** — the only way to reach the object; checks and creates on first call.

---

## How It Works

```
First call:  Singleton::getInstance()
                 |
                 v
          instance == nullptr?  YES
                 |
                 v
          instance = new Singleton()   <-- constructor runs once
                 |
                 v
          return instance   (address: 0x...)

Second call: Singleton::getInstance()
                 |
                 v
          instance == nullptr?  NO
                 |
                 v
          return instance   (same address: 0x...)
```

`s1 == s2` evaluates to `1` — both pointers point to the same object.

---

## Thread Safety Note

The lazy variant shown here is **not thread-safe**: two threads could both see `instance == nullptr` simultaneously and each call `new Singleton()`. In production C++ the fix is:

```cpp
// C++11+ — guaranteed thread-safe static local
static Singleton* getInstance() {
    static Singleton instance;   // initialized exactly once, thread-safe
    return &instance;
}
```

Or use `std::call_once` / `std::mutex` for the pointer-based approach.

---

## Design Patterns Used

### Singleton (Creational)
Controls instance creation at the class level. The class itself is responsible for its own lifecycle, rather than delegating that responsibility to the caller.

---

## How to Build and Run

```bash
g++ -std=c++17 Singleton.cpp -o singleton
./singleton
```

Expected output:

```
Singleton Constructor called
1
```

The constructor prints once (instance created on first `getInstance()` call). The second call returns the cached pointer. `s1 == s2` prints `1`.

---

## Extensibility

| Extension | How to Add |
|-----------|-----------|
| Thread-safe lazy init | Use `static local` or `std::call_once` |
| Reset for testing | Add a `resetInstance()` method (test-only) |
| Derived singletons | Template the base: `template<class T> class Singleton` |
| Logging singleton | Inherit from this base and add `log(message)` |

---

## References

- Design Patterns: Elements of Reusable Object-Oriented Software (GoF)
- Scott Meyers — *Effective C++*, Item 4 (static local for thread-safe singletons)
