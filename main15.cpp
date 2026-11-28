#include <iostream>
#include <vector>
#include <string>
#include <unordered_set>
#include <algorithm>
#include <limits>
#include <cassert>

using namespace std;

struct Node {
    int key;
    Node *left = nullptr, *right = nullptr;
    explicit Node(int k) : key(k) {}
};

struct Mem {
    size_t alloc = 0, freed = 0;
    size_t active() const { return alloc >= freed ? alloc - freed : 0; }
};

Node* makeN(Mem& m, int k) { m.alloc++; return new Node(k); }
void freeN(Mem& m, Node* p) { if (p) { delete p; m.freed++; } }

void inorder(const Node* r, vector<int>& out) {
    if (!r) return;
    inorder(r->left, out);
    out.push_back(r->key);
    inorder(r->right, out);
}

size_t height(const Node* r) { return r ? 1 + max(height(r->left), height(r->right)) : 0; }
size_t countN(const Node* r) { return r ? 1 + countN(r->left) + countN(r->right) : 0; }

void clearT(Node*& r, Mem& m) {
    if (!r) return;
    clearT(r->left, m); clearT(r->right, m);
    freeN(m, r); r = nullptr;
}

// 1. Перевірка діапазону BST
bool checkRange(const Node* n, long long lo, long long hi, string& err) {
    if (!n) return true;
    long long k = n->key;
    if (k <= lo || k >= hi) {
        err = "Порушено межу (" + to_string(lo) + ".." + to_string(hi) + ") вузлом " + to_string(k);
        return false;
    }
    return checkRange(n->left, lo, k - 1, err) && checkRange(n->right, k + 1, hi, err);
}

bool validBst(const Node* r, string& err) {
    long long inf = numeric_limits<long long>::max() / 4;
    return checkRange(r, -inf, inf, err);
}

// 2. Пошук (рекурсивний та ітеративний)
struct SRes { bool found = false; size_t comps = 0; vector<int> path; };

void sRecStep(const Node* c, int x, SRes& r) {
    if (!c) return;
    r.path.push_back(c->key);
    r.comps += 2;
    if (x == c->key) { r.comps--; r.found = true; return; }
    sRecStep(x < c->key ? c->left : c->right, x, r);
}

SRes searchRec(const Node* root, int x) { SRes r; sRecStep(root, x, r); return r; }

SRes searchIter(const Node* root, int x) {
    SRes r;
    for (const Node* c = root; c; ) {
        r.path.push_back(c->key);
        r.comps++;
        if (x == c->key) { r.found = true; return r; }
        r.comps++;
        c = (x < c->key) ? c->left : c->right;
    }
    return r;
}

struct ORes { bool ok = false; size_t comps = 0; string msg; };

// 3. Вставлення (заборона дублікатів)
Node* insBst(Node* root, int x, Mem& m, ORes& o) {
    o = ORes{};
    if (!root) { o.ok = true; o.msg = "Вставлено корінь " + to_string(x); return makeN(m, x); }
    Node *c = root, *p = nullptr; bool goL = false;
    while (c) {
        p = c;
        o.comps++;
        if (x == c->key) { o.msg = "Дублікат " + to_string(x); return root; }
        o.comps++;
        goL = (x < c->key);
        c = goL ? c->left : c->right;
    }
    Node* nn = makeN(m, x);
    if (goL) p->left = nn; else p->right = nn;
    o.ok = true;
    o.msg = "Вставлено " + to_string(x);
    return root;
}

Node* minN(Node* n, size_t& comps) {
    while (n && n->left) { comps++; n = n->left; }
    return n;
}

// 4–6. Видалення (0, 1, 2 нащадки; замінник — найменший у правому піддереві)
Node* delBst(Node* root, int x, Mem& m, ORes& o) {
    o = ORes{};
    if (!root) { o.msg = "Не знайдено " + to_string(x); return nullptr; }
    o.comps++;
    if (x < root->key) { root->left = delBst(root->left, x, m, o); return root; }
    o.comps++;
    if (x > root->key) { root->right = delBst(root->right, x, m, o); return root; }

    o.ok = true;
    if (!root->left && !root->right) { freeN(m, root); o.msg = "Видалено листок " + to_string(x); return nullptr; }
    if (!root->left) { Node* rc = root->right; freeN(m, root); o.msg = "Видалено " + to_string(x) + " (1 правий)"; return rc; }
    if (!root->right) { Node* lc = root->left; freeN(m, root); o.msg = "Видалено " + to_string(x) + " (1 лівий)"; return lc; }

    Node* s = minN(root->right, o.comps);
    int sk = s->key;
    root->key = sk;
    ORes tmp;
    root->right = delBst(root->right, sk, m, tmp);
    o.comps += tmp.comps;
    o.msg = "Видалено " + to_string(x) + " (2 нащадки), замінник " + to_string(sk);
    return root;
}

