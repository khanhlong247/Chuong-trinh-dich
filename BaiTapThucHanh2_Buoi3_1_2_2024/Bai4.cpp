#include <iostream>
#include <string>
#include <cctype>

using namespace std;

bool isValidPascalName(const string& str) {
    if (str.empty() || str.size() > 255) return false;
    if (!isalpha(str[0])) return false;
    for (size_t i = 1; i < str.size(); i++) {
        if (!isalnum(str[i]) && str[i] != '_') return false;
    }
    return true;
}

bool isInteger(const string& str) {
    if (str.empty()) return false;
    size_t start = 0;
    if (str[0] == '-' || str[0] == '+') start = 1;
    if (str[start] == '0' && str.size() > start + 1) return false;
    for (size_t i = start; i < str.size(); i++) {
        if (!isdigit(str[i])) return false;
    }
    return true;
}

bool isReal(const string& str) {
    if (str.empty()) return false;
    size_t start = 0;
    bool hasDot = false, hasE = false;
    if (str[0] == '-' || str[0] == '+') start = 1;
    for (size_t i = start; i < str.size(); i++) {
        char ch = str[i];
        if (ch == '.') {
            if (hasDot || hasE) return false;
            hasDot = true;
        } else if (ch == 'E' || ch == 'e') {
            if (hasE || i == start) return false;
            hasE = true;
            if (i + 1 < str.size() && (str[i + 1] == '+' || str[i + 1] == '-')) i++;
            if (i + 1 >= str.size() || !isdigit(str[i + 1])) return false;
        } else if (!isdigit(ch)) {
            return false;
        }
    }
    if (str[start] == '0' && !hasDot && !hasE && str.size() > start + 1) return false;
    return hasDot || hasE;
}

string checkRelationalOperator(const string& input) {
    char state = 0;
    for (char c : input) {
        switch (state) {
            case 0:
                if (c == '<') state = 1;
                else if (c == '=') state = 5;
                else if (c == '>') state = 6;
                else return "Invalid";
                break;
            case 1:
                if (c == '=') state = 2;
                else if (c == '>') state = 3;
                else return "Less than";
                break;
            case 2: return "Less or equal";
            case 3: return "Not equal";
            case 5: return "Equal";
            case 6:
                if (c == '=') state = 7;
                else return "Greater than";
                break;
            case 7: return "Greater or equal";
            default: return "Invalid";
        }
    }
    switch (state) {
        case 1: return "Less than";
        case 2: return "Less or equal";
        case 3: return "Not equal";
        case 5: return "Equal";
        case 6: return "Greater than";
        case 7: return "Greater or equal";
        default: return "Invalid";
    }
}

int main() {
    string input;
    cout << "Nhap xau can kiem tra: ";
    cin >> input;

    if (isValidPascalName(input)) {
        cout << "Loai xau: Ten hop le" << endl;
    } else if (isInteger(input)) {
        cout << "Loai xau: So nguyen hop le" << endl;
    } else if (isReal(input)) {
        cout << "Loai xau: So thuc hop le" << endl;
    } else {
        string relation = checkRelationalOperator(input);
        if (relation != "Invalid") {
            cout << "Loai xau: Toan tu quan he hop le - " << relation << endl;
        } else {
            cout << "Loai xau: Khong hop le" << endl;
        }
    }

    return 0;
}
