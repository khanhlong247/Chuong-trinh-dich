#include <iostream>
#include <string>
#include <cctype>

using namespace std;

// Hàm kiểm tra xâu có phải là số nguyên hợp lệ
bool isInteger(const string& str) {
    if (str.empty()) return false;
    size_t start = 0;

    if (str[0] == '-' || str[0] == '+') {
        start = 1;
    }

    // Trường hợp số 0 là ký tự đầu tiên
    if (str[start] == '0' && str.size() > start + 1) {
        return false; // Không hợp lệ nếu có nhiều chữ số sau số 0
    }

    // Tất cả các ký tự còn lại phải là chữ số
    for (size_t i = start; i < str.size(); i++) {
        if (!isdigit(str[i])) {
            return false;
        }
    }
    return true;
}

// Hàm kiểm tra xâu có phải là số thực hợp lệ
bool isReal(const string& str) {
    if (str.empty()) return false;
    size_t start = 0;
    bool hasDot = false, hasE = false;

    if (str[0] == '-' || str[0] == '+') {
        start = 1;
    }

    for (size_t i = start; i < str.size(); i++) {
        char ch = str[i];

        if (ch == '.') {
            if (hasDot || hasE) return false;
            hasDot = true;
        }
        // Kiểm tra 'E' hoặc 'e'
        else if (ch == 'E' || ch == 'e') {
            if (hasE || i == start) return false;
            hasE = true;
            if (i + 1 < str.size() && (str[i + 1] == '+' || str[i + 1] == '-')) {
                i++;
            }
            if (i + 1 >= str.size() || !isdigit(str[i + 1])) return false; // Phải có số sau 'E'
        }
        // Kiểm tra các ký tự khác
        else if (!isdigit(ch)) {
            return false;
        }
    }

    if (str[start] == '0' && !hasDot && !hasE && str.size() > start + 1) {
        return false; 
    }

    return hasDot || hasE; 
}

int main() {
    string input;

    cout << "Nhap xau can kiem tra: ";
    cin >> input;

    if (isInteger(input)) {
        cout << "So nguyen hop le!" << endl;
    } else if (isReal(input)) {
        cout << "So thuc hop le!" << endl;
    } else {
        cout << "Khong hop le!" << endl;
    }

    return 0;
}
