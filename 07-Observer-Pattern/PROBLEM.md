# Observer Design Pattern

**Type:** Behavioral

## Intent

Define a one-to-many dependency between objects so that when one object (the **subject/observable**) changes state, all its dependents (**observers**) are notified and updated automatically.

> "Think of a YouTube channel. When a creator uploads a video, every subscriber gets a notification. The channel doesn't know or care who its subscribers are — it just broadcasts. Each subscriber decides what to do with the notification."

---

## When to Use

- An object's state change should trigger updates in an unknown or varying number of other objects
- You want loose coupling — the subject shouldn't depend on concrete observer types
- Pub/sub systems, event listeners, notification services

---

## Class Diagram

```
    +---------------------------+          +---------------------------+
    |       <<interface>>       |          |       <<interface>>       |
    |         Ichannel          |          |        Isubscriber        |
    |---------------------------|          |---------------------------|
    | + subscribe(s)            |          | + update()                |
    | + unSubscribe(s)          |          +---------------------------+
    | + notifySubscribers()     |                      ^
    +---------------------------+                      |
                ^                             +------------------+
                |                             |   Subscriber     |
       +------------------+                  |------------------|
       |     Channel      |----------------->| - name: string   |
       |------------------|   references     | - channel: Channel* |
       | - name: string   |                  |                  |
       | - latestVideo    |                  | + update()       |
       | - subscribers[]  |                  +------------------+
       |                  |
       | + subscribe()    |
       | + unSubscribe()  |
       | + notifySubscribers() |
       | + uploadVideo()  |
       | + getVideoData() |
       +------------------+
```

---

## UML Sequence Diagram

```
    sub1          sub2          channel
     |             |               |
     |---- subscribe(sub1) ------->|
     |             |               |
     |             |-- subscribe(sub2) -->|
     |             |               |
     |             | uploadVideo("first") |
     |             |               |-- notifySubscribers()
     |<-- update() --------------- |
     |  "Hey s1, Checkout..."      |
     |             |<-- update() --|
     |             | "Hey s2, ..." |
     |             |               |
     |---- unSubscribe(sub1) ----->|
     |             |               |
     |             | uploadVideo("second")|
     |             |               |-- notifySubscribers()
     |  (no update, unsubscribed)  |
     |             |<-- update() --|
     |             | "Hey s2, ..." |
```

---

## How It Works

```
Channel.uploadVideo(title):
    latestVideo = title
    print "[channel uploaded title]"
    notifySubscribers()
        |
        for each subscriber in subscribers[]:
            subscriber.update()
                |
                print "Hey <name>, " + channel.getVideoData()
```

**Subscribe/Unsubscribe:**

```
subscribe(s):
    if s not already in subscribers:
        subscribers.push_back(s)

unSubscribe(s):
    find s in subscribers
    if found: erase it
```

---

## Design Choices

| Decision | Choice | Reason |
|----------|--------|--------|
| Observer interface (`Isubscriber`) | single `update()` method | Keeps observers decoupled — Channel calls `update()` and each observer pulls what it needs via `getVideoData()` |
| Subject interface (`Ichannel`) | pure virtual subscribe/unsubscribe/notify | Allows swapping Channel implementations without breaking subscribers |
| Pull model | Subscriber calls `channel->getVideoData()` inside `update()` | Observer fetches only what it needs; Channel doesn't push data through `update()` arguments |
| Duplicate guard on `subscribe()` | `find()` before `push_back()` | Prevents a subscriber from being notified multiple times |

---

## Design Patterns Used

### Observer (Behavioral)
Subject (`Channel`) maintains a list of observers (`Subscriber`). On state change (`uploadVideo`), it iterates the list and calls `update()` on each — without knowing their concrete types.

---

## Key C++ vs Java Differences

| Concept | Java | C++ |
|---------|------|-----|
| Observer interface | `interface Observer` | Abstract class with pure virtual `update()` |
| Subject interface | `interface Observable` | Abstract class (`Ichannel`) |
| List of observers | `ArrayList<Observer>` | `vector<Isubscriber*>` |
| Remove observer | `list.remove(o)` | `find()` + `erase()` |
| Memory | Garbage collected | Raw pointers; `delete` needed in production |

---

## Extensibility

| Extension | How to Add |
|-----------|-----------|
| New subscriber type (e.g., EmailNotifier) | Create `EmailNotifier : Isubscriber`, implement `update()` |
| Multiple channels | Each `Channel` manages its own subscriber list independently |
| Event filtering | Pass an event type to `update(event)` so observers can ignore irrelevant events |
| Thread-safe notifications | Lock `subscribers` vector before iterating in `notifySubscribers()` |

---

## How to Build and Run

```bash
g++ -std=c++17 ObservableDesignPattern.cpp -o observer
./observer
```

Expected output:

```
[WarLord uploaded "This is my first video."]
Hey s1,
Checkout our new Video : This is my first video.

Hey s2,
Checkout our new Video : This is my first video.


[WarLord uploaded "This is my second video"]
Hey s2,
Checkout our new Video : This is my second video

```

`s1` unsubscribed before the second upload, so only `s2` is notified.

---

## References

- Design Patterns: Elements of Reusable Object-Oriented Software (GoF)
- ByteByteGo - Object-Oriented Design Interview
