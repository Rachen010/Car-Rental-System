#include "CarBST.h"
#include <iostream>
#include <iomanip>
#include <fstream>
#include <sstream>
#include <vector>
using namespace std;

// Create: เริ่มต้นเป็นต้นไม้ว่าง
CarBST::CarBST() : root(nullptr) {}
CarBST::~CarBST() { destroy(root); }

// ---------- Insert ----------
// เปรียบเทียบทะเบียนแล้วเดินลงซ้าย/ขวาจนเจอที่ว่าง จึงสร้าง node ใหม่
// ถ้าทะเบียนซ้ำ จะไม่เพิ่ม และตั้ง ok = false
CarNode* CarBST::insertNode(CarNode* n, const Car& c, bool& ok) {
    if (!n) { ok = true; return new CarNode(c); }   // เจอที่ว่าง -> สร้าง node ใหม่
    if (c.plate < n->data.plate)      n->left  = insertNode(n->left, c, ok);
    else if (c.plate > n->data.plate) n->right = insertNode(n->right, c, ok);
    else ok = false;                                // ทะเบียนซ้ำ
    return n;
}

// ---------- Delete ----------
// แบ่งเป็น 3 กรณี เมื่อเจอ node ที่ต้องการลบ
//   1) ไม่มีลูก           -> ลบทิ้งได้เลย
//   2) มีลูกด้านเดียว      -> ให้ลูกนั้นขึ้นมาแทนที่
//   3) มีลูกสองข้าง        -> หา "inorder successor" (ค่าน้อยสุดของฝั่งขวา)
//                            มาแทนค่า แล้วลบ successor ตัวนั้นออกจากฝั่งขวา
CarNode* CarBST::removeNode(CarNode* n, const string& plate, bool& ok) {
    if (!n) return nullptr;                         // ไม่พบทะเบียนนี้
    if (plate < n->data.plate)      n->left  = removeNode(n->left, plate, ok);
    else if (plate > n->data.plate) n->right = removeNode(n->right, plate, ok);
    else {                                          // เจอ node ที่ต้องลบ
        ok = true;
        if (!n->left)  { CarNode* r = n->right; delete n; return r; }  // กรณี 1 และ 2
        if (!n->right) { CarNode* l = n->left;  delete n; return l; }  // กรณี 2
        // กรณี 3: มีลูกสองข้าง
        CarNode* s = n->right;
        while (s->left) s = s->left;                // หา successor = ซ้ายสุดของฝั่งขวา
        n->data = s->data;                          // คัดลอกข้อมูล successor มาแทน
        bool dummy;
        n->right = removeNode(n->right, s->data.plate, dummy);  // ลบ successor เดิมทิ้ง
    }
    return n;
}

// ---------- Inorder Traversal ----------
// เดิน ซ้าย -> ตัวเอง -> ขวา ทำให้ได้ผลลัพธ์เรียงตามทะเบียนจากน้อยไปมาก
void CarBST::inorder(CarNode* n) const {
    if (!n) return;
    inorder(n->left);
    cout << left << setw(12) << n->data.plate
         << setw(12) << n->data.brand
         << setw(12) << n->data.model
         << right << setw(10) << fixed << setprecision(2) << n->data.pricePerDay
         << "   " << (n->data.available ? "Available" : "Rented") << "\n";
    inorder(n->right);
}

// ลบ node ทั้งต้นแบบ postorder (ลูกก่อน แล้วค่อยลบตัวเอง) กัน memory leak
void CarBST::destroy(CarNode* n) {
    if (!n) return;
    destroy(n->left);
    destroy(n->right);
    delete n;
}

// ---------- ฟังก์ชัน public ----------
bool CarBST::insert(const Car& c) {
    bool ok = false;
    root = insertNode(root, c, ok);
    return ok;
}

// ---------- Search ----------
// เริ่มจาก root เทียบทะเบียน: น้อยกว่าไปซ้าย มากกว่าไปขวา
// ถ้าเจอคืน pointer ของข้อมูลรถ (แก้ไขสถานะได้) ถ้าไม่เจอคืน nullptr
Car* CarBST::search(const string& plate) {
    CarNode* cur = root;
    while (cur) {
        if (plate == cur->data.plate) return &cur->data;
        cur = (plate < cur->data.plate) ? cur->left : cur->right;
    }
    return nullptr;
}

bool CarBST::remove(const string& plate) {
    bool ok = false;
    root = removeNode(root, plate, ok);
    return ok;
}

void CarBST::printAll() const {
    if (!root) { cout << "  (no cars)\n"; return; }
    cout << left << setw(12) << "Plate" << setw(12) << "Brand"
         << setw(12) << "Model" << right << setw(10) << "Price/day"
         << "   Status\n";
    cout << string(60, '-') << "\n";
    inorder(root);
}

// ---------- บันทึก / โหลดไฟล์ ----------
// รูปแบบ cars.txt: 1 บรรทัด = 1 คัน แยกช่องด้วย TAB
//   plate <TAB> brand <TAB> model <TAB> pricePerDay <TAB> available(1/0)
// เขียนด้วย preorder เพื่อให้ตอนโหลดกลับมาแล้วรูปร่างต้นไม้ BST เหมือนเดิม
void CarBST::saveNode(CarNode* n, ostream& out) const {
    if (!n) return;
    out << n->data.plate << '\t' << n->data.brand << '\t' << n->data.model << '\t'
        << fixed << setprecision(2) << n->data.pricePerDay << '\t'
        << (n->data.available ? 1 : 0) << '\n';
    saveNode(n->left, out);
    saveNode(n->right, out);
}

bool CarBST::saveToFile(const string& filename) const {
    ofstream out(filename.c_str());
    if (!out) return false;
    saveNode(root, out);
    return true;
}

bool CarBST::loadFromFile(const string& filename) {
    ifstream in(filename.c_str());
    if (!in) return false;
    string line;
    while (getline(in, line)) {
        if (!line.empty() && line[line.size() - 1] == '\r') line.erase(line.size() - 1);
        if (line.empty()) continue;
        vector<string> f;
        stringstream ss(line);
        string part;
        while (getline(ss, part, '\t')) f.push_back(part);
        if (f.size() < 5) continue;                 // บรรทัดเสีย ข้าม
        try {
            Car c;
            c.plate = f[0]; c.brand = f[1]; c.model = f[2];
            c.pricePerDay = stod(f[3]);
            c.available = (f[4] == "1");
            insert(c);
        } catch (...) { /* ข้ามบรรทัดที่อ่านค่าไม่ได้ */ }
    }
    return true;
}
