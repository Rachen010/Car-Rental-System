#include "Utils.h"
#include <iostream>
#include <cctype>
using namespace std;

string toUpper(string s) {
    for (size_t i = 0; i < s.size(); i++) s[i] = toupper((unsigned char)s[i]);
    return s;
}

// ใช้ getline เสมอ (แทน cin >>) เพื่อไม่ให้มี newline ค้างใน buffer
string readLine(const string& prompt) {
    string s;
    cout << prompt;
    getline(cin, s);
    return s;
}

// ถ้าผู้ใช้พิมพ์ไม่ใช่ตัวเลข (เช่น "abc" หรือ "12x") จะแจ้งเตือนแล้วถามใหม่
// ไม่ให้โปรแกรมค้างหรือหยุดทำงาน
int readInt(const string& prompt) {
    while (true) {
        string s = readLine(prompt);
        try {
            size_t pos;
            int v = stoi(s, &pos);
            if (pos == s.size()) return v;          // แปลงได้ครบทั้งข้อความ
        } catch (...) {}
        cout << "  Invalid number, try again.\n";
    }
}

double readDouble(const string& prompt) {
    while (true) {
        string s = readLine(prompt);
        try {
            size_t pos;
            double v = stod(s, &pos);
            if (pos == s.size()) return v;
        } catch (...) {}
        cout << "  Invalid number, try again.\n";
    }
}
