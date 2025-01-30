#include <iostream>
#include <windows.h>
#include <fstream>
#include <iomanip>
#include <limits>
using namespace std;

// Global constants for file names
const string ITEM_FILE = "itemstore.dat";
const string TEMP_FILE = "temp.dat";

// Function to move the cursor to a specific position
void gotoxy(int x, int y) {
    COORD coord;
    coord.X = x;
    coord.Y = y;
    SetConsoleCursorPosition(GetStdHandle(STD_OUTPUT_HANDLE), coord);
}

// Structure to store date
struct Date {
    int day, month, year;
};

// Base class for items
class Item {
protected:
    int itemNo;
    char name[25];
    Date manufacturingDate;

public:
    void addItem() {
        cout << "\n\n\tItem No: ";
        cin >> itemNo;
        cin.ignore(); // Clear input buffer
        cout << "\n\n\tName of the item: ";
        cin.getline(name, 25);
        cout << "\n\n\tManufacturing Date (dd-mm-yyyy): ";
        cin >> manufacturingDate.day >> manufacturingDate.month >> manufacturingDate.year;
    }

    void showItem() const {
        cout << "\n\tItem No: " << itemNo;
        cout << "\n\n\tName of the item: " << name;
        cout << "\n\n\tManufacturing Date: " << manufacturingDate.day << "-"
             << manufacturingDate.month << "-" << manufacturingDate.year;
    }

    void reportItem() const {
        gotoxy(3, 7);
        cout << itemNo;
        gotoxy(13, 7);
        cout << name;
    }

    int getItemNo() const {
        return itemNo;
    }
};

// Derived class for amount calculations
class Amount : public Item {
private:
    float price, quantity, tax, discount, netAmount;

public:
    void addItemDetails() {
        addItem();
        cout << "\n\n\tPrice (KSh): ";
        cin >> price;
        cout << "\n\n\tQuantity: ";
        cin >> quantity;
        cout << "\n\n\tTax percent: ";
        cin >> tax;
        cout << "\n\n\tDiscount percent: ";
        cin >> discount;
        calculateNetAmount();
    }

    void calculateNetAmount() {
        float grossAmount = price + (price * (tax / 100));
        netAmount = quantity * (grossAmount - (grossAmount * (discount / 100)));
    }

    void showItemDetails() const {
        showItem();
        cout << "\n\n\tPrice: KSh " << price;
        cout << "\n\n\tQuantity: " << quantity;
        cout << "\n\n\tTax: " << tax << "%";
        cout << "\n\n\tDiscount: " << discount << "%";
        cout << "\n\n\tNet Amount: KSh " << netAmount;
    }

    void reportItemDetails() const {
        reportItem();
        gotoxy(23, 7);
        cout << price;
        gotoxy(33, 7);
        cout << quantity;
        gotoxy(44, 7);
        cout << tax;
        gotoxy(52, 7);
        cout << discount;
        gotoxy(64, 7);
        cout << netAmount;
    }

    float getNetAmount() const {
        return netAmount;
    }
};

// Function to display the main menu
void showMainMenu() {
    system("cls");
    gotoxy(25, 2);
    cout << "Super Market Billing System";
    gotoxy(25, 3);
    cout << "===========================";
    gotoxy(25, 5);
    cout << "1. Bill Report";
    gotoxy(25, 6);
    cout << "2. Add/Edit/Delete Item";
    gotoxy(25, 7);
    cout << "3. Show Item Details";
    gotoxy(25, 8);
    cout << "4. Exit";
    gotoxy(25, 10);
    cout << "Enter your choice: ";
}

// Function to add an item
void addItem() {
    Amount item;
    ofstream fout(ITEM_FILE, ios::binary | ios::app);
    if (!fout) {
        cerr << "Error: Unable to open file.\n";
        return;
    }
    item.addItemDetails();
    fout.write((char*)&item, sizeof(item));
    fout.close();
    cout << "\n\t\tItem added successfully!";
    getch();
}

