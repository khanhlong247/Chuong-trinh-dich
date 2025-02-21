#include <iostream>
#include <sstream>
#include <string>
#include <vector>
#include <algorithm>
using namespace std;

#define MAX 100

string gram[MAX][MAX];
int np = 0; // số dòng của văn phạm

// Hàm loại bỏ khoảng trắng trong chuỗi
string removeSpaces(const string &s) {
    string r = s;
    r.erase(remove_if(r.begin(), r.end(), ::isspace), r.end());
    return r;
}

// Hàm kiểm tra ký tự có phải là non-terminal hay không.
bool isNonTerminal(char c) {
    return (c >= 'A' && c <= 'Z');
}

// Hàm thêm phần tử vào tập (vector<string>) nếu chưa có
bool addToSet(vector<string>& set, const string& sym) {
    if(find(set.begin(), set.end(), sym) == set.end()){
        set.push_back(sym);
        return true;
    }
    return false;
}

// Hàm tìm chỉ số của non-terminal trong mảng văn phạm (giả sử non-terminal là 1 ký tự)
int getNonTerminalIndex(char nt) {
    for (int i = 0; i < np; i++) {
        // gram[i][0] chứa non-terminal của quy tắc
        if(!gram[i][0].empty() && gram[i][0][0] == nt)
            return i;
    }
    return -1;
}

// Hàm tính FIRST cho toàn bộ văn phạm
void computeFIRST(vector<vector<string>>& FIRSTSets) {
    // Khởi tạo FIRST cho mỗi non-terminal là rỗng
    FIRSTSets.assign(np, vector<string>());
    
    bool changed = true;
    while(changed) {
        changed = false;
        // Duyệt từng quy tắc A -> productions
        for (int i = 0; i < np; i++) {
            for (int j = 1; gram[i][j] != ""; j++) {
                string production = gram[i][j];
                // Nếu production chính là "none" (epsilon)
                if(production == "none") {
                    if(addToSet(FIRSTSets[i], "none"))
                        changed = true;
                    continue;
                }
                bool allEpsilon = true;
                // Duyệt từ trái sang phải của production
                for (size_t pos = 0; pos < production.size(); pos++) {
                    char symbol = production[pos];
                    if (!isNonTerminal(symbol)) {
                        // Nếu là terminal, thêm nó và dừng duyệt production
                        if(addToSet(FIRSTSets[i], string(1, symbol)))
                            changed = true;
                        allEpsilon = false;
                        break;
                    } else {
                        int idx = getNonTerminalIndex(symbol);
                        if(idx == -1) { // nếu không tìm thấy, coi như là terminal
                            if(addToSet(FIRSTSets[i], string(1, symbol)))
                                changed = true;
                            allEpsilon = false;
                            break;
                        } else {
                            // Thêm tất cả các phần tử trong FIRST của non-terminal đó, trừ "none"
                            for (auto s : FIRSTSets[idx]) {
                                if(s != "none") {
                                    if(addToSet(FIRSTSets[i], s))
                                        changed = true;
                                }
                            }
                            // Nếu FIRST của non-terminal đó không chứa "none", dừng duyệt production
                            if(find(FIRSTSets[idx].begin(), FIRSTSets[idx].end(), "none") == FIRSTSets[idx].end()){
                                allEpsilon = false;
                                break;
                            }
                        }
                    }
                }
                if(allEpsilon) {
                    if(addToSet(FIRSTSets[i], "none"))
                        changed = true;
                }
            }
        }
    }
}

// Hàm tính FIRST của một chuỗi ký hiệu (chuỗi có thể chứa terminal và non-terminal)
vector<string> computeFirstString(const string &s, const vector<vector<string>>& FIRSTSets) {
    vector<string> result;
    // Nếu chuỗi rỗng, trả về tập rỗng (nghĩa là beta không tồn tại)
    if(s.empty())
        return result;
    
    bool allEpsilon = true;
    for (size_t pos = 0; pos < s.size(); pos++) {
        char symbol = s[pos];
        if(!isNonTerminal(symbol)) {
            // Terminal: thêm và dừng
            addToSet(result, string(1, symbol));
            allEpsilon = false;
            break;
        } else {
            int idx = getNonTerminalIndex(symbol);
            if(idx == -1) {
                addToSet(result, string(1, symbol));
                allEpsilon = false;
                break;
            } else {
                // Thêm tất cả các ký hiệu trong FIRST của non-terminal đó, ngoại trừ "none"
                for(auto sym : FIRSTSets[idx]) {
                    if(sym != "none")
                        addToSet(result, sym);
                }
                // Nếu không chứa "none", dừng
                if(find(FIRSTSets[idx].begin(), FIRSTSets[idx].end(), "none") == FIRSTSets[idx].end()){
                    allEpsilon = false;
                    break;
                }
            }
        }
    }
    // Nếu tất cả các ký hiệu đều có thể sinh ra epsilon, ta có thể nói chuỗi s có FIRST là "none"
    if(allEpsilon)
        addToSet(result, "none");
    
    return result;
}

