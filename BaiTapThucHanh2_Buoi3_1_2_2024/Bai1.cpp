#include <iostream>
#include <string>
#include <cctype>

using namespace std;

bool isValidPascalName(const string& str) {
    // Kiểm tra độ dài
    if (str.empty() || str.size() > 255) {
        return false;
    }

    // Kiểm tra ký tự đầu tiên
    if (!isalpha(str[0])) {
        return false;
    }

    // Kiểm tra các ký tự còn lại
    for (size_t i = 1; i < str.size(); i++) {
        if (!isalnum(str[i]) && str[i] != '_') {
            return false;
        }
    }

    return true;
}

int main() {
    string input;

    cout << "Nhap ten can kiem tra: ";
    cin >> input;

    if (isValidPascalName(input)) {
        cout << "Ten hop le!" << endl;
    } else {
        cout << "Ten khong hop le!" << endl;
    }

    return 0;
}