// Function to display all items
void showAllItems() {
    Amount item;
    ifstream fin(ITEM_FILE, ios::binary);
    if (!fin) {
        cerr << "Error: Unable to open file.\n";
        return;
    }
    float grandTotal = 0;
    system("cls");
    gotoxy(30, 3);
    cout << "BILL DETAILS";
    gotoxy(3, 5);
    cout << "ITEM NO";
    gotoxy(13, 5);
    cout << "NAME";
    gotoxy(23, 5);
    cout << "PRICE";
    gotoxy(33, 5);
    cout << "QUANTITY";
    gotoxy(44, 5);
    cout << "TAX %";
    gotoxy(52, 5);
    cout << "DISCOUNT %";
    gotoxy(64, 5);
    cout << "NET AMOUNT";
    while (fin.read((char*)&item, sizeof(item))) {
        item.reportItemDetails();
        grandTotal += item.getNetAmount();
    }
    gotoxy(17, 20);
    cout << "\n\n\t\tGrand Total: KSh " << grandTotal;
    fin.close();
    getch();
}

// Function to edit an item
void editItem() {
    int itemNo;
    cout << "\n\n\tEnter Item Number to edit: ";
    cin >> itemNo;
    Amount item;
    fstream file(ITEM_FILE, ios::binary | ios::in | ios::out);
    if (!file) {
        cerr << "Error: Unable to open file.\n";
        return;
    }
    bool found = false;
    while (file.read((char*)&item, sizeof(item))) {
        if (item.getItemNo() == itemNo) {
            found = true;
            item.showItemDetails();
            cout << "\n\n\tEnter new details:\n";
            item.addItemDetails();
            file.seekp(-static_cast<int>(sizeof(item)), ios::cur);
            file.write((char*)&item, sizeof(item));
            cout << "\n\t\tItem updated successfully!";
            break;
        }
    }
    if (!found) {
        cout << "\n\t\tItem not found!";
    }
    file.close();
    getch();
}

// Function to delete an item
void deleteItem() {
    int itemNo;
    cout << "\n\n\tEnter Item Number to delete: ";
    cin >> itemNo;
    Amount item;
    ifstream fin(ITEM_FILE, ios::binary);
    ofstream fout(TEMP_FILE, ios::binary);
    if (!fin || !fout) {
        cerr << "Error: Unable to open file.\n";
        return;
    }
    bool found = false;
    while (fin.read((char*)&item, sizeof(item))) {
        if (item.getItemNo() != itemNo) {
            fout.write((char*)&item, sizeof(item));
        } else {
            found = true;
        }
    }
    fin.close();
    fout.close();
    remove(ITEM_FILE.c_str());
    rename(TEMP_FILE.c_str(), ITEM_FILE.c_str());
    if (found) {
        cout << "\n\t\tItem deleted successfully!";
    } else {
        cout << "\n\t\tItem not found!";
    }
    getch();
}

// Function to show item details
void showItemDetails() {
    int itemNo;
    cout << "\n\n\tEnter Item Number: ";
    cin >> itemNo;
    Amount item;
    ifstream fin(ITEM_FILE, ios::binary);
    if (!fin) {
        cerr << "Error: Unable to open file.\n";
        return;
    }
    bool found = false;
    while (fin.read((char*)&item, sizeof(item))) {
        if (item.getItemNo() == itemNo) {
            item.showItemDetails();
            found = true;
            break;
        }
    }
    if (!found) {
        cout << "\n\t\tItem not found!";
    }
    fin.close();
    getch();
}

// Main function
int main() {
    int choice;
    while (true) {
        showMainMenu();
        cin >> choice;
        switch (choice) {
            case 1: showAllItems(); break;
            case 2: {
                system("cls");
                gotoxy(25, 2);
                cout << "1. Add Item";
                gotoxy(25, 3);
                cout << "2. Edit Item";
                gotoxy(25, 4);
                cout << "3. Delete Item";
                gotoxy(25, 5);
                cout << "4. Back to Main Menu";
                gotoxy(25, 7);
                cout << "Enter your choice: ";
                int subChoice;
                cin >> subChoice;
                switch (subChoice) {
                    case 1: addItem(); break;
                    case 2: editItem(); break;
                    case 3: deleteItem(); break;
                    case 4: break;
                    default: cout << "\n\t\tInvalid choice!";
                }
                break;
            }
            case 3: showItemDetails(); break;
            case 4: exit(0);
            default: cout << "\n\t\tInvalid choice!";
        }
    }
    return 0;
}