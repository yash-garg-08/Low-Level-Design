#include <iostream>
#include <string>
#include <vector>
#include <unordered_map>
#include <unordered_set>
#include <ctime>
#include <algorithm>
#include <iomanip>
using namespace std;

// ============================================================================
// Movie
// ============================================================================
// Represents the static details of a film (title, genre, duration).
// Immutable by design -- movie details don't change once created.
// Separated from Screening to distinguish fixed movie data from dynamic
// screening schedules, improving reusability.
// ============================================================================

class Movie {
    string title;
    string genre;
    int durationInMinutes;

public:
    Movie(string title, string genre, int duration)
        : title(title), genre(genre), durationInMinutes(duration) {}

    string getTitle() const { return title; }
    string getGenre() const { return genre; }
    int getDurationInMinutes() const { return durationInMinutes; }
};

// ============================================================================
// PricingStrategy (Strategy Pattern) and Concrete Strategies
// ============================================================================
// The Strategy Pattern allows each seat to have its own pricing logic
// (Normal, Premium, VIP) without modifying the Seat class.
// This promotes extensibility -- adding a new rate class (e.g., StudentRate)
// only requires a new subclass, no changes to existing code.
//
// Each concrete strategy holds a fixed price value for its tier.
// ============================================================================

class PricingStrategy {
public:
    virtual ~PricingStrategy() = default;
    virtual double getPrice() const = 0;
    virtual string getTierName() const = 0;
};

class NormalRate : public PricingStrategy {
    double price;
public:
    NormalRate(double p) : price(p) {}
    double getPrice() const override { return price; }
    string getTierName() const override { return "NORMAL"; }
};

class PremiumRate : public PricingStrategy {
    double price;
public:
    PremiumRate(double p) : price(p) {}
    double getPrice() const override { return price; }
    string getTierName() const override { return "PREMIUM"; }
};

class VIPRate : public PricingStrategy {
    double price;
public:
    VIPRate(double p) : price(p) {}
    double getPrice() const override { return price; }
    string getTierName() const override { return "VIP"; }
};

// ============================================================================
// Seat
// ============================================================================
// Represents an individual seat in a cinema room.
// Each seat has a unique seat number and a PricingStrategy that determines
// its cost. The strategy can be swapped at runtime (e.g., for promotions).
//
// The Seat class does NOT track booking state -- that's handled by
// ScreeningManager, because the same seat can be available for one
// screening and booked for another.
// ============================================================================

class Seat {
    string seatNumber;
    PricingStrategy* pricingStrategy;  // non-owning, strategy outlives seat usage

public:
    Seat(string num, PricingStrategy* strategy)
        : seatNumber(num), pricingStrategy(strategy) {}

    string getSeatNumber() const { return seatNumber; }
    PricingStrategy* getPricingStrategy() const { return pricingStrategy; }
    void setPricingStrategy(PricingStrategy* s) { pricingStrategy = s; }
};

// ============================================================================
// Layout
// ============================================================================
// Organizes seats into a grid structure (rows x columns) for a cinema room.
// Provides two lookup methods:
//   - By seat number (string key)  -> O(1) via unordered_map
//   - By row/column position       -> O(1) via nested map
//
// Seats are initialized with null pricing by default. Pricing strategies
// should be assigned after layout creation (e.g., front rows = VIP).
//
// Uses nested map instead of 2D array for flexibility with irregular layouts
// (e.g., missing seats, variable row sizes).
// ============================================================================

class Layout {
    int rows;
    int columns;
    unordered_map<string, Seat*> seatsByNumber;
    unordered_map<int, unordered_map<int, Seat*>> seatsByPosition;
    vector<Seat> seats;  // owns all Seat objects

public:
    Layout(int r, int c) : rows(r), columns(c) {
        // Pre-reserve to prevent reallocation (pointers would dangle)
        seats.reserve(r * c);
        for (int i = 0; i < r; i++) {
            for (int j = 0; j < c; j++) {
                string num = to_string(i) + "-" + to_string(j);
                seats.emplace_back(num, nullptr);
                Seat* seatPtr = &seats.back();
                seatsByNumber[num] = seatPtr;
                seatsByPosition[i][j] = seatPtr;
            }
        }
    }

    int getRows() const { return rows; }
    int getColumns() const { return columns; }

    Seat* getSeatByNumber(const string& num) {
        auto it = seatsByNumber.find(num);
        return (it != seatsByNumber.end()) ? it->second : nullptr;
    }