// 7. Перевірка інваріантів і пам'яті
bool checkInv(const Node* r, const Mem& m, string& out) {
    string err;
    bool b = validBst(r, err);
    unordered_set<const Node*> vis;
    auto acyc = [&](auto& self, const Node* n) -> bool {
        if (!n) return true;
        if (!vis.insert(n).second) return false;
        return self(self, n->left) && self(self, n->right);
    };
    bool a = acyc(acyc, r);
    vector<int> v; inorder(r, v);
    bool s = true;
    for (size_t i = 1; i < v.size(); ++i) if (v[i] < v[i - 1]) s = false;
    out = string("BST=") + (b ? "OK" : err) + ", ACYCLIC=" + (a ? "OK" : "FAIL") +
          ", SORTED=" + (s ? "OK" : "FAIL") + ", N=" + to_string(countN(r)) +
          ", alloc=" + to_string(m.alloc) + ", freed=" + to_string(m.freed) + ", active=" + to_string(m.active());
    return b && a && s && (m.alloc >= m.freed);
}

void printIn(const string& lbl, const Node* r) {
    vector<int> v; inorder(r, v);
    cout << lbl << ": ";
    for (size_t i = 0; i < v.size(); ++i) cout << v[i] << (i + 1 < v.size() ? ", " : "\n");
}

// 8. Експеримент із формою дерева
void shapeExp() {
    vector<pair<string, vector<int>>> exp = {
        {"Майже збалансована", {40, 20, 60, 10, 30, 50, 70}},
        {"Вироджена", {10, 20, 30, 40, 50, 60, 70}},
        {"Змішана", {35, 10, 60, 20, 50, 70, 40}}
    };
    cout << "\n--- Експеримент із формою дерева (10..70) ---\n";
    for (const auto& [name, ord] : exp) {
        Mem m; Node* r = nullptr; ORes o;
        for (int k : ord) r = insBst(r, k, m, o);
        cout << name << " | висота=" << height(r)
             << " | пошук 70: " << searchIter(r, 70).comps << " порівнянь"
             << " | пошук 99: " << searchIter(r, 99).comps << " порівнянь\n";
        clearT(r, m); assert(m.active() == 0);
    }
}

