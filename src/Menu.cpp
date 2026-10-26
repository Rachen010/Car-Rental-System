#include "Menu.h"
#include "CarBST.h"
#include "CustomerList.h"
#include "Utils.h"
#include <iostream>
#include <iomanip>
using namespace std;

// ข้อมูลหลักของระบบ: BST เก็บรถ และ Linked List เก็บลูกค้า/ประวัติการเช่า
// ประกาศเป็น static เพื่อให้ใช้ได้เฉพาะในไฟล์นี้
static CarBST cars;
static CustomerList customers;

// เมนู 1: เพิ่มรถ -> Insert ลง BST
static void addCar() {
    Car c;
    c.plate = toUpper(readLine("Plate: "));
    if (c.plate.empty()) { cout << "  Plate cannot be empty.\n"; return; }
    c.brand = readLine("Brand: ");
    c.model = readLine("Model: ");
    c.pricePerDay = readDouble("Price per day: ");
    c.available = true;
    cout << (cars.insert(c) ? "  Car added.\n" : "  Plate already exists.\n");
}

// เมนู 2: ค้นหารถ -> Search ใน BST ด้วยทะเบียน
static void searchCar() {
    string p = toUpper(readLine("Plate to search: "));
    Car* c = cars.search(p);
    if (!c) { cout << "  Car not found.\n"; return; }
    cout << "  " << c->plate << " | " << c->brand << " " << c->model
         << " | " << c->pricePerDay << "/day | "
         << (c->available ? "Available" : "Rented") << "\n";
}

// เมนู 3: ลบรถ -> Delete จาก BST (ห้ามลบถ้ารถกำลังถูกเช่า)
static void deleteCar() {
    string p = toUpper(readLine("Plate to delete: "));
    Car* c = cars.search(p);
    if (!c) { cout << "  Car not found.\n"; return; }
    if (!c->available) { cout << "  Car is currently rented, cannot delete.\n"; return; }
    cars.remove(p);
    cout << "  Car deleted.\n";
}

// เมนู 5: ลงทะเบียนลูกค้า -> Insert ลง Linked List
static void addCustomer() {
    string id = readLine("Customer ID: ");
    string name = readLine("Customer name: ");
    if (id.empty()) { cout << "  ID cannot be empty.\n"; return; }
    cout << (customers.insertCustomer(id, name) ? "  Customer added.\n"
                                                : "  Customer ID already exists.\n");
}

// เมนู 12: ค้นหาลูกค้า -> Search ใน Linked List ด้วย ID หรือชื่อ (พิมพ์แค่บางส่วนของชื่อก็ได้)
static void searchCustomer() {
    string key = readLine("Customer ID or name to search: ");
    if (key.empty()) { cout << "  Please type something to search.\n"; return; }
    if (customers.searchAndPrint(key) == 0)
        cout << "  Customer not found.\n";
    else
        cout << "  (use menu 6 to see the rental history of a customer)\n";
}

// เมนู 6: ดูประวัติการเช่า -> Search ลูกค้า แล้วแสดง Linked List ประวัติ
static void showCustomerHistory() {
    string id = readLine("Customer ID: ");
    Customer* c = customers.searchCustomer(id);
    if (!c) { cout << "  Customer not found.\n"; return; }
    customers.printHistory(c);
}

// เมนู 7: ลบลูกค้า -> Delete จาก Linked List (ห้ามลบถ้ายังมีรถค้างคืน)
static void deleteCustomer() {
    string id = readLine("Customer ID to delete: ");
    int r = customers.deleteCustomer(id);
    if (r == 0) cout << "  Customer deleted.\n";
    else if (r == 1) cout << "  Customer not found.\n";
    else cout << "  Customer still has an active rental.\n";
}

// เมนู 9: เช่ารถ
// ขั้นตอน: 1) ตรวจว่ามีลูกค้า  2) ค้นรถใน BST  3) ตรวจว่ารถว่าง
//          4) คำนวณราคา  5) เพิ่มประวัติใน Linked List ของลูกค้า  6) ตั้งสถานะรถเป็น Rented
static void rentCar() {
    string id = readLine("Customer ID: ");
    Customer* cu = customers.searchCustomer(id);
    if (!cu) { cout << "  Customer not found (register first).\n"; return; }
    string p = toUpper(readLine("Plate: "));
    Car* car = cars.search(p);
    if (!car) { cout << "  Car not found.\n"; return; }
    if (!car->available) { cout << "  Car is already rented.\n"; return; }
    int days = readInt("Number of days: ");
    if (days <= 0) { cout << "  Days must be > 0.\n"; return; }
    double total = days * car->pricePerDay;
    Rental* r = customers.insertRental(cu, p, days, total);
    car->available = false;
    cout << "  Rented! Rental ID " << r->id << ", total " << fixed
         << setprecision(2) << total << "\n";
}

