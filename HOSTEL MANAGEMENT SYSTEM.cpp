#include <iostream>    // for cin, cout
#include <fstream>     // for file handling
#include <string>      // for std::string, getline()
#include <cstdlib>     // for system()
#include <stdexcept>   // for exception handling (optional)
using namespace std;

/* ================= ABSTRACT CLASS ================= */
class Person {
public:
    virtual void display() = 0;  // Pure virtual function
};

/* ================= INTERFACE ================= */
class MessInterface {
public:
    virtual void messDetails() = 0;
};

/* ================= ROOM CLASS ================= */
class Room {
    int roomNo;
    int seater;
    int occupied;

public:
    Room(int r = 0, int s = 1) {
        roomNo = r;
        seater = s;
        occupied = 0;
    }

    bool hasSpace() {
        return occupied < seater;
    }

    void addStudent() {
        if (!hasSpace())
            throw "RoomFull";
        occupied++;
    }

    int getRoomNo() const { return roomNo; }
    int getSeater() const { return seater; }
    int getOccupied() const { return occupied; }
};

/* ================= STUDENT CLASS ================= */
class Student : public Person {
    string name;
    int rollNo;
    Room* room;
    bool messOn;

public:
    static int rollGenerator;

    Student() {
        rollNo = ++rollGenerator;
        room = NULL;
        messOn = false;
    }

    int getRoll() const { return rollNo; }

    void setStudent(string n, Room* r, bool mess) {
        name = n;
        room = r;
        messOn = mess;
    }

    int calculateFee() const {
        int fee = 0;
        if (room->getSeater() == 1) fee = 10000;
        else if (room->getSeater() == 2) fee = 7000;
        else fee = 5000;

        if (messOn) fee += 3000;
        return fee;
    }

    void saveVoucher() {
        string fileName = "voucher_" + to_string(rollNo) + ".txt";
        ofstream file(fileName);
        file << "Roll No: " << rollNo << endl;
        file << "Name: " << name << endl;
        file << "Room No: " << room->getRoomNo() << endl;
        file << "Seater: " << room->getSeater() << endl;
        file << "Mess: " << (messOn ? "ON" : "OFF") << endl;
        file << "Total Fee: " << calculateFee() << endl;
        file << "-------------------------" << endl;
        file.close();
    }

    void display() {
        cout << "\nName: " << name;
        cout << "\nRoll No: " << rollNo;
        cout << "\nRoom No: " << room->getRoomNo();
        cout << "\nSeater: " << room->getSeater();
        cout << "\nMess: " << (messOn ? "ON" : "OFF");
        cout << "\nTotal Fee: " << calculateFee();
        cout << "\n---------------------------\n";
    }
};

int Student::rollGenerator = 1000;

/* ================= MESS MODULE ================= */
class MessModule : public MessInterface {
public:
    void messDetails() {
        ifstream file("mess.txt");
        string name;

        cout << "\n--- Mess Students ---\n";
        while (getline(file, name)) {
            cout << name << endl;
        }
        file.close();
    }
};

/* ================= HOSTEL CLASS ================= */
class Hostel {
    Room rooms[10];
    int roomCount;
    MessModule mess;

public:
    Hostel() { roomCount = 0; }

    void addRoom() {
        int r, s;
        cout << "Enter Room No: ";
        cin >> r;
        cout << "Enter Seater (1/2/3): ";
        cin >> s;

        try {
            if (s < 1 || s > 3) throw 1;

            for (int i = 0; i < roomCount; i++) {
                if (rooms[i].getRoomNo() == r) throw 2;
            }

            rooms[roomCount++] = Room(r, s);
            cout << "Room added successfully!\n";
        }
        catch (int err) {
            if (err == 1)
                cout << "Invalid seater! Only 1, 2 or 3 allowed.\n";
            else if (err == 2)
                cout << "Room already exists! Choose another room number.\n";
        }
    }

    void showRooms() {
        cout << "\nAvailable Rooms:\n";
        for (int i = 0; i < roomCount; i++) {
            cout << i + 1 << ". Room "
                 << rooms[i].getRoomNo()
                 << " (" << rooms[i].getOccupied()
                 << "/" << rooms[i].getSeater() << ")\n";
        }
    }

    void addStudent() {
        if (roomCount == 0) {
            cout << "No rooms available!\n";
            return;
        }

        Student* s = new Student();
        string name;
        int choice, messChoice;

        cout << "Enter Student Name: ";
        cin.ignore();
        getline(cin, name);

        showRooms();
        cout << "Choose Room (1 to " << roomCount << "): ";
        cin >> choice;

        try {
            if (choice < 1 || choice > roomCount)
                throw string("InvalidRoom");

            Room* selectedRoom = &rooms[choice - 1];
            selectedRoom->addStudent();

            cout << "Mess required? (1=Yes 0=No): ";
            cin >> messChoice;

            s->setStudent(name, selectedRoom, messChoice);

            ofstream st("students.txt", ios::app);
            st << s->getRoll() << " " << name << endl;
            st.close();

            if (messChoice) {
                ofstream ms("mess.txt", ios::app);
                ms << name << endl;
                ms.close();
            }

            s->saveVoucher();
            s->display();
        }
        catch (string) {
            cout << "Invalid room selection!\n";
        }
        catch (const char*) {
            cout << "No space in this room, please choose another room.\n";
        }

        delete s;
    }

    void viewVoucher() {
        int roll;
        cout << "Enter Roll Number: ";
        cin >> roll;

        string fileName = "voucher_" + to_string(roll) + ".txt";
        ifstream fin(fileName);

        if (!fin) {
            cout << "Voucher not found for this roll number!\n";
            return;
        }

        string line;
        cout << "\n--- Fee Voucher ---\n";
        while (getline(fin, line)) {
            cout << line << endl;
        }
        fin.close();

        string command = "notepad " + fileName;
        system(command.c_str());
    }

    void viewMess() {
        mess.messDetails();
    }
};

/* ================= ADMIN LOGIN ================= */
bool adminLogin() {
    string u, p;
    int attempts = 0;

    while (attempts < 3) {
        cout << "Username: ";
        cin >> u;
        cout << "Password: ";
        cin >> p;

        if (u == "admin" && p == "1234") {
            cout << "Login successful!\n";
            return true;
        }

        cout << "Wrong credentials!\n";
        attempts++;
    }

    cout << "Too many attempts. Program terminated.\n";
    return false;
}

/* ================= MAIN ================= */
int main() {
    Hostel hostel;
    int choice;

    cout << "========================================\n";
    cout << " WELCOME TO UOL HOSTEL MANAGEMENT SYSTEM\n";
    cout << "========================================\n\n";

    cout << "PLEASE ENTER ADMIN LOGIN CREDENTIALS\n\n";

    if (!adminLogin())
        return 0;

    do {
        cout << "\n1. Add Room";
        cout << "\n2. Add Student";
        cout << "\n3. View Mess Students";
        cout << "\n4. View Fee Voucher";
        cout << "\n0. Exit";
        cout << "\nEnter choice: ";
        cin >> choice;

        switch (choice) {
        case 1: hostel.addRoom(); break;
        case 2: hostel.addStudent(); break;
        case 3: hostel.viewMess(); break;
        case 4: hostel.viewVoucher(); break;
        }
    } while (choice != 0);

    return 0;
}