#include <iostream>
#include <sstream>
#include <string>
#include <vector>
#include <algorithm>
using namespace std;

#define MAX 100

string gram[MAX][MAX];  
int np = 0;  // số dòng của văn phạm

string concat(string a, string b) {
    string r = a;
    for (char c : b)
        if (r.find(c) == string::npos)
            r.push_back(c);
    return r;
}

//-----------------------
// Cấu trúc cây phân tích (parse tree) cho một ô trong bảng CYK.
struct Node {
    string var;       // biến (non-terminal) được tạo ra, ví dụ: "S"
    string prod;      // sản xuất sử dụng (ví dụ: "AB" nếu rule S->AB, hoặc terminal "a")
    int i, j;         // phạm vi xâu mà Node này sinh ra (từ i đến j)
    int split;        // nếu là sản xuất nhị thức, chỉ số phân chia (split index)
    Node* left;       // con trái
    Node* right;      // con phải
    Node(string v, string p, int i_, int j_) : var(v), prod(p), i(i_), j(j_), split(-1), left(nullptr), right(nullptr) {}
};

//-----------------------
// Hàm gen_comb: từ hai chuỗi (chứa các non-terminal) a và b,
// duyệt từng cặp ký tự (non-terminal) và kiểm tra xem có quy tắc nào có dạng A-> (B C)
// sao cho B là ký tự từ a và C là ký tự từ b. Nếu có, thêm A vào kết quả.
string gen_comb(string a, string b) {
    string res = "";
    string temp;
    for (char x : a) {
        for (char y : b) {
            temp = "";
            temp.push_back(x);
            temp.push_back(y);
            // Duyệt văn phạm: nếu có quy tắc A -> temp, thêm A vào res.
            for (int i = 0; i < np; i++) {
                for (int j = 1; gram[i][j] != ""; j++) {
                    if (gram[i][j] == temp)
                        res = concat(res, gram[i][0]);
                }
            }
        }
    }
    return res;
}

//-----------------------
// Hàm in cây phân tích (parse tree)
void printParseTree(Node* root, int indent = 0) {
    if(root == nullptr) return;
    for (int i = 0; i < indent; i++) cout << "  ";
    cout << root->var;
    if(root->left == nullptr && root->right == nullptr)
        cout << " -> " << root->prod;
    else
        cout << " -> " << root->prod << " (Ap dung:[" << root->i << "," << root->j << "])";
    cout << "\n";
    printParseTree(root->left, indent+1);
    printParseTree(root->right, indent+1);
}

string removeSpaces(const string &s) {
    string r = s;
    r.erase(remove_if(r.begin(), r.end(), ::isspace), r.end());
    return r;
}

int main()
{
    cout << "Nhap van pham (ket thuc bang dong trong):" << endl;
    string line;
    while(getline(cin, line)) {
        if(line.empty()) break;
        line = removeSpaces(line);
        line.erase(remove(line.begin(), line.end(), ';'), line.end());
        int pt = line.find("->");
        if(pt == string::npos) continue;
        gram[np][0] = line.substr(0, pt);
        string rhs = line.substr(pt+2);
        int j = 1;
        while(!rhs.empty()){
            int pos = rhs.find("|");
            if(pos == string::npos) {
                gram[np][j] = rhs;
                rhs = "";
            } else {
                gram[np][j] = rhs.substr(0, pos);
                rhs = rhs.substr(pos+1);
            }
            j++;
        }
        np++;
    }
    
    cout << "Nhap ky tu bat dau: ";
    string start;
    getline(cin, start);
    
    cout << "Nhap xau can kiem tra: ";
    string str;
    getline(cin, str);
    
    int n = str.size();
    
    vector<vector<vector<Node*>>> table(n, vector<vector<Node*>>(n));
    
    for (int i = 0; i < n; i++) {
        string st(1, str[i]);
        for (int r = 0; r < np; r++) {
            for (int k = 1; gram[r][k] != ""; k++) {
                if (gram[r][k] == st) {
                    Node* node = new Node(gram[r][0], st, i, i);
                    table[i][i].push_back(node);
                }
            }
        }
    }
    
    for (int len = 2; len <= n; len++) {
        for (int i = 0; i <= n - len; i++) {
            int j = i + len - 1;
            for (int k = i; k < j; k++) {
                for (auto leftNode : table[i][k]) {
                    for (auto rightNode : table[k+1][j]) {
                        string comb = leftNode->var + rightNode->var;
                        for (int r = 0; r < np; r++) {
                            for (int p = 1; gram[r][p] != ""; p++) {
                                if (gram[r][p] == comb) {
                                    Node* node = new Node(gram[r][0], comb, i, j);
                                    node->split = k;
                                    node->left = leftNode;
                                    node->right = rightNode;
                                    table[i][j].push_back(node);
                                }
                            }
                        }
                    }
                }
            }
        }
    }
    
    cout << "\nBang CYK:" << endl;
    for (int i = 0; i < n; i++) {
        for (int j = i; j < n; j++) {
            cout << "Cell[" << i << "][" << j << "]: ";
            for (auto node : table[i][j]) {
                cout << node->var << " ";
            }
            cout << endl;
        }
    }
    
    bool belongs = false;
    Node* parseTree = nullptr;
    for (auto node : table[0][n-1]) {
        if (node->var == start) {
            belongs = true;
            parseTree = node;
            break;
        }
    }
    
    if(belongs) {
        cout << "\nXau thuoc ngon ngu!" << endl;
        cout << "\nCay phan tich:" << endl;
        printParseTree(parseTree);
    }
    else {
        cout << "\nXau khong thuoc ngon ngu." << endl;
    }
    
    return 0;
}