// เมนู 10: คืนรถ
// หารายการเช่าที่ยังไม่คืนใน Linked List ของลูกค้า แล้วตั้ง returned = true
// พร้อมคืนสถานะรถใน BST ให้เป็น Available
static void returnCar() {
    string id = readLine("Customer ID: ");
    Customer* cu = customers.searchCustomer(id);
    if (!cu) { cout << "  Customer not found.\n"; return; }
    string p = toUpper(readLine("Plate: "));
    Rental* r = customers.findActiveRental(cu, p);
    if (!r) { cout << "  No active rental of this car for this customer.\n"; return; }
    r->returned = true;
    Car* car = cars.search(p);
    if (car) car->available = true;
    cout << "  Car returned. Thank you!\n";
}

// เมนู 11: ลบประวัติการเช่า -> Delete จาก Linked List (ลบได้เฉพาะที่คืนรถแล้ว)
static void deleteRentalRecord() {
    string id = readLine("Customer ID: ");
    Customer* cu = customers.searchCustomer(id);
    if (!cu) { cout << "  Customer not found.\n"; return; }
    int rid = readInt("Rental ID to delete: ");
    int r = customers.deleteRental(cu, rid);
    if (r == 0) cout << "  Record deleted.\n";
    else if (r == 1) cout << "  Record not found.\n";
    else cout << "  Rental is still active, return the car first.\n";
}

static void printMenu() {
    cout << "\n===== CAR RENTAL SYSTEM =====\n"
         << " [Cars - BST]\n"
         << "  1. Add car\n"
         << "  2. Search car\n"
         << "  3. Delete car\n"
         << "  4. Show all cars (sorted by plate)\n"
         << " [Customers - Linked List]\n"
         << "  5. Register customer\n"
         << "  6. Show customer rental history\n"
         << "  7. Delete customer\n"
         << "  8. Show all customers\n"
         << " 12. Search customer (by ID or name)\n"
         << " [Rental]\n"
         << "  9. Rent a car\n"
         << " 10. Return a car\n"
         << " 11. Delete a rental record\n"
         << "  0. Exit\n";
}

// ชื่อไฟล์เก็บข้อมูล (อยู่โฟลเดอร์เดียวกับที่รันโปรแกรม)
static const char* CARS_FILE      = "cars.txt";
static const char* CUSTOMERS_FILE = "customer.txt";

// บันทึกข้อมูลทั้งหมดลงไฟล์ (เรียกหลังทุกการทำงานในเมนู)
static void saveData() {
    if (!cars.saveToFile(CARS_FILE))
        cout << "  [Warning] Cannot write " << CARS_FILE << "\n";
    if (!customers.saveToFile(CUSTOMERS_FILE))
        cout << "  [Warning] Cannot write " << CUSTOMERS_FILE << "\n";
}

// ข้อมูลตัวอย่าง ใช้ครั้งแรกที่ยังไม่มีไฟล์ (ทะเบียนที่ใส่แบบไม่เรียงกัน เพื่อให้เห็นว่า BST จัดเรียงให้)
static void loadSampleData() {
    cars.insert({"1AB1234", "Toyota", "Vios",    800,  true});
    cars.insert({"2CD5678", "Honda",  "City",    900,  true});
    cars.insert({"3EF9012", "Mazda",  "CX-5",    1500, true});
    cars.insert({"1AA0001", "Isuzu",  "D-Max",   1200, true});
    cars.insert({"4GH3456", "Nissan", "Almera",  750,  true});
    customers.insertCustomer("C001", "Somchai");
    customers.insertCustomer("C002", "Suda");
}

// โหลดข้อมูลจาก cars.txt และ customer.txt
// ถ้าไม่มีทั้งสองไฟล์ (เปิดโปรแกรมครั้งแรก) ให้ใช้ข้อมูลตัวอย่างแล้วสร้างไฟล์ให้
void loadData() {
    bool hasCars = cars.loadFromFile(CARS_FILE);
    bool hasCust = customers.loadFromFile(CUSTOMERS_FILE);
    if (!hasCars && !hasCust) {
        loadSampleData();
        saveData();
        cout << "No data files found - created " << CARS_FILE << " and "
             << CUSTOMERS_FILE << " with sample data.\n";
    } else {
        cout << "Data loaded from " << CARS_FILE << " and " << CUSTOMERS_FILE << ".\n";
    }
}

// วนลูปแสดงเมนู -> รับตัวเลือก -> เรียกฟังก์ชันที่เกี่ยวข้อง จนกว่าจะเลือก 0
void runMenu() {
    int choice;
    do {
        printMenu();
        choice = readInt("Choose: ");
        cout << "\n";
        switch (choice) {
            case 1:  addCar(); break;
            case 2:  searchCar(); break;
            case 3:  deleteCar(); break;
            case 4:  cars.printAll(); break;
            case 5:  addCustomer(); break;
            case 6:  showCustomerHistory(); break;
            case 7:  deleteCustomer(); break;
            case 8:  customers.printAllCustomers(); break;
            case 9:  rentCar(); break;
            case 10: returnCar(); break;
            case 11: deleteRentalRecord(); break;
            case 12: searchCustomer(); break;
            case 0:  cout << "Goodbye!\n"; break;
            default: cout << "  Invalid choice.\n";
        }
        saveData();     // บันทึกลงไฟล์ทุกครั้งหลังทำรายการ ข้อมูลจึงไม่หายแม้ปิดโปรแกรมกลางคัน
    } while (choice != 0);
}
