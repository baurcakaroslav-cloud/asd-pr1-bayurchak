#include <iostream>
#include <vector>
#include <string>
#include <algorithm>
#include <cassert>

using namespace std;

struct St { long long c = 0, w = 0; };

void insSort(int* a, int n, St& s) {
    for (int i = 1; i < n; ++i) {
        int k = a[i]; ++s.w; int j = i - 1;
        while (j >= 0 && (++s.c, a[j] > k)) { a[j + 1] = a[j]; ++s.w; --j; }
        a[j + 1] = k; ++s.w;
    }
}

void bubSort(int* a, int n, St& s) {
    for (int i = 0; i < n - 1; ++i) {
        bool ok = false;
        for (int j = 0; j + 1 < n - i; ++j)
            if (++s.c, a[j] > a[j + 1]) { swap(a[j], a[j + 1]); s.w += 2; ok = true; }
        if (!ok) break;
    }
}

int linFind(const int* a, int n, int x, long long& c) {
    c = 0;
    for (int i = 0; i < n; ++i) if (++c, a[i] == x) return i;
    return -1;
}

int binFind(const int* a, int n, int x, long long& c) {
    c = 0; int l = 0, r = n - 1;
    while (l <= r) {
        int m = l + (r - l) / 2;
        if (++c, a[m] == x) return m;
        if (++c, a[m] < x) l = m + 1; else r = m - 1;
    }
    return -1;
}

vector<int> naiveSub(const string& t, const string& p, long long& c) {
    c = 0; vector<int> ans;
    if (p.empty() || p.size() > t.size()) return ans;
    for (size_t i = 0; i + p.size() <= t.size(); ++i) {
        size_t j = 0;
        while (j < p.size() && (++c, t[i + j] == p[j])) ++j;
        if (j == p.size()) ans.push_back((int)i);
    }
    return ans;
}

vector<int> kmpSub(const string& t, const string& p, long long& pc, long long& sc) {
    pc = sc = 0; vector<int> ans;
    if (p.empty() || p.size() > t.size()) return ans;
    vector<int> pi(p.size(), 0);
    for (size_t i = 1, k = 0; i < p.size(); ++i) {
        while (k > 0 && (++pc, p[i] != p[k])) k = pi[k - 1];
        if (++pc, p[i] == p[k]) ++k;
        pi[i] = (int)k;
    }
    for (size_t i = 0, q = 0; i < t.size(); ++i) {
        while (q > 0 && (++sc, t[i] != p[q])) q = pi[q - 1];
        if (++sc, t[i] == p[q]) ++q;
        if (q == p.size()) { ans.push_back((int)(i + 1 - q)); q = pi[q - 1]; }
    }
    return ans;
}

struct LNode { int v; LNode* next = nullptr; explicit LNode(int x) : v(x) {} };

class List {
    LNode *head = nullptr, *tail = nullptr; int n = 0;
public:
    ~List() { clear(); }
    void pushFront(int x) { LNode* p = new LNode(x); p->next = head; head = p; if (!tail) tail = p; ++n; }
    void pushBack(int x) { LNode* p = new LNode(x); if (!head) head = tail = p; else tail = tail->next = p; ++n; }
    LNode* find(int x) const { for (LNode* c = head; c; c = c->next) if (c->v == x) return c; return nullptr; }
    bool insAfter(int k, int x) {
        LNode* p = find(k); if (!p) return false;
        LNode* q = new LNode(x); q->next = p->next; p->next = q; if (tail == p) tail = q; ++n; return true;
    }
    int delAll(int x) {
        int r = 0;
        while (head && head->v == x) { LNode* t = head; head = head->next; delete t; --n; ++r; }
        if (!head) { tail = nullptr; return r; }
        for (LNode* c = head; c->next; )
            if (c->next->v == x) { LNode* t = c->next; c->next = t->next; if (t == tail) tail = c; delete t; --n; ++r; }
            else c = c->next;
        return r;
    }
    void clear() { while (head) { LNode* t = head; head = head->next; delete t; } tail = nullptr; n = 0; }
    int size() const { return n; }
    void show(const string& m) const {
        cout << m << " [" << n << "]: ";
        for (LNode* c = head; c; c = c->next) cout << c->v << (c->next ? " -> " : "");
        cout << "\n";
    }
};

