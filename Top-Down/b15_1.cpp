#include <iostream>
#include <sstream>
#include <string>
#include <map>
#include <vector>
#include <climits>
#include <unordered_set>
using namespace std;

// Các biến toàn cục
map<string, vector<string>> grammar;
string startSymbol;
string input;
bool foundDerivation = false;

void inputGrammar() {
    cout << "Nhap van pham (ket thuc bang dong trong):\n";
    string line;
    while(getline(cin, line) && !line.empty()){
         istringstream iss(line);
         string lhs, arrow;
         iss >> lhs >> arrow;  // Lấy vế trái và ký hiệu "->"
         string rules;
         getline(iss, rules);  // Lấy phần bên phải của dòng
         vector<string> ruleList;
         size_t pos = 0;
         // Tách các quy tắc dựa trên ký tự '|'
         while((pos = rules.find('|')) != string::npos) {
              string token = rules.substr(0, pos);
              ruleList.push_back(token);
              rules.erase(0, pos + 1);
         }
         ruleList.push_back(rules); // Thêm quy tắc cuối cùng
         // Xóa khoảng trắng dư thừa
         for(auto &rule : ruleList) {
              rule.erase(0, rule.find_first_not_of(" \t"));
              rule.erase(rule.find_last_not_of(" \t") + 1);
         }
         grammar[lhs] = ruleList;
    }
    cout << "Nhap ky tu bat dau: ";
    cin >> startSymbol;
    cin.ignore(); // Bỏ ký tự xuống dòng dư
}

/*
 * Kiểm tra một ký tự có phải là non-terminal hay không.
 * Với cách nhập trên, các non-terminal là các ký hiệu được lưu trong map 'grammar'
 */
bool isNonTerminal(char c) {
    string s(1, c);
    return grammar.find(s) != grammar.end();
}

int getMinLength(const string &nonTerminal, map<string, int> &memo, unordered_set<string>& visiting) {
    if(memo.find(nonTerminal) != memo.end())
         return memo[nonTerminal];
    // Nếu nonTerminal đang được xử lý thì trả về một giá trị rất lớn (giả sử không tối ưu)
    if(visiting.find(nonTerminal) != visiting.end()){
         return INT_MAX / 2;
    }
    visiting.insert(nonTerminal);
    
    // Nếu không có trong grammar -> là terminal
    if(grammar.find(nonTerminal) == grammar.end()){
         memo[nonTerminal] = 1;
         visiting.erase(nonTerminal);
         return 1;
    }
    
    int minLen = INT_MAX;
    for(auto &prod : grammar[nonTerminal]){
         int sum = 0;
         for(char c : prod){
              string sym(1, c);
              int len = getMinLength(sym, memo, visiting);
              // Nếu một phần sinh ra có “độ dài vô cùng” thì bỏ qua quy tắc đó
              if(len == INT_MAX/2) {
                  sum = INT_MAX/2;
                  break;
              }
              sum += len;
         }
         if(sum < minLen) minLen = sum;
    }
    memo[nonTerminal] = minLen;
    visiting.erase(nonTerminal);
    return minLen;
}

// Hàm bọc để gọi getMinLength với tập 'visiting' rỗng ban đầu
int getMinLength(const string &nonTerminal, map<string, int> &memo) {
    unordered_set<string> visiting;
    return getMinLength(nonTerminal, memo, visiting);
}

/*
 * Tính tổng độ dài tối thiểu có thể sinh từ một xâu (có cả terminal và non-terminal)
 */
int getMinLengthForString(const string &s, map<string, int> &memo) {
    int total = 0;
    for(char c : s){
         string sym(1, c);
         total += getMinLength(sym, memo);
    }
    return total;
}

void parseString(const string &current, vector<string> &steps) {
    // Lấy terminal prefix (chuỗi ký tự terminal liên tiếp bên trái)
    string terminalPrefix = "";
    for (char c : current) {
         if (isNonTerminal(c))
             break;
         terminalPrefix.push_back(c);
    }
    // Nếu terminal prefix không khớp với phần đầu của input thì cắt nhánh
    if(terminalPrefix.size() > input.size() || input.substr(0, terminalPrefix.size()) != terminalPrefix)
         return;
    
    // Cắt nhánh sớm nếu độ dài tối thiểu mà current có thể sinh ra vượt quá độ dài input
    {
         map<string, int> memo;
         int minPossible = getMinLengthForString(current, memo);
         if(minPossible > input.size())
             return;
    }
    
    // Nếu current không còn non-terminal nào thì đã thu được xâu chỉ gồm terminal
    bool hasNonTerminal = false;
    for(char c : current){
         if(isNonTerminal(c)) { hasNonTerminal = true; break; }
    }
    if(!hasNonTerminal) {
         if(current == input) {
              foundDerivation = true;
              cout << "\nXau duoc sinh ra boi van pham. Cach phan tich:\n";
              for(auto &step : steps)
                   cout << step << endl;
              cout << "Ket qua: " << current << "\n";
              cout << "----------------------------\n";
         }
         return;
    }
    
    // Tìm vị trí non-terminal bên trái nhất
    int pos = -1;
    for(int i = 0; i < current.size(); i++){
         if(isNonTerminal(current[i])){
              pos = i;
              break;
         }
    }
    if(pos == -1) return;
    
    string nonTerm(1, current[pos]);
    // Thử từng quy tắc của non-terminal vừa tìm được
    for(auto &prod : grammar[nonTerm]){
         // Thay thế ký hiệu non-terminal tại vị trí pos bằng quy tắc prod
         string newString = current;
         newString.replace(pos, 1, prod);
         steps.push_back("Su dung quy tac: " + nonTerm + " -> " + prod + " ket qua: " + newString);
         parseString(newString, steps);
         steps.pop_back();  // Backtracking
    }
}

int main() {
    // Nhập văn phạm
    inputGrammar();
    
    // Nhập xâu cần kiểm tra
    cout << "\nNhap xau can kiem tra: ";
    cin >> input;
    
    vector<string> steps;
    // Bắt đầu phân tích với ký hiệu bắt đầu
    parseString(startSymbol, steps);
    
    if(!foundDerivation) {
         cout << "\nXau khong duoc sinh ra boi van pham.\n";
    }
    return 0;
}
