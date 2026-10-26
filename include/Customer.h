#pragma once
#include <string>

// One rental record (node of the rental-history linked list)
struct Rental {
    int id;
    std::string plate;
    int days;
    double total;
    bool returned;
    Rental* next;
};

// One customer (node of the customer linked list)
struct Customer {
    std::string id, name;
    Rental* history;    // head of rental history list
    Customer* next;
};