    Seat* getSeatByPosition(int row, int col) {
        auto rowIt = seatsByPosition.find(row);
        if (rowIt == seatsByPosition.end()) return nullptr;
        auto colIt = rowIt->second.find(col);
        return (colIt != rowIt->second.end()) ? colIt->second : nullptr;
    }

    vector<Seat*> getAllSeats() {
        vector<Seat*> result;
        for (auto& s : seats) result.push_back(&s);
        return result;
    }
};

// ============================================================================
// Room
// ============================================================================
// Represents a screening room within a cinema. Each room has a unique
// room number and a Layout that defines its seating arrangement.
// Separated from Layout so rooms can potentially share or customize
// seating configurations.
// ============================================================================

class Room {
    string roomNumber;
    Layout* layout;  // non-owning

public:
    Room(string num, Layout* l) : roomNumber(num), layout(l) {}

    string getRoomNumber() const { return roomNumber; }
    Layout* getLayout() const { return layout; }
};

// ============================================================================
// Cinema
// ============================================================================
// Models a physical cinema location containing multiple rooms.
// Uses composition -- Cinema owns a list of Room pointers.
// This allows each room to operate independently with its own layout
// while the Cinema provides a unified context (name + location).
// ============================================================================

class Cinema {
    string name;
    string location;
    vector<Room*> rooms;

public:
    Cinema(string name, string loc) : name(name), location(loc) {}

    void addRoom(Room* room) { rooms.push_back(room); }

    string getName() const { return name; }
    string getLocation() const { return location; }
    vector<Room*> getRooms() const { return rooms; }
};

// ============================================================================
// Screening
// ============================================================================
// Combines a Movie, a Room, and a time slot to define a specific showing.
// While Movie captures film-level data, Screening adds the context of
// *when* and *where* it's being shown.
//
// Uses time_t for start/end times (simpler than chrono for this use case).
// ============================================================================

class Screening {
    Movie* movie;
    Room* room;
    time_t startTime;
    time_t endTime;

public:
    Screening(Movie* m, Room* r, time_t start, time_t end)
        : movie(m), room(r), startTime(start), endTime(end) {}

    Movie* getMovie() const { return movie; }
    Room* getRoom() const { return room; }
    time_t getStartTime() const { return startTime; }
    time_t getEndTime() const { return endTime; }

    // Duration in minutes
    double getDurationMinutes() const {
        return difftime(endTime, startTime) / 60.0;
    }
};

// ============================================================================
// Ticket
// ============================================================================
// Represents a purchased seat for a specific Screening.
// The price is captured at booking time, so it remains fixed even if the
// seat's PricingStrategy changes later (e.g., a promotion ends).
// ============================================================================

class Ticket {
    Screening* screening;
    Seat* seat;
    double price;

public:
    Ticket(Screening* scr, Seat* s, double p)
        : screening(scr), seat(s), price(p) {}

    Screening* getScreening() const { return screening; }
    Seat* getSeat() const { return seat; }
    double getPrice() const { return price; }
};

// ============================================================================
// Order
// ============================================================================
// Groups multiple Tickets into a single transaction.
// Tracks the order timestamp and provides total cost calculation.
// A user can book multiple seats for the same screening in one order.
// ============================================================================

class Order {
    vector<Ticket*> tickets;
    time_t orderDate;

public:
    Order() : orderDate(time(nullptr)) {}

    void addTicket(Ticket* t) { tickets.push_back(t); }

    double calculateTotalPrice() const {
        double total = 0.0;
        for (auto* t : tickets)
            total += t->getPrice();
        return total;
    }

    vector<Ticket*> getAllTickets() const { return tickets; }
    time_t getOrderDate() const { return orderDate; }
};

// ============================================================================
// ScreeningManager
// ============================================================================
// Central coordinator for screening and ticket operations.
//
// Data structures:
//   - screeningsByMovie[Movie*]     -> list of Screening* for that movie
//   - ticketsByScreening[Screening*] -> list of Ticket* sold for that screening
//
// Key operations:
//   - addScreening: schedule a new showing
//   - addTicket: record a ticket sale
//   - getAvailableSeats: computes remaining seats by subtracting booked
//     seats from the room's full layout
//
// Separated from Cinema so that static cinema attributes (location, rooms)
// aren't coupled with dynamic scheduling/booking logic.
// ============================================================================

