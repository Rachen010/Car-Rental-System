#include "CustomerList.h"
#include "Utils.h"
#include <iostream>
#include <iomanip>
#include <fstream>
#include <sstream>
#include <vector>
using namespace std;

// Create: เริ่มต้นเป็น list ว่าง
CustomerList::CustomerList() : head(nullptr), rentalCounter(0) {}

// ทำลาย list: เดินลบลูกค้าทีละคน พร้อมลบประวัติการเช่าของคนนั้นด้วย
CustomerList::~CustomerList() {
    while (head) {
        Customer* t = head;
        head = head->next;
        freeHistory(t->history);
        delete t;
    }
}

void CustomerList::freeHistory(Rental* r) {
    while (r) { Rental* t = r; r = r->next; delete t; }
}

// ================= ส่วนของลูกค้า =================

// Insert: เพิ่มลูกค้าต่อท้าย list (ไม่ให้ ID ซ้ำ)
bool CustomerList::insertCustomer(const string& id, const string& name) {
    if (searchCustomer(id)) return false;           // ID ซ้ำ
    Customer* c = new Customer{id, name, nullptr, nullptr};
    if (!head) head = c;                            // list ว่าง -> เป็นตัวแรก
    else {
        Customer* cur = head;
        while (cur->next) cur = cur->next;          // เดินไปท้าย list
        cur->next = c;
    }
    return true;
}

// Search: เดินไล่ทีละ node จนเจอ ID ที่ต้องการ
Customer* CustomerList::searchCustomer(const string& id) {
    for (Customer* c = head; c; c = c->next)
        if (c->id == id) return c;
    return nullptr;
}

// Delete: ใช้ prev ตามหลัง cur เพื่อเชื่อม node ก่อนหน้ากับ node ถัดไป
// ห้ามลบถ้าลูกค้ายังมีรถที่เช่าอยู่และยังไม่คืน
int CustomerList::deleteCustomer(const string& id) {
    Customer *prev = nullptr, *cur = head;
    while (cur && cur->id != id) { prev = cur; cur = cur->next; }
    if (!cur) return 1;                             // ไม่พบลูกค้า
    for (Rental* r = cur->history; r; r = r->next)
        if (!r->returned) return 2;                 // ยังมีรถค้างคืน
    if (prev) prev->next = cur->next; else head = cur->next;  // ตัด node ออกจาก list
    freeHistory(cur->history);                      // ลบประวัติการเช่าของลูกค้าคนนี้ด้วย
    delete cur;
    return 0;
}

// Search แบบละเอียด: เดินไล่ทุก node ใน list เก็บทุกคนที่ตรงเงื่อนไข
//   - ID ตรงกันพอดี  หรือ  - ชื่อมีคำที่ค้นอยู่ข้างใน (เช่นพิมพ์ "som" เจอ "Somchai")
// แปลงเป็นตัวพิมพ์ใหญ่ทั้งสองฝั่งก่อนเทียบ เพื่อไม่สนตัวพิมพ์เล็ก/ใหญ่
int CustomerList::searchAndPrint(const string& keyword) const {
    string key = toUpper(keyword);
    int found = 0;
    for (Customer* c = head; c; c = c->next) {
        bool match = (toUpper(c->id) == key) ||
                     (!key.empty() && toUpper(c->name).find(key) != string::npos);
        if (!match) continue;

        if (found == 0) {                           // พิมพ์หัวตารางครั้งแรกที่เจอ
            cout << left << setw(10) << "ID" << setw(20) << "Name"
                 << setw(10) << "Rentals" << "Active\n" << string(48, '-') << "\n";
        }
        int total = 0, active = 0;                  // นับประวัติการเช่าของลูกค้าคนนี้
        for (Rental* r = c->history; r; r = r->next) {
            ++total;
            if (!r->returned) ++active;
        }
        cout << left << setw(10) << c->id << setw(20) << c->name
             << setw(10) << total << active << "\n";
        ++found;
    }
    return found;
}

// ================= ส่วนของประวัติการเช่า =================

// Insert: เพิ่มประวัติการเช่าต่อท้าย list ของลูกค้าคนนั้น (สถานะเริ่มต้น = ยังไม่คืนรถ)
Rental* CustomerList::insertRental(Customer* c, const string& plate, int days, double total) {
    Rental* r = new Rental{++rentalCounter, plate, days, total, false, nullptr};
    if (!c->history) c->history = r;
    else {
        Rental* cur = c->history;
        while (cur->next) cur = cur->next;
        cur->next = r;
    }
    return r;
}

