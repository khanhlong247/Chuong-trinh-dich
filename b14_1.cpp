#include <iostream>
#include <sstream>
#include <string>
#include <vector>
#include <map>
#include <unordered_set>
#include <cctype>
using namespace std;

// --- CẤU TRÚC LƯU TRỮ VĂN PHẠM ---
// Lưu tất cả các quy tắc dưới dạng: LHS -> danh sách các RHS (chuỗi)
map<string, vector<string>> grammar;

// Sau khi nhập, ta tách thành 2 nhóm quy tắc:
// lexicalRules: các quy tắc có dạng X -> a, với a là ký tự thường (độ dài = 1)
// binaryRules: các quy tắc có dạng X -> YZ, với Y và Z có thể là terminal (chữ thường) hoặc non-terminal (chữ in hoa) (độ dài = 2)
map<string, vector<string>> lexicalRules; // key: "a"  --> list { "A", ... }
map<string, vector<string>> binaryRules;  // key: "aA", "bA", "cA", ... --> list { LHS như "S" hay "A", ... }

string startSymbol;  // ký hiệu bắt đầu (ví dụ: S)
  
// --- HÀM NHẬP VĂN PHẠM ---
// Nhập các dòng với định dạng: 
//    S -> aA | bA
//    A -> cA | bA | d
// Kết thúc nhập khi gặp dòng trống.
void inputGrammar() {
    cout << "Nhap van pham (ket thuc bang dong trong):\n";
    string line;
    while(getline(cin, line) && !line.empty()){
        istringstream iss(line);
        string lhs, arrow;
        iss >> lhs >> arrow;  // Lấy LHS và ký hiệu "->"
        string rules;
        getline(iss, rules);  // Lấy phần còn lại của dòng chứa các RHS
        vector<string> ruleList;
        size_t pos = 0;
        // Tách các quy tắc theo ký tự '|'
        while((pos = rules.find('|')) != string::npos){
            string token = rules.substr(0, pos);
            ruleList.push_back(token);
            rules.erase(0, pos + 1);
        }
        ruleList.push_back(rules); // thêm quy tắc cuối cùng
        // Loại bỏ khoảng trắng dư thừa ở đầu và cuối mỗi quy tắc
        for(auto &r : ruleList) {
            size_t start = r.find_first_not_of(" \t");
            size_t end = r.find_last_not_of(" \t");
            if(start != string::npos && end != string::npos)
                r = r.substr(start, end - start + 1);
            else
                r = "";
        }
        grammar[lhs] = ruleList;
    }
}

// --- XỬ LÝ VĂN PHẠM: TẠO BẢNG LEXICAL & BINARY RULES ---
// Duyệt qua grammar để tách các quy tắc có dạng "X -> a" (lexical)
// và "X -> YZ" (binary). Ở đây ta xét quy tắc có độ dài 1 (sau khi loại bỏ khoảng trắng) là lexical,
// và quy tắc có độ dài 2 là binary.
void processGrammar() {
    for(auto &entry : grammar) {
        string lhs = entry.first;
        for(auto &rhs : entry.second) {
            // Loại bỏ khoảng trắng trong chuỗi production
            string prod;
            for(char c : rhs) {
                if(!isspace(c))
                    prod.push_back(c);
            }
            if(prod.empty()) continue;
            if(prod.size() == 1 && islower(prod[0])) {
                // quy tắc lexical: ví dụ A -> d
                string key(1, prod[0]);
                lexicalRules[key].push_back(lhs);
            } else if(prod.size() == 2) {
                // quy tắc binary: ví dụ S -> aA, S -> bA, A -> cA, A -> bA, v.v.
                binaryRules[prod].push_back(lhs);
            }
            // Các quy tắc có độ dài khác có thể không được xử lý
        }
    }
}