// Hàm tính FOLLOW cho các non-terminal của văn phạm
void computeFOLLOW(const vector<vector<string>>& FIRSTSets, vector<vector<string>>& FOLLOWSets) {
    // Khởi tạo FOLLOW cho mỗi non-terminal là rỗng
    FOLLOWSets.assign(np, vector<string>());
    
    // Quy tắc 1: với ký hiệu bắt đầu (gram[0][0]), thêm dấu "$"
    addToSet(FOLLOWSets[0], "$");
    
    bool changed = true;
    while(changed) {
        changed = false;
        // Duyệt từng quy tắc A -> productions
        for (int i = 0; i < np; i++) {
            string A = gram[i][0];
            for (int j = 1; gram[i][j] != ""; j++) {
                string production = gram[i][j];
                // Duyệt từng vị trí của production
                for (size_t pos = 0; pos < production.size(); pos++) {
                    char symbol = production[pos];
                    if(isNonTerminal(symbol)) {
                        int idxB = getNonTerminalIndex(symbol);
                        if(idxB == -1) continue; // B không tìm thấy (không hợp lệ)
                        
                        // β là phần sau của production sau B
                        string beta = (pos + 1 < production.size()) ? production.substr(pos + 1) : "";
                        vector<string> firstBeta = computeFirstString(beta, FIRSTSets);
                        
                        // Quy tắc 2: Thêm tất cả ký hiệu trong FIRST(beta) (ngoại trừ "none") vào FOLLOW(B)
                        for (auto f : firstBeta) {
                            if(f != "none") {
                                if(addToSet(FOLLOWSets[idxB], f))
                                    changed = true;
                            }
                        }
                        
                        // Quy tắc 3: Nếu beta rỗng hoặc FIRST(beta) chứa "none", thêm FOLLOW(A) vào FOLLOW(B)
                        if(beta.empty() || find(firstBeta.begin(), firstBeta.end(), "none") != firstBeta.end()){
                            for(auto f : FOLLOWSets[i]) {
                                if(addToSet(FOLLOWSets[idxB], f))
                                    changed = true;
                            }
                        }
                    }
                }
            }
        }
    }
}

int main() {
    // Nhập văn phạm từ bàn phím
    cout << "Nhap van pham (ket thuc bang dong trong):" << endl;
    string line;
    while(getline(cin, line)) {
        if(line.empty())
            break;
        line = removeSpaces(line);
        // Xóa dấu chấm phẩy nếu có
        line.erase(remove(line.begin(), line.end(), ';'), line.end());
        int posArrow = line.find("->");
        if(posArrow == string::npos)
            continue;
        // Lưu non-terminal ở cột 0
        gram[np][0] = line.substr(0, posArrow);
        string rhs = line.substr(posArrow + 2);
        int j = 1;
        // Tách các sản xuất theo dấu '|'
        while(!rhs.empty()){
            int posBar = rhs.find("|");
            if(posBar == string::npos) {
                gram[np][j] = rhs;
                rhs = "";
            } else {
                gram[np][j] = rhs.substr(0, posBar);
                rhs = rhs.substr(posBar + 1);
            }
            j++;
        }
        np++;
    }
    
    // Tính FIRST cho văn phạm
    vector<vector<string>> FIRSTSets;
    computeFIRST(FIRSTSets);
    
    // Tính FOLLOW cho văn phạm
    vector<vector<string>> FOLLOWSets;
    computeFOLLOW(FIRSTSets, FOLLOWSets);
    
    // In ra các tập FOLLOW
    cout << "\nFOLLOW sets:\n";
    for (int i = 0; i < np; i++) {
        cout << "FOLLOW(" << gram[i][0] << ") = { ";
        for (auto sym : FOLLOWSets[i])
            cout << sym << " ";
        cout << "}\n";
    }
    
    return 0;
}