struct TNode { int k; TNode *l = nullptr, *r = nullptr; explicit TNode(int x) : k(x) {} };

class BST {
    TNode* root = nullptr;
    TNode* ins(TNode* p, int x, bool& ok) {
        if (!p) { ok = true; return new TNode(x); }
        if (x < p->k) p->l = ins(p->l, x, ok); else if (x > p->k) p->r = ins(p->r, x, ok);
        return p;
    }
    TNode* minN(TNode* p) const { while (p && p->l) p = p->l; return p; }
    TNode* del(TNode* p, int x, bool& ok) {
        if (!p) return nullptr;
        if (x < p->k) p->l = del(p->l, x, ok);
        else if (x > p->k) p->r = del(p->r, x, ok);
        else {
            ok = true;
            if (!p->l || !p->r) { TNode* ch = p->l ? p->l : p->r; delete p; return ch; }
            TNode* s = minN(p->r); p->k = s->k; bool d = false; p->r = del(p->r, s->k, d);
        }
        return p;
    }
    void ord(TNode* p, int mode, vector<int>& v) const {
        if (!p) return;
        if (mode == 1) v.push_back(p->k);
        ord(p->l, mode, v);
        if (mode == 0) v.push_back(p->k);
        ord(p->r, mode, v);
        if (mode == 2) v.push_back(p->k);
    }
    int h(TNode* p) const { return p ? 1 + max(h(p->l), h(p->r)) : 0; }
    int inter(TNode* p) const { return (!p || (!p->l && !p->r)) ? 0 : 1 + inter(p->l) + inter(p->r); }
    void clr(TNode* p) { if (p) { clr(p->l); clr(p->r); delete p; } }
public:
    ~BST() { clear(); }
    bool insert(int x) { bool ok = false; root = ins(root, x, ok); return ok; }
    bool erase(int x) { bool ok = false; root = del(root, x, ok); return ok; }
    bool find(int x, long long& c) const {
        c = 0;
        for (TNode* p = root; p; ) {
            if (++c, x == p->k) return true;
            p = (++c, x < p->k) ? p->l : p->r;
        }
        return false;
    }
    int height() const { return h(root); }
    int internal() const { return inter(root); }
    void clear() { clr(root); root = nullptr; }
    void showOrd(const string& lbl, int m) const {
        vector<int> v; ord(root, m, v);
        cout << lbl << ": ";
        for (size_t i = 0; i < v.size(); ++i) cout << v[i] << (i + 1 < v.size() ? ", " : "\n");
    }
};

void checkAll() {
    long long c = 0, pc = 0, sc = 0; St st;
    int e[1] = {0}; assert(linFind(e, 0, 1, c) == -1 && binFind(e, 0, 1, c) == -1);
    int o[1] = {-5}; insSort(o, 1, st); assert(o[0] == -5 && linFind(o, 1, -5, c) == 0);
    int d[5] = {2, -3, 2, 0, -3}; insSort(d, 5, st);
    for (int i = 1; i < 5; ++i) assert(d[i - 1] <= d[i]);
    assert(naiveSub("ABABA", "ABA", c) == vector<int>({0, 2}));
    assert(kmpSub("ABABA", "ABA", pc, sc) == vector<int>({0, 2}));
    assert(naiveSub("ABC", "ABCD", c).empty() && naiveSub("", "A", c).empty() && naiveSub("ABC", "", c).empty());
    List l; assert(!l.insAfter(1, 2) && l.delAll(1) == 0);
    l.pushFront(5); l.pushBack(5); assert(l.insAfter(5, 7) && l.delAll(5) == 2 && l.size() == 1);
    BST b; assert(b
