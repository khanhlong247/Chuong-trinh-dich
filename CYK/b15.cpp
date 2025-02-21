#include <iostream>
#include <sstream>
#include <string>
#include <vector>
#include <algorithm>
#include <iomanip>
using namespace std;

#define MAX 100

string gram[MAX][MAX];  
int np = 0;

string concat(string a, string b) {
    string r = a;
    for (char c : b)
        if (r.find(c) == string::npos)
            r.push_back(c);
    return r;
}

struct Node {
    string var;
    string prod;
    int i, j;
    int split;
    Node* left;
    Node* right;
    Node(string v, string p, int i_, int j_) : var(v), prod(p), i(i_), j(j_), split(-1), left(nullptr), right(nullptr) {}
};

void printCYKTable(const vector<vector<vector<Node*>>>& table, int n) {
    cout << "\nBang CYK:\n";
    // Hàng dưới cùng là len = 1, hàng trên cùng là len = n
    for (int len = 1; len <= n; len++) {
        // Thụt đầu dòng (số lần tab) phụ thuộc vào len
        // len càng lớn thì ta càng thụt vào nhiều để tạo dạng tam giác
        for (int sp = 0; sp < len - 1; sp++) {
            cout << "\t";
        }

        // Với độ dài len, i chạy từ 0 đến (n - len)
        // j = i + (len - 1)
        for (int i = 0; i <= n - len; i++) {
            int j = i + len - 1;
            cout << "[";
            auto &cell = table[i][j];
            for (size_t k = 0; k < cell.size(); k++) {
                cout << cell[k]->var;
                if (k != cell.size() - 1) cout << ", ";
            }
            cout << "]\t";
        }
        cout << "\n";
    }
    cout << "\n";
}

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
    
    printCYKTable(table, n);
    
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