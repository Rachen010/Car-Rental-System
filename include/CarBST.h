#pragma once
#include <string>
#include <iosfwd>
#include "Car.h"

// Binary Search Tree of cars, key = license plate
struct CarNode {
    Car data;
    CarNode *left, *right;
    CarNode(const Car& c) : data(c), left(nullptr), right(nullptr) {}
};

class CarBST {
    CarNode* root;

    CarNode* insertNode(CarNode* n, const Car& c, bool& ok);
    CarNode* removeNode(CarNode* n, const std::string& plate, bool& ok);
    void     inorder(CarNode* n) const;
    void     destroy(CarNode* n);
    void     saveNode(CarNode* n, std::ostream& out) const;

public:
    CarBST();                                   // Create
    ~CarBST();
    bool insert(const Car& c);                  // Insert
    Car* search(const std::string& plate);      // Search
    bool remove(const std::string& plate);      // Delete
    void printAll() const;
    bool saveToFile(const std::string& filename) const;   // เขียนรถทั้งหมดลงไฟล์
    bool loadFromFile(const std::string& filename);       // อ่านรถจากไฟล์ (false = ไม่มีไฟล์)
};
