
#include <iostream>
#include <vector>
#include <string>
#include <memory>
#include <fstream>
#include <algorithm>
#include <limits>
using namespace std;

// Custom Exception
class BookingException : public exception
{
public:
    const char* what() const noexcept override
    {
        return "Booking Error!";
    }
};

// Base Class
class Room
{
protected:
    int roomNumber;
    bool available;

public:
    Room(int n)
    {
        roomNumber = n;
        available = true;
    }

    virtual double getPrice() = 0;

    virtual string getType() = 0;

    int getNumber()
    {
        return roomNumber;
    }

    bool isAvailable()
    {
        return available;
    }

    void setAvailable(bool status)
    {
        available = status;
    }

    virtual void display()
    {
        cout << roomNumber << "  "
             << getType() << "  Rs."
             << getPrice() << "  "
             << (available ? "Available" : "Booked")
             << endl;
    }

    virtual ~Room() {}
};

// Derived Class
class StandardRoom : public Room
{
public:
    StandardRoom(int n) : Room(n) {}

    double getPrice() override
    {
        return 1500;
    }

    string getType() override
    {
        return "Standard";
    }
};

// Derived Class
class DeluxeRoom : public Room
{
public:
    DeluxeRoom(int n) : Room(n) {}

    double getPrice() override
    {
        return 2500;
    }

    string getType() override
    {
        return "Deluxe";
    }
};

// Booking Class
class Booking
{
public:
    int bookingID;
    string customerName;
    int roomNumber;
    int nights;
    double totalAmount;
    bool active;

    Booking(int id, string name, int room,
            int n, double amount)
    {
        bookingID = id;
        customerName = name;
        roomNumber = room;
        nights = n;
        totalAmount = amount;
        active = true;
    }

    void display()
    {
        cout << "\nBooking ID: " << bookingID;
        cout << "\nCustomer Name: " << customerName;
        cout << "\nRoom Number: " << roomNumber;
        cout << "\nNumber of Nights: " << nights;
        cout << "\nTotal Amount: Rs." << totalAmount;
        cout << "\nStatus: "
             << (active ? "Active" : "Cancelled");
        cout << "\n--------------------------\n";
    }
};

// Hotel Class
class Hotel
{
private:
    vector<unique_ptr<Room>> rooms;
    vector<Booking> bookings;
    int nextID = 101;

public:

    // Constructor
    Hotel()
    {
        rooms.push_back(make_unique<StandardRoom>(101));
        rooms.push_back(make_unique<StandardRoom>(102));
        rooms.push_back(make_unique<DeluxeRoom>(201));
        rooms.push_back(make_unique<DeluxeRoom>(202));
    }

    // Display Rooms
    void showRooms()
    {
        cout << "\nROOM DETAILS\n";
        cout << "Room  Type  Price  Status\n";

        for (auto &r : rooms)
        {
            r->display();
        }
    }

    // Book Room
    void bookRoom()
    {
        string name;
        int roomNo, nights;

        cout << "\nEnter Customer Name: ";
        getline(cin, name);

        if (name.empty())
            throw BookingException();

        cout << "Enter Room Number: ";
        cin >> roomNo;

        cout << "Enter Number of Nights: ";
        cin >> nights;

        cin.ignore(numeric_limits<streamsize>::max(), '\n');

        if (nights <= 0)
            throw BookingException();

        auto it = find_if(rooms.begin(), rooms.end(),
            [roomNo](const unique_ptr<Room>& r)
            {
                return r->getNumber() == roomNo;
            });

        if (it == rooms.end() || !(*it)->isAvailable())
            throw BookingException();

        double total = (*it)->getPrice() * nights;

        (*it)->setAvailable(false);

        bookings.emplace_back(
            nextID, name, roomNo, nights, total
        );

        // Save booking in CSV file
        ofstream file("bookings.csv", ios::app);

        if (!file)
        {
            (*it)->setAvailable(true);
            bookings.pop_back();
            throw BookingException();
        }

        if (file.tellp() == 0)
        {
            file << "ID,Name,Room,Nights,Amount,Status\n";
        }

        file << nextID << ","
             << name << ","
             << roomNo << ","
             << nights << ","
             << total << ",Active\n";

        file.close();

        cout << "\nRoom Booked Successfully!";
        cout << "\nBooking ID: " << nextID;
        cout << "\nTotal Amount: Rs." << total << endl;

        nextID++;
    }

    // View Bookings
    void showBookings()
    {
        if (bookings.empty())
        {
            cout << "\nNo bookings found.\n";
            return;
        }

        for (auto &b : bookings)
        {
            b.display();
        }
    }

    // Cancel Booking
    void cancelBooking()
    {
        int id;

        cout << "\nEnter Booking ID: ";
        cin >> id;

        auto it = find_if(bookings.begin(), bookings.end(),
            [id](const Booking& b)
            {
                return b.bookingID == id && b.active;
            });

        if (it == bookings.end())
            throw BookingException();

        for (auto &r : rooms)
        {
            if (r->getNumber() == it->roomNumber)
            {
                r->setAvailable(true);
                break;
            }
        }

        it->active = false;

        cout << "\nBooking Cancelled Successfully!\n";
    }

    // Read CSV File
    void bookingHistory()
    {
        ifstream file("bookings.csv");

        if (!file)
        {
            cout << "\nNo booking history found.\n";
            return;
        }

        string line;

        cout << "\nBOOKING HISTORY\n";

        while (getline(file, line))
        {
            cout << line << endl;
        }

        file.close();
    }
};

// Main Function
int main()
{
    Hotel hotel;
    int choice;

    do
    {
        cout << "\n===== HOTEL ROOM BOOKING SYSTEM =====\n";
        cout << "1. View Rooms\n";
        cout << "2. Book Room\n";
        cout << "3. View Bookings\n";
        cout << "4. Cancel Booking\n";
        cout << "5. Booking History\n";
        cout << "0. Exit\n";

        cout << "\nEnter Your Choice: ";
        cin >> choice;

        cin.ignore(numeric_limits<streamsize>::max(), '\n');

        try
        {
            switch (choice)
            {
                case 1:
                    hotel.showRooms();
                    break;

                case 2:
                    hotel.bookRoom();
                    break;

                case 3:
                    hotel.showBookings();
                    break;

                case 4:
                    hotel.cancelBooking();
                    break;

                case 5:
                    hotel.bookingHistory();
                    break;

                case 0:
                    cout << "\nThank You!\n";
                    break;

                default:
                    cout << "\nInvalid Choice!\n";
            }
        }
        catch (const exception& e)
        {
            cout << "\nError: " << e.what() << endl;
        }

    } while (choice != 0);

    return 0;
}
