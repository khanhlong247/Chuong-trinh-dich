#include <iostream>
#include <sstream>
#include <string>
#include <vector>
#include <algorithm>
#include <iomanip>
using namespace std;

#define MAX 100

string gram[MAX][MAX];  
int np = 0;  // số dòng của văn phạm

// Hàm xóa khoảng trắng trong chuỗi (để thuận tiện khi nhập văn phạm)
string removeSpaces(const string &s) {
    string r = s;
    r.erase(remove_if(r.begin(), r.end(), ::isspace), r.end());
    return r;
}

bool isNonTerminal(char c) {
    return (c >= 'A' && c <= 'Z');
}

// Hàm thêm một ký hiệu vào tập FIRST nếu chưa có
bool addToFirst(vector<string>& firstSet, const string& sym) {
    if(find(firstSet.begin(), firstSet.end(), sym) == firstSet.end()){
        firstSet.push_back(sym);
        return true;
    }
    return false;
}

int main() {
    // Nhập văn phạm
    cout << "Nhap van pham (ket thuc bang dong trong):" << endl;
    string line;
    while(getline(cin, line)) {
        if(line.empty()) break;
        line = removeSpaces(line);
        // Xóa dấu chấm phẩy nếu có
        line.erase(remove(line.begin(), line.end(), ';'), line.end());
        int posArrow = line.find("->");
        if(posArrow == string::npos) continue;
        // Lưu ký hiệu không kết thúc (non-terminal) ở cột 0
        gram[np][0] = line.substr(0, posArrow);
        string rhs = line.substr(posArrow + 2);
        int j = 1;
        // Tách các sản xuất theo dấu |
        while(!rhs.empty()){
            int posBar = rhs.find("|");
            if(posBar == string::npos) {
                gram[np][j] = rhs;
                rhs = "";
            } else {
                gram[np][j] = rhs.substr(0, posBar);
                rhs = rhs.substr(posBar+1);
            }
            j++;
        }
        np++;
    }

    vector<vector<string>> FIRST(np);
    
    bool changed = true;
    while(changed) {
        changed = false;
        // Duyệt từng quy tắc A -> production, với A = gram[i][0]
        for (int i = 0; i < np; i++) {
            for (int j = 1; gram[i][j] != ""; j++) {
                string production = gram[i][j];
                // Nếu production là "none", đây là production epsilon
                if(production == "none") {
                    if(addToFirst(FIRST[i], "none"))
                        changed = true;
                    continue;
                }
                bool allNone = true; // Giả sử tất cả ký hiệu trong production đều có thể sinh ra epsilon ("none")
                // Duyệt từ trái sang phải trong production (giả sử mỗi ký hiệu là 1 ký tự)
                for (size_t pos = 0; pos < production.size(); pos++) {
                    char symbol = production[pos];
                    if (!isNonTerminal(symbol)) {
                        // Nếu là ký hiệu kết thúc (terminal), thêm vào FIRST(A)
                        if(addToFirst(FIRST[i], string(1, symbol)))
                            changed = true;
                        allNone = false;
                        break; // dừng duyệt production
                    } else {
                        // symbol là non-terminal, tìm vị trí của nó trong văn phạm
                        int index = -1;
                        for (int k = 0; k < np; k++) {
                            if(gram[k][0] == string(1, symbol)) {
                                index = k;
                                break;
                            }
                        }
                        // Nếu không tìm thấy (có thể lỗi nhập), xem như symbol là terminal
                        if(index == -1) {
                            if(addToFirst(FIRST[i], string(1, symbol)))
                                changed = true;
                            allNone = false;
                            break;
                        } else {
                            // Thêm tất cả các ký hiệu trong FIRST của non-terminal đó (ngoại trừ "none")
                            for (auto s : FIRST[index]) {
                                if(s != "none") {
                                    if(addToFirst(FIRST[i], s))
                                        changed = true;
                                }
                            }
                            // Nếu FIRST của non-terminal đó không chứa "none", dừng duyệt production
                            bool containsNone = false;
                            for(auto s: FIRST[index]){
                                if(s == "none") {
                                    containsNone = true;
                                    break;
                                }
                            }
                            if(!containsNone) {
                                allNone = false;
                                break;
                            }
                        }
                    }
                }
                // Nếu tất cả các ký hiệu trong production đều có thể sinh ra "none", thêm "none" vào FIRST(A)
                if(allNone) {
                    if(addToFirst(FIRST[i], "none"))
                        changed = true;
                }
            }
        }
    }
    
    // In ra các tập FIRST
    cout << "\nFIRST sets:\n";
    for (int i = 0; i < np; i++) {
        cout << "FIRST(" << gram[i][0] << ") = { ";
        for (auto sym : FIRST[i]) {
            cout << sym << " ";
        }
        cout << "}\n";
    }
    
    return 0;
}
