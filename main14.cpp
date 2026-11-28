#include <iostream>
#include <sstream>
#include <string>
#include <vector>
#include <unordered_set>
#include <algorithm>
#include <cassert>

using namespace std;

struct Node {
    int key;
    Node* left = nullptr;
    Node* right = nullptr;
    explicit Node(int k) : key(k) {}
};

struct Mem {
    size_t alloc = 0, freed = 0;
    size_t active() const { return alloc >= freed ? alloc - freed : 0; }
};

Node* makeNode(Mem& m, int k) { m.alloc++; return new Node(k); }
void freeNode(Mem& m, Node* p) { if (p) { delete p; m.freed++; } }

Node* insertRule(Node* r, int v, Mem& m) {
    if (!r) return makeNode(m, v);
    if (v < r->key) r->left = insertRule(r->left, v, m);
    else if (v > r->key) r->right = insertRule(r->right, v, m);
    return r;
}

Node* buildPrefix(istream& in, Mem& m, bool& ok) {
    string tok;
    if (!(in >> tok)) { ok = false; return nullptr; }
    if (tok == "#") return nullptr;
    try {
        size_t pos = 0;
        int v = stoi(tok, &pos);
        if (pos != tok.size()) { ok = false; return nullptr; }
        Node* n = makeNode(m, v);
        n->left = buildPrefix(in, m, ok);
        n->right = buildPrefix(in, m, ok);
        return n;
    } catch (...) { ok = false; return nullptr; }
}

Node* parsePrefix(const string& s, Mem& m, bool& ok) {
    istringstream iss(s);
    ok = true;
    Node* r = buildPrefix(iss, m, ok);
    string extra;
    if (iss >> extra) ok = false;
    return r;
}

void preRec(const Node* r, vector<int>& a) {
    if (!r) return;
    a.push_back(r->key);
    preRec(r->left, a);
    preRec(r->right, a);
}

void inRec(const Node* r, vector<int>& a) {
    if (!r) return;
    inRec(r->left, a);
    a.push_back(r->key);
    inRec(r->right, a);
}

void postRec(const Node* r, vector<int>& a) {
    if (!r) return;
    postRec(r->left, a);
    postRec(r->right, a);
    a.push_back(r->key);
}

// Ітеративний прямий обхід зі стеком на звичайному масиві (достатній рівень)
vector<int> preIter(const Node* r) {
    vector<int> a;
    if (!r) return a;
    const Node* st[256];
    size_t top = 0;
    st[top++] = r;
    while (top > 0) {
        const Node* cur = st[--top];
        a.push_back(cur->key);
        if (cur->right) st[top++] = cur->right;
        if (cur->left) st[top++] = cur->left;
    }
    return a;
}

size_t nCount(const Node* r) { return r ? 1 + nCount(r->left) + nCount(r->right) : 0; }
size_t lCount(const Node* r) {
    if (!r) return 0;
    return (!r->left && !r->right) ? 1 : lCount(r->left) + lCount(r->right);
}
size_t iCount(const Node* r) {
    if (!r || (!r->left && !r->right)) return 0;
    return 1 + iCount(r->left) + iCount(r->right);
}
size_t height(const Node* r) { return r ? 1 + max(height(r->left), height(r->right)) : 0; }

bool checkAcyclic(const Node* r, unordered_set<const Node*>& vis) {
    if (!r) return true;
    if (!vis.insert(r).second) return false;
    return checkAcyclic(r->left, vis) && checkAcyclic(r->right, vis);
}

void clearTree(Node*& r, Mem& m) {
    if (!r) return;
    clearTree(r->left, m);
    clearTree(r->right, m);
    freeNode(m, r);
    r = nullptr;
}

void printSeq(const string& lbl, const vector<int>& v) {
    cout << lbl << ": ";
    if (v.empty()) { cout << "порожньо\n"; return; }
    for (size_t i = 0; i < v.size(); ++i) cout << v[i] << (i + 1 < v.size() ? ", " : "\n");
}

void runTests() {
    struct TC { string p; vector<int> pre, in, post; size_t n, l, h; };
    vector<TC> tcs = {
        {"#", {}, {}, {}, 0, 0, 0},
        {"5 # #", {5}, {5}, {5}, 1, 1, 1},
        {"2 1 # # 3 # #", {2, 1, 3}, {1, 2, 3}, {1, 3, 2}, 3, 2, 2},
        {"3 2 1 # # # #", {3, 2, 1}, {1, 2, 3}, {1, 2, 3}, 3, 1, 3},
        {"1 # 2 # 3 # #", {1, 2, 3}, {1, 2, 3}, {3, 2, 1}, 3, 1, 3},
        {"4 2 # # 6 # #", {4, 2, 6}, {2, 4, 6}, {2, 6, 4}, 3, 2, 2},
        {"5 3 2 # # 4 # # 7 6 # # 8 # #", {5, 3, 2, 4, 7, 6, 8}, {2, 3, 4, 5, 6, 7, 8}, {2, 4, 3, 6, 8, 7, 5}, 7, 4, 3},
        {"1 2 # # 3 4 # # #", {1, 2, 3, 4}, {2, 1, 4, 3}, {2, 4, 3, 1}, 4, 2, 3},
        {"0 -2 # # 5 # #", {0, -2, 5}, {-2, 0, 5}, {-2, 5, 0}, 3, 2, 2},
        {"4 4 # # 4 # #", {4, 4, 4}, {4, 4, 4}, {4, 4, 4}, 3, 2, 2},
        // 5 власних: повне, вироджене вліво, вироджене вправо, неповне, з повтореннями
        {"50 30 20 # # 40 # # 70 60 # # 80 # #", {50, 30, 20, 40, 70, 60, 80}, {20, 30, 40, 50, 60, 70,
