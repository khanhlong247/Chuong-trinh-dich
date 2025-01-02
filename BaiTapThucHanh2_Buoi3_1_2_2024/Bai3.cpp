#include <iostream>
#include <string>
using namespace std;

string checkRelationalOperator(const string& input) {
    char state = 0;

    for (char c : input) {
        switch (state) {
            case 0:
                if (c == '<') {
                    state = 1;
                }
                else if (c == '=') {
                    state = 5;
                }
                else if (c == '>') {
                    state = 6;
                }
                else {
                    return "Khong hop le";
                } 
                break;
            case 1:
                if (c == '=') {
                    state = 2;
                }
                else if (c == '>') {
                    state = 3;
                }
                else {
                    return "Less than"; // "<"
                }
                break;
            case 2:
                return "Less or equal"; // "<="
            case 3:
                return "Not equal"; // "<>"
            case 5:
                return "Equal"; // "="
            case 6:
                if (c == '=') {
                    state = 7;
                }
                else {
                    return "Greater than"; // ">"
                }
                break;
            case 7:
                return "Greater or equal"; // ">="
            default:
                return "Khong hop le";
        }
    }

    // Kết quả cuối cùng dựa vào trạng thái
    switch (state) {
        case 0: return "Khong hop le";
        case 1: return "Less than"; // "<"
        case 2: return "Less or equal"; // "<="
        case 3: return "Not equal"; // "<>"
        case 4: return "Less than"; // "<"
        case 5: return "Equal"; // "="
        case 6: return "Greater than"; // ">"
        case 7: return "Greater or equal"; // ">"
        default: return "Khong hop le";
    }
}

int main() {
    string input;

    cout << "Nhap xau can kiem tra: ";
    cin >> input;

    string result = checkRelationalOperator(input);
    if (result == "Khong hop le") {
        cout << "Khong hop le!" << endl;
    } else {
        cout << "Quan he: " << result << endl;
    }

    return 0;
}