// 20 автоматизованих сценаріїв (15 базових + 5 власних)
void run20() {
    auto eq = [](const Node* r, const vector<int>& e) { vector<int> v; inorder(r, v); return v == e; };
    { Mem m; Node* r = nullptr; assert(!searchIter(r, 5).found && eq(r, {})); }
    { Mem m; Node* r = nullptr; ORes o; r = insBst(r, 5, m, o); assert(o.ok && eq(r, {5})); clearT(r, m); }
    { Mem m; Node* r = nullptr; ORes o; r = insBst(r, 5, m, o); r = delBst(r, 5, m, o); assert(!r && m.active() == 0); }
    { Mem m; Node* r = nullptr; ORes o; for (int k : {5, 3, 7}) r = insBst(r, k, m, o); r = insBst(r, 4, m, o); assert(eq(r, {3, 4, 5, 7})); clearT(r, m); }
    { Mem m; Node* r = nullptr; ORes o; for (int k : {5, 3, 7}) r = insBst(r, k, m, o); size_t a = m.alloc; r = insBst(r, 5, m, o); assert(!o.ok && m.alloc == a); clearT(r, m); }
    { Mem m; Node* r = nullptr; ORes o; for (int k : {5, 3, 7}) r = insBst(r, k, m, o); r = delBst(r, 3, m, o); assert(eq(r, {5, 7})); clearT(r, m); }
    { Mem m; Node* r = nullptr; ORes o; for (int k : {5, 3, 7, 6}) r = insBst(r, k, m, o); r = delBst(r, 7, m, o); assert(eq(r, {3, 5, 6})); clearT(r, m); }
    { Mem m; Node* r = nullptr; ORes o; for (int k : {5, 3, 8, 7, 9}) r = insBst(r, k, m, o); r = delBst(r, 8, m, o); assert(eq(r, {3, 5, 7, 9})); clearT(r, m); }
    { Mem m; Node* r = nullptr; ORes o; for (int k : {5, 3, 8, 7, 9}) r = insBst(r, k, m, o); r = delBst(r, 5, m, o); assert(eq(r, {3, 7, 8, 9})); clearT(r, m); }
    { Mem m; Node* r = nullptr; ORes o; for (int k : {5, 3, 7}) r = insBst(r, k, m, o); r = delBst(r, 10, m, o); assert(!o.ok && eq(r, {3, 5, 7})); clearT(r, m); }
    { Mem m; Node* r = nullptr; ORes o; for (int k : {1, 2, 3, 4}) r = insBst(r, k, m, o); assert(searchIter(r, 4).path.size() == 4); clearT(r, m); }
    { Mem m; Node* r = nullptr; ORes o; for (int k : {4, 2, 6, 1, 3, 5, 7}) r = insBst(r, k, m, o); assert(searchIter(r, 7).path.size() == 3); clearT(r, m); }
    { Mem m; Node* r = nullptr; ORes o; for (int k : {5, 3, 7, 6}) r = insBst(r, k, m, o); r = delBst(r, 5, m, o); assert(eq(r, {3, 6, 7})); clearT(r, m); }
    { Mem m; Node* r = nullptr; ORes o; for (int k : {2, 1}) r = insBst(r, k, m, o); r = delBst(r, 2, m, o); assert(eq(r, {1})); clearT(r, m); }
    { Mem m; Node* r = nullptr; ORes o; for (int k : {2, 3}) r = insBst(r, k, m, o); r = delBst(r, 2, m, o); assert(eq(r, {3})); clearT(r, m); }
    // 5 власних сценаріїв
    { Mem m; Node* r = nullptr; ORes o; for (int k : {50, 30, 70}) r = insBst(r, k, m, o); r = delBst(r, 30, m, o); r = delBst(r, 30, m, o); assert(!o.ok); clearT(r, m); }
    { Mem m; Node* r = nullptr; ORes o; for (int k : {50, 30, 80, 70, 90, 75}) r = insBst(r, k, m, o); r = delBst(r, 50, m, o); string e; assert(validBst(r, e) && eq(r, {30, 70, 75, 80, 90})); clearT(r, m); }
    { Mem m; Node* r = nullptr; ORes o; for (int k : {40, 20, 60, 10, 30, 50, 70}) r = insBst(r, k, m, o); r = delBst(r, 40, m, o); r = delBst(r, 50, m, o); r = delBst(r, 60, m, o); assert(eq(r, {10, 20, 30, 70})); clearT(r, m); }
    { Mem m; Node* r = nullptr; ORes o; for (int k : {50, 30, 70}) r = insBst(r, k, m, o); clearT(r, m); assert(!r && m.active() == 0); r = insBst(r, 100, m, o); assert(eq(r, {100})); clearT(r, m); }
    { Mem m; Node* r = makeN(m, 50); r->left = makeN(m, 30); r->left->right = makeN(m, 65); string e; assert(!validBst(r, e)); clearT(r, m); }
    cout << "Пройдено сценаріїв: 20 / 20\n";
}

int main() {
    setlocale(LC_ALL, "");
    run20();

    Mem mem; Node* root = nullptr; ORes op; string inv;
    for (int k : {50, 30, 70, 20, 40, 60, 80}) root = insBst(root, k, mem, op);
    printIn("\nПочатковий симетричний", root);
    checkInv(root, mem, inv); cout << inv << "\n";

    auto sr = searchIter(root, 60);
    assert(sr.found && sr.path == searchRec(root, 60).path);
    cout << "Пошук 60: знайдено=" << sr.found << ", порівнянь=" << sr.comps << "\n";
    printIn("Після пошуку 60", root);
    checkInv(root, mem, inv); cout << inv << "\n";

    root = insBst(root, 65, mem, op);
    cout << "Вставлення 65: " << op.msg << ", порівнянь=" << op.comps << "\n";
    printIn("Після вставлення 65", root);
    checkInv(root, mem, inv); cout << inv << "\n";

    root = delBst(root, 70, mem, op);
    cout << "Видалення 70: " << op.msg << ", порівнянь=" << op.comps << "\n";
    printIn("Після видалення 70", root);
    checkInv(root, mem, inv); cout << inv << "\n";

    shapeExp();
    clearT(root, mem);
    cout << "\nАктивних вузлів після очищення: " << mem.active() << "\n";
    return 0;
}
