#include <iostream>
#include <string>
#include <cctype>
#include <vector>

using namespace std;

// Hàm kiểm tra tính hợp lệ của tên theo quy tắc Pascal
bool isValidPascalName(const string& str) {
    if (str.empty() || str.size() > 255) return false;
    if (!isalpha(str[0])) return false;
    for (size_t i = 1; i < str.size(); i++) {
        if (!isalnum(str[i]) && str[i] != '_') return false;
    }
    return true;
}

// Hàm kiểm tra xâu có phải là số nguyên hợp lệ
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

// Hàm kiểm tra xâu có phải là số thực hợp lệ
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

// Hàm kiểm tra xâu có phải là toán tử quan hệ hợp lệ
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

// Hàm phân tích từ vựng
vector<pair<string, string>> lexicalAnalysis(const string& expression) {
    vector<pair<string, string>> tokens;
    string currentToken;

    for (size_t i = 0; i < expression.size(); i++) {
        char ch = expression[i];

        // Xử lý khoảng trắng
        if (isspace(ch)) {
            if (!currentToken.empty()) {
                tokens.push_back({"unknown", currentToken});
                currentToken.clear();
            }
            continue;
        }

        // Xử lý toán tử
        if (ch == '<' || ch == '>' || ch == '=' || ch == '+' || ch == '-' || ch == '*' || ch == '/' || ch == ';') {
            if (!currentToken.empty()) {
                tokens.push_back({"unknown", currentToken});
                currentToken.clear();
            }

            // Kiểm tra toán tử hai ký tự
            if (i + 1 < expression.size() &&
                ((ch == '<' && (expression[i + 1] == '=' || expression[i + 1] == '>')) ||
                 (ch == '>' && expression[i + 1] == '=') ||
                 (ch == '=' && expression[i + 1] == '='))) {
                tokens.push_back({"operator", string(1, ch) + string(1, expression[i + 1])});
                i++;
            } else {
                tokens.push_back({"operator", string(1, ch)});
            }
            continue;
        }

        currentToken += ch;
    }

    if (!currentToken.empty()) {
        tokens.push_back({"unknown", currentToken});
    }

    // Phân loại các token
    for (auto& token : tokens) {
        if (isValidPascalName(token.second)) {
            token.first = "Ten";
        } else if (isInteger(token.second)) {
            token.first = "So nguyen";
        } else if (isReal(token.second)) {
            token.first = "SO thuc";
        } else if (token.first == "unknown" && checkRelationalOperator(token.second) != "Invalid") {
            token.first = "Toan tu";
        }
    }

    return tokens;
}

int main() {
    string expression;
    cout << "Nhap bieu thuc: ";
    getline(cin, expression);

    vector<pair<string, string>> tokens = lexicalAnalysis(expression);

    for (const auto& token : tokens) {
        cout << "<" << token.first << ", " << token.second << ">" << endl;
    }

    return 0;
}