// --- HÀM PHÂN TÍCH THEO PHƯƠNG PHÁP BOTTOM-UP ---
// Sử dụng đệ quy với backtracking.
// s: xâu hiện tại (hỗn hợp terminal và non-terminal)
// steps: lưu lại các bước chuyển để in ra nếu suy ra thành công
// visited: các trạng thái xâu đã duyệt (tránh lặp vô hạn)
bool bottomUpParse(const string &s, vector<string> &steps, unordered_set<string> &visited) {
    // Nếu xâu chỉ có 1 ký tự và bằng ký hiệu bắt đầu -> thành công
    if(s.size() == 1 && s == startSymbol) {
        cout << "\nXau duoc suy ra theo phuong phap bottom-up. Cac buoc suy ra:\n";
        for(auto &st : steps)
            cout << st << "\n";
        cout << "Ket qua: " << s << "\n";
        return true;
    }
    // Nếu trạng thái này đã duyệt, bỏ qua
    if(visited.count(s))
        return false;
    visited.insert(s);

    bool appliedReduction = false;
    
    // --- 1. THỰC HIỆN "PAIR REDUCTION" --- 
    // Duyệt từng cặp ký tự liền nhau từ phải qua trái.
    // Lưu ý: Bỏ đi điều kiện kiểm tra cả 2 ký tự là in hoa.
    for (int i = s.size() - 2; i >= 0; i--) {
        string pair = s.substr(i, 2);
        if(binaryRules.find(pair) != binaryRules.end()) {
            vector<string> candidates = binaryRules[pair];
            vector<string> filtered;
            // Nếu xâu dài hơn 2 thì ưu tiên các quy tắc mà LHS khác ký hiệu bắt đầu
            if(s.size() > 2) {
                for(auto X : candidates) {
                    if(X != startSymbol)
                        filtered.push_back(X);
                }
                if(filtered.empty())
                    filtered = candidates;
            } else { // nếu xâu chỉ có 2 ký tự, ưu tiên chọn quy tắc có LHS = startSymbol (nếu có)
                for(auto X : candidates) {
                    if(X == startSymbol)
                        filtered.push_back(X);
                }
                if(filtered.empty())
                    filtered = candidates;
            }
            // Thử từng quy tắc trong danh sách candidate (theo thứ tự đã lọc)
            for(auto X : filtered) {
                string newS = s;
                // Thay thế cặp tại vị trí i (độ dài 2) bằng ký hiệu X
                newS.replace(i, 2, X);
                string step = "Su dung quy tac: " + X + " -> " + pair + " ket qua: " + newS;
                steps.push_back(step);
                if(bottomUpParse(newS, steps, visited))
                    return true;
                steps.pop_back();
            }
            appliedReduction = true;  // Đã có ít nhất 1 bước giảm theo cặp được thực hiện
        }
    }
    
    // --- 2. NẾU KHÔNG CÓ BƯỚC "PAIR REDUCTION" THÌ THỰC HIỆN "TERMINAL REDUCTION" ---
    if(!appliedReduction) {
        for (int i = s.size() - 1; i >= 0; i--) {
            if(islower(s[i])) {
                string term(1, s[i]);
                if(lexicalRules.count(term)) {
                    vector<string> candidates = lexicalRules[term];
                    for(auto X : candidates) {
                        string newS = s;
                        newS.replace(i, 1, X);
                        string step = "Su dung quy tac: " + X + " -> " + term + " ket qua: " + newS;
                        steps.push_back(step);
                        if(bottomUpParse(newS, steps, visited))
                            return true;
                        steps.pop_back();
                    }
                    // Sau khi thử chuyển ký tự terminal ở vị trí này thì dừng (theo ý tưởng: chỉ thực hiện 1 terminal reduction tại 1 vị trí rồi kiểm tra lại cặp)
                    return false;
                }
            }
        }
    }
    
    return false;
}

int main() {
    // Nhập văn phạm từ bàn phím
    inputGrammar();
    
    // Nhập ký hiệu bắt đầu
    cout << "Nhap ky tu bat dau: ";
    cin >> startSymbol;
    cin.ignore();
    
    // Xử lý văn phạm để tách thành lexicalRules và binaryRules
    processGrammar();
    
    // Nhập xâu kết quả (xâu terminal)
    cout << "\nNhap xau can kiem tra: ";
    string input;
    cin >> input;
    
    vector<string> steps;
    unordered_set<string> visited;
    
    if(!bottomUpParse(input, steps, visited))
        cout << "\nXau khong duoc suy ra theo phuong phap bottom-up.\n";
    
    return 0;
}
