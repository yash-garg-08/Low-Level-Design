# Strategy Design Pattern

**Type:** Behavioral

## Intent

Define a family of algorithms, encapsulate each one, and make them interchangeable. Strategy lets the algorithm vary independently from the clients that use it.

## Example

A `Robot` whose walking and talking behaviors are injected as strategies:

- `WalkableRobot` (strategy interface) → `NormalWalk`, `NoWalk`
- `TalkableRobot` (strategy interface) → `NormalTalk`, `NoTalk`
- `Robot` (context) composes both behaviors and delegates to them
- Concrete robots: `Companion`, `WorkerRobot`

This favors **composition over inheritance** — behaviors can be mixed and matched (or swapped at runtime) without an explosion of subclasses.

## Class Diagram

```
+------------------+        +------------------+
| WalkableRobot    |        | TalkableRobot    |
| <<interface>>    |        | <<interface>>    |
+------------------+        +------------------+
| + walk()         |        | + talk()         |
+------------------+        +------------------+
   ^          ^                ^          ^
   |          |                |          |
+--------+ +--------+    +----------+ +--------+
|Normal  | | NoWalk |    |NormalTalk| | NoTalk |
|Walk    | |        |    |          | |        |
+--------+ +--------+    +----------+ +--------+

            +---------------------------+
            | Robot (context)           |
            +---------------------------+
            | - walkBehavior            |
            | - talkBehavior            |
            +---------------------------+
            | + walk()  -> delegates    |
            | + talk()  -> delegates    |
            | + projection() (abstract) |
            +---------------------------+
                  ^             ^
                  |             |
            +-----------+ +-------------+
            | Companion | | WorkerRobot |
            +-----------+ +-------------+
```

## How to Run

```bash
g++ -std=c++17 StrategyDesignPattern.cpp -o strategy
./strategy
```

Expected output:

```
Walking normally....
Talking normally....
Displaying companion robot....
--------------------------------------
Walking normally....
Not talking....
Displaying worker robot....
```