class ScreeningManager {
    unordered_map<Movie*, vector<Screening*>> screeningsByMovie;
    unordered_map<Screening*, vector<Ticket*>> ticketsByScreening;

public:
    void addScreening(Movie* movie, Screening* screening) {
        screeningsByMovie[movie].push_back(screening);
    }

    vector<Screening*> getScreeningsForMovie(Movie* movie) {
        auto it = screeningsByMovie.find(movie);
        return (it != screeningsByMovie.end()) ? it->second : vector<Screening*>{};
    }

    void addTicket(Screening* screening, Ticket* ticket) {
        ticketsByScreening[screening].push_back(ticket);
    }

    vector<Ticket*> getTicketsForScreening(Screening* screening) {
        auto it = ticketsByScreening.find(screening);
        return (it != ticketsByScreening.end()) ? it->second : vector<Ticket*>{};
    }

    // Returns seats that haven't been booked yet for this screening
    // Uses unordered_set for O(1) lookup instead of O(n) inner loop
    vector<Seat*> getAvailableSeats(Screening* screening) {
        vector<Seat*> allSeats = screening->getRoom()->getLayout()->getAllSeats();
        vector<Ticket*> booked = getTicketsForScreening(screening);

        // Build a set of booked seat pointers -- O(b) where b = booked count
        unordered_set<Seat*> bookedSeats;
        for (auto* ticket : booked)
            bookedSeats.insert(ticket->getSeat());

        // Filter available -- O(n) where n = total seats, each lookup O(1)
        vector<Seat*> available;
        for (auto* seat : allSeats) {
            if (bookedSeats.find(seat) == bookedSeats.end())
                available.push_back(seat);
        }
        return available;
    }
};

// ============================================================================
// MovieBookingSystem (Facade Pattern)
// ============================================================================
// The single entry point for clients. Delegates internally to:
//   - ScreeningManager for scheduling and booking
//   - Movie/Cinema lists for catalog management
//
// Keeps client code simple -- they don't need to know about
// ScreeningManager or internal data structures.
//
// bookTicket() calculates the price from the seat's PricingStrategy at
// booking time, creating a Ticket with that frozen price.
// ============================================================================

class MovieBookingSystem {
    vector<Movie*> movies;
    vector<Cinema*> cinemas;
    ScreeningManager screeningManager;
    vector<Ticket*> allTickets;  // owns all Ticket objects for cleanup

public:
    ~MovieBookingSystem() {
        for (auto* t : allTickets) delete t;
    }

    void addMovie(Movie* movie) { movies.push_back(movie); }
    void addCinema(Cinema* cinema) { cinemas.push_back(cinema); }

    void addScreening(Movie* movie, Screening* screening) {
        screeningManager.addScreening(movie, screening);
    }

    // Books a ticket for a specific seat at a screening
    Ticket* bookTicket(Screening* screening, Seat* seat) {
        // Check if seat is already booked for this screening
        vector<Seat*> available = screeningManager.getAvailableSeats(screening);
        bool found = false;
        for (auto* s : available) {
            if (s == seat) { found = true; break; }
        }
        if (!found) {
            cout << "[BOOKING FAILED] Seat " << seat->getSeatNumber()
                 << " is already booked!" << endl;
            return nullptr;
        }

        double price = seat->getPricingStrategy()->getPrice();
        Ticket* ticket = new Ticket(screening, seat, price);
        screeningManager.addTicket(screening, ticket);
        allTickets.push_back(ticket);
        return ticket;
    }

    vector<Screening*> getScreeningsForMovie(Movie* movie) {
        return screeningManager.getScreeningsForMovie(movie);
    }

    vector<Seat*> getAvailableSeats(Screening* screening) {
        return screeningManager.getAvailableSeats(screening);
    }

    vector<Ticket*> getTicketsForScreening(Screening* screening) {
        return screeningManager.getTicketsForScreening(screening);
    }

    int getTicketCount(Screening* screening) {
        return screeningManager.getTicketsForScreening(screening).size();
    }
};

// ============================================================================
// Helper: create a time_t from hour/minute (today's date)
// ============================================================================

time_t makeTime(int hour, int minute) {
    time_t now = time(nullptr);
    tm* t = localtime(&now);
    t->tm_hour = hour;
    t->tm_min = minute;
    t->tm_sec = 0;
    return mktime(t);
}

// ============================================================================
// Main -- Demo
// ============================================================================