// Search: ค้นประวัติการเช่าด้วยรหัสการเช่า
Rental* CustomerList::searchRental(Customer* c, int rentalId) {
    for (Rental* r = c->history; r; r = r->next)
        if (r->id == rentalId) return r;
    return nullptr;
}

// หารายการเช่าของรถคันนี้ที่ "ยังไม่คืน" ใช้ตอนคืนรถ
Rental* CustomerList::findActiveRental(Customer* c, const string& plate) {
    for (Rental* r = c->history; r; r = r->next)
        if (r->plate == plate && !r->returned) return r;
    return nullptr;
}

// Delete: ลบได้เฉพาะรายการที่คืนรถแล้ว
int CustomerList::deleteRental(Customer* c, int rentalId) {
    Rental *prev = nullptr, *cur = c->history;
    while (cur && cur->id != rentalId) { prev = cur; cur = cur->next; }
    if (!cur) return 1;                             // ไม่พบรายการ
    if (!cur->returned) return 2;                   // ยังไม่คืนรถ ลบไม่ได้
    if (prev) prev->next = cur->next; else c->history = cur->next;
    delete cur;
    return 0;
}

// ================= แสดงผล =================
void CustomerList::printHistory(Customer* c) const {
    cout << "Customer: " << c->name << " (ID " << c->id << ")\n";
    if (!c->history) { cout << "  (no rental history)\n"; return; }
    cout << left << setw(8) << "RentID" << setw(12) << "Plate"
         << setw(6) << "Days" << setw(12) << "Total" << "Status\n";
    cout << string(48, '-') << "\n";
    for (Rental* r = c->history; r; r = r->next)
        cout << left << setw(8) << r->id << setw(12) << r->plate
             << setw(6) << r->days << setw(12) << fixed << setprecision(2)
             << r->total << (r->returned ? "Returned" : "Active") << "\n";
}

void CustomerList::printAllCustomers() const {
    if (!head) { cout << "  (no customers)\n"; return; }
    cout << left << setw(12) << "ID" << "Name\n" << string(30, '-') << "\n";
    for (Customer* c = head; c; c = c->next)
        cout << left << setw(12) << c->id << c->name << "\n";
}

// ================= บันทึก / โหลดไฟล์ =================
// รูปแบบ customer.txt (แยกช่องด้วย TAB):
//   C <TAB> customerId <TAB> name
//   R <TAB> rentalId <TAB> plate <TAB> days <TAB> total <TAB> returned(1/0)
// บรรทัด R ใต้บรรทัด C ใดๆ คือประวัติการเช่าของลูกค้าคนนั้น
bool CustomerList::saveToFile(const string& filename) const {
    ofstream out(filename.c_str());
    if (!out) return false;
    for (Customer* c = head; c; c = c->next) {
        out << "C\t" << c->id << '\t' << c->name << '\n';
        for (Rental* r = c->history; r; r = r->next)
            out << "R\t" << r->id << '\t' << r->plate << '\t' << r->days << '\t'
                << fixed << setprecision(2) << r->total << '\t'
                << (r->returned ? 1 : 0) << '\n';
    }
    return true;
}

bool CustomerList::loadFromFile(const string& filename) {
    ifstream in(filename.c_str());
    if (!in) return false;
    string line;
    Customer* cur = nullptr;                        // ลูกค้าคนล่าสุดที่อ่านมา
    while (getline(in, line)) {
        if (!line.empty() && line[line.size() - 1] == '\r') line.erase(line.size() - 1);
        if (line.empty()) continue;
        vector<string> f;
        stringstream ss(line);
        string part;
        while (getline(ss, part, '\t')) f.push_back(part);
        try {
            if (f[0] == "C" && f.size() >= 3) {
                insertCustomer(f[1], f[2]);
                cur = searchCustomer(f[1]);
            } else if (f[0] == "R" && f.size() >= 6 && cur) {
                int rid = stoi(f[1]);
                Rental* r = insertRental(cur, f[2], stoi(f[3]), stod(f[4]));
                --rentalCounter;                // insertRental นับเพิ่มให้แล้ว คืนค่ากลับ
                r->id = rid;
                r->returned = (f[5] == "1");
                if (rid > rentalCounter) rentalCounter = rid;   // ให้รหัสการเช่าใหม่ไม่ซ้ำของเดิม
            }
        } catch (...) { /* ข้ามบรรทัดเสีย */ }
    }
    return true;
}
