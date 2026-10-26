#pragma once
#include <string>
#include "Customer.h"

// Linked list of customers; each customer owns a linked list of rentals
class CustomerList {
    Customer* head;
    int rentalCounter;

    void freeHistory(Rental* r);

public:
    CustomerList();                                             // Create
    ~CustomerList();

    // ---- customer operations ----
    bool      insertCustomer(const std::string& id, const std::string& name);  // Insert
    Customer* searchCustomer(const std::string& id);                           // Search
    int       deleteCustomer(const std::string& id);                           // Delete (0 ok, 1 not found, 2 active rental)

    // ---- rental history operations ----
    Rental* insertRental(Customer* c, const std::string& plate, int days, double total);
    Rental* searchRental(Customer* c, int rentalId);
    Rental* findActiveRental(Customer* c, const std::string& plate);
    int     deleteRental(Customer* c, int rentalId);               // 0 ok, 1 not found, 2 still active

    // ค้นหาลูกค้าด้วย ID หรือชื่อ (บางส่วนของชื่อก็ได้ ไม่สนตัวพิมพ์เล็ก/ใหญ่) แล้วแสดงผล
    // คืนจำนวนลูกค้าที่พบ
    int searchAndPrint(const std::string& keyword) const;

    void printHistory(Customer* c) const;
    void printAllCustomers() const;

    // ---- file ----
    bool saveToFile(const std::string& filename) const;   // เขียนลูกค้า + ประวัติการเช่า
    bool loadFromFile(const std::string& filename);       // อ่านจากไฟล์ (false = ไม่มีไฟล์)
};