int main() {
    cout << "========== Movie Ticket Booking System ==========" << endl << endl;

    // ---- 1. Create pricing strategies ----
    NormalRate normalRate(10.0);
    PremiumRate premiumRate(15.0);
    VIPRate vipRate(25.0);

    // ---- 2. Create a layout (4 rows x 5 columns = 20 seats) ----
    Layout layout1(4, 5);

    // Assign pricing: row 0-1 = Normal, row 2 = Premium, row 3 = VIP
    for (int r = 0; r < 4; r++) {
        for (int c = 0; c < 5; c++) {
            Seat* seat = layout1.getSeatByPosition(r, c);
            if (r <= 1)      seat->setPricingStrategy(&normalRate);
            else if (r == 2) seat->setPricingStrategy(&premiumRate);
            else             seat->setPricingStrategy(&vipRate);
        }
    }

    // ---- 3. Create cinema and room ----
    Room room1("R1", &layout1);
    Cinema cinema("CinePlex", "Downtown");
    cinema.addRoom(&room1);

    // ---- 4. Create movies ----
    Movie movie1("Inception", "Sci-Fi", 148);
    Movie movie2("The Dark Knight", "Action", 152);

    // ---- 5. Create screenings ----
    Screening screening1(&movie1, &room1, makeTime(14, 0), makeTime(16, 30));
    Screening screening2(&movie1, &room1, makeTime(19, 0), makeTime(21, 30));
    Screening screening3(&movie2, &room1, makeTime(17, 0), makeTime(19, 30));

    // ---- 6. Assemble the system ----
    MovieBookingSystem system;
    system.addMovie(&movie1);
    system.addMovie(&movie2);
    system.addCinema(&cinema);
    system.addScreening(&movie1, &screening1);
    system.addScreening(&movie1, &screening2);
    system.addScreening(&movie2, &screening3);

    // ---- 7. Show available screenings ----
    cout << "Screenings for \"" << movie1.getTitle() << "\":" << endl;
    for (auto* scr : system.getScreeningsForMovie(&movie1)) {
        time_t st = scr->getStartTime();
        tm* stm = localtime(&st);
        cout << "  Room " << scr->getRoom()->getRoomNumber()
             << " at " << put_time(stm, "%H:%M")
             << " (" << scr->getDurationMinutes() << " min)" << endl;
    }
    cout << endl;

    // ---- 8. Check available seats ----
    cout << "Available seats for screening 1: "
         << system.getAvailableSeats(&screening1).size() << " / 20" << endl;

    // ---- 9. Book some tickets ----
    Seat* seat1 = layout1.getSeatByNumber("0-2");  // Normal
    Seat* seat2 = layout1.getSeatByNumber("2-3");  // Premium
    Seat* seat3 = layout1.getSeatByNumber("3-1");  // VIP

    cout << endl << "--- Booking tickets ---" << endl;
    Ticket* t1 = system.bookTicket(&screening1, seat1);
    Ticket* t2 = system.bookTicket(&screening1, seat2);
    Ticket* t3 = system.bookTicket(&screening1, seat3);

    if (t1) cout << "Booked seat " << t1->getSeat()->getSeatNumber()
                 << " (" << t1->getSeat()->getPricingStrategy()->getTierName()
                 << ") -> $" << t1->getPrice() << endl;
    if (t2) cout << "Booked seat " << t2->getSeat()->getSeatNumber()
                 << " (" << t2->getSeat()->getPricingStrategy()->getTierName()
                 << ") -> $" << t2->getPrice() << endl;
    if (t3) cout << "Booked seat " << t3->getSeat()->getSeatNumber()
                 << " (" << t3->getSeat()->getPricingStrategy()->getTierName()
                 << ") -> $" << t3->getPrice() << endl;

    // ---- 10. Try double-booking ----
    cout << endl << "--- Attempting double-booking ---" << endl;
    system.bookTicket(&screening1, seat1);  // Should fail

    // ---- 11. Create an order ----
    cout << endl << "--- Creating order ---" << endl;
    Order order;
    if (t1) order.addTicket(t1);
    if (t2) order.addTicket(t2);
    if (t3) order.addTicket(t3);
    cout << "Order total: $" << order.calculateTotalPrice()
         << " (" << order.getAllTickets().size() << " tickets)" << endl;

    // ---- 12. Check remaining seats ----
    cout << endl << "Available seats after booking: "
         << system.getAvailableSeats(&screening1).size() << " / 20" << endl;
    cout << "Tickets sold for screening 1: "
         << system.getTicketCount(&screening1) << endl;

    cout << endl << "========== Simulation Complete ==========" << endl;
    return 0;
}
