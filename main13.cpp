#include <iostream>
#include <iomanip>
#include <string>
#include <vector>
#include <unordered_set>
#include <cassert>

using namespace std;

struct Node {
    string fullName;
    string group;
    double rating = 0.0;
    Node* prev = nullptr;
    Node* next = nullptr;

    Node() = default;
    Node(const string& name, const string& grp, double rt, Node* p = nullptr, Node* n = nullptr)
        : fullName(name), group(grp), rating(rt), prev(p), next(n) {}
};

struct MemoryTracker {
    size_t allocated = 0;
    size_t freed = 0;

    size_t active() const {
        return (allocated >= freed) ? (allocated - freed) : 0;
    }

    string status() const {
        return "виділено=" + to_string(allocated) +
               ", звільнено=" + to_string(freed) +
               ", активних=" + to_string(active());
    }
};

Node* makeNode(MemoryTracker& mem, const string& name, const string& grp, double rating,
               Node* prev = nullptr, Node* next = nullptr) {
    Node* node = new Node(name, grp, rating, prev, next);
    mem.allocated++;
    return node;
}

void destroyNode(MemoryTracker& mem, Node* node) {
    if (node != nullptr) {
        delete node;
        mem.freed++;
    }
}

struct DoublyLinkedList {
    Node* head = nullptr;
    Node* tail = nullptr;
    size_t count = 0;
    MemoryTracker mem;
};

vector<double> forwardValues(const DoublyLinkedList& list) {
    vector<double> res;
    const Node* curr = list.head;
    size_t steps = 0;
    while (curr != nullptr && steps <= list.count + 5) {
        res.push_back(curr->rating);
        curr = curr->next;
        steps++;
    }
    return res;
}

vector<double> backwardValues(const DoublyLinkedList& list) {
    vector<double> res;
    const Node* curr = list.tail;
    size_t steps = 0;
    while (curr != nullptr && steps <= list.count + 5) {
        res.push_back(curr->rating);
        curr = curr->prev;
        steps++;
    }
    return res;
}

bool insertDoublyAt(DoublyLinkedList& list, size_t pos,
                    const string& name, const string& grp, double rating) {
    if (pos > list.count) return false;

    if (list.count == 0) {
        Node* n = makeNode(list.mem, name, grp, rating, nullptr, nullptr);
        list.head = n;
        list.tail = n;
        list.count = 1;
        return true;
    }

    if (pos == 0) {
        Node* n = makeNode(list.mem, name, grp, rating, nullptr, list.head);
        list.head->prev = n;
        list.head = n;
        list.count++;
        return true;
    }

    if (pos == list.count) {
        Node* n = makeNode(list.mem, name, grp, rating, list.tail, nullptr);
        list.tail->next = n;
        list.tail = n;
        list.count++;
        return true;
    }

    Node* curr = list.head;
    for (size_t i = 0; i < pos; ++i) {
        curr = curr->next;
    }
    Node* prevNode = curr->prev;
    Node* n = makeNode(list.mem, name, grp, rating, prevNode, curr);
    prevNode->next = n;
    curr->prev = n;
    list.count++;
    return true;
}

bool removeDoublyAt(DoublyLinkedList& list, size_t pos) {
    if (list.head == nullptr || pos >= list.count) return false;

    Node* target = list.head;
    for (size_t i = 0; i < pos; ++i) {
        target = target->next;
    }

    Node* left = target->prev;
    Node* right = target->next;

    if (left != nullptr) {
        left->next = right;
    } else {
        list.head = right;
    }

    if (right != nullptr) {
        right->prev = left;
    } else {
        list.tail = left;
    }

    if (list.head == nullptr || list.tail == nullptr) {
        list.head = nullptr;
        list.tail = nullptr;
    }

    destroyNode(list.mem, target);
    list.count--;
    return true;
}

bool removeDoublyByKey(DoublyLinkedList& list, double key) {
    Node* curr = list.head;
    size_t idx = 0;
    while (curr != nullptr) {
        if (curr->rating == key) {
            return removeDoublyAt(list, idx);
        }
        curr = curr->next;
        idx++;
    }
    return false;
}

string checkDoublyInvariants(const DoublyLinkedList& list) {
    if (list.count == 0) {
        if (list.head != nullptr || list.tail != nullptr) {
            return "ПОМИЛКА: порожній список має ненульові head/tail";
        }
        return "OK (порожній двонапрямлений список; " + list.mem.status() + ")";
    }

    if (list.head == nullptr || list.tail == nullptr) {
        return "ПОМИЛКА: відсутній head або tail";
    }
    if (list.head->prev != nullptr) {
        return "ПОМИЛКА: head->prev не дорівнює nullptr";
    }
    if (list.tail->next != nullptr) {
        return "ПОМИЛКА: tail->next не дорівнює nullptr";
    }

    unordered_set<const Node*> seen;
    const Node* curr = list.head;
    size_t fwdCount = 0;
    while (curr != nullptr) {
        if (seen.count(curr)) {
            return "ПОМИЛКА: виявлено цикл у прямому обході";
        }
        if (fwdCount > list.count) {
            return "ПОМИЛКА: розрив або перевищення count у прямому обході";
        }
        seen.insert(curr);
        if (curr->next != nullptr && curr->next->prev != curr) {
            return "ПОМИЛКА: порушено зворотний зв'язок next->prev";
        }
        if (curr->next == nullptr && curr != list.tail) {
            return "ПОМИЛКА: останній досяжний вузол не збігається з tail";
        }
        curr = curr->next;
        fwdCount++;
    }

    if (fwdCount != list.count) {
        return "ПОМИЛКА: пряма довжина (" + to_string(fwdCount) + ") != count (" + to_string(list.count) + ")";
    }

    const Node* back = list.tail;
    size_t bwdCount = 0;
    while (back != nullptr) {
        if (bwdCount > list.count) {
            return "ПОМИЛКА: розрив або перевищення count у зворотному обході";
        }
        if (back->prev != nullptr && back->prev->next != back) {
            return "ПОМИЛКА: порушено прямий зв'язок prev->next";
        }
        back = back->prev;
        bwdCount++;
    }

    if (bwdCount != list.count) {
        return "ПОМИЛКА: зворотна довжина (" + to_string(bwdCount) + ") != count (" + to_string(list.count) + ")";
    }

    return "OK (вузлів=" + to_string(list.count) + ", прямі й зворотні зв'язки узгоджені; " +
           list.mem.status() + ")";
}

void clearDoubly(DoublyLinkedList& list) {
    Node* curr = list.head;
    while (curr != nullptr) {
        Node* nxt = curr->next;
        destroyNode(list.mem, curr);
        curr = nxt;
    }
    list.head = nullptr;
    list.tail = nullptr;
    list.count = 0;
}

struct CircularSinglyList {
    Node* head = nullptr;
    Node* tail = nullptr;
    size_t count = 0;
    MemoryTracker mem;
};

bool appendCircularSingly(CircularSinglyList& list, const string& name, const string& grp, double rating) {
    Node* n = makeNode(list.mem, name, grp, rating, nullptr, nullptr);
    if (list.head == nullptr) {
        n->next = n;
        list.head = n;
        list.tail = n;
        list.count = 1;
        return true;
    }
    n->next = list.head;
    list.tail->next = n;
    list.tail = n;
    list.count++;
    return true;
}

struct StepWalkResult {
    vector<double> values;
    const Node* endNode = nullptr;
    bool returnedToStartAfterN = false;
};

StepWalkResult walkCircularSinglySteps(const CircularSinglyList& list, size_t steps) {
    StepWalkResult out;
    if (list.head == nullptr || steps == 0) {
        out.endNode = list.head;
        out.returnedToStartAfterN = (list.head == nullptr && steps == 0);
        return out;
    }

    const Node* curr = list.head;
    for (size_t i = 0; i < steps; ++i) {
        out.values.push_back(curr->rating);
        curr = curr->next;
    }
    out.endNode = curr;
    out.returnedToStartAfterN = (steps == list.count && curr == list.head);
    return out;
}

string checkCircularSinglyInvariants(const CircularSinglyList& list) {
    if (list.count == 0) {
        if (list.head != nullptr || list.tail != nullptr) {
            return "ПОМИЛКА: порожній циклічний список має ненульові покажчики";
        }
        return "OK (порожній циклічний однонапрямлений список; " + list.mem.status() + ")";
    }

    if (list.head == nullptr || list.tail == nullptr) {
        return "ПОМИЛКА: відсутній head або tail";
    }
    if (list.tail->next != list.head) {
        return "ПОМИЛКА: цикл не замкнено (tail->next != head)";
    }

    const Node* curr = list.head;
    for (size_t i = 0; i < list.count; ++i) {
        if (curr == nullptr) {
            return "ПОМИЛКА: розрив ланцюжка до завершення count кроків";
        }
        curr = curr->next;
    }

    if (curr != list.head) {
        return "ПОМИЛКА: довжина циклу не відповідає count";
    }

    return "OK (цикл замкнено за " + to_string(list.count) + " кроків, tail->next == head; " +
           list.mem.status() + ")";
}

void clearCircularSingly(CircularSinglyList& list) {
    if (list.head == nullptr) {
        list.tail = nullptr;
        list.count = 0;
        return;
    }
    list.tail->next = nullptr;
    Node* curr = list.head;
    while (curr != nullptr) {
        Node* nxt = curr->next;
        destroyNode(list.mem, curr);
        curr = nxt;
    }
    list.head = nullptr;
    list.tail = nullptr;
    list.count = 0;
}

struct CircularDoublyList {
    Node* head = nullptr;
    Node* tail = nullptr;
    size_t count = 0;
    MemoryTracker mem;
};

bool appendCircularDoubly(CircularDoublyList& list, const string& name, const string& grp, double rating) {
    Node* n = makeNode(list.mem, name, grp, rating, nullptr, nullptr);
    if (list.head == nullptr) {
        n->next = n;
        n->prev = n;
        list.head = n;
        list.tail = n;
        list.count = 1;
        return true;
    }
    n->prev = list.tail;
    n->next = list.head;
    list.tail->next = n;
    list.head->prev = n;
    list.tail = n;
    list.count++;
    return true;
}

StepWalkResult walkCircularDoublyForwardOneCycle(const CircularDoublyList& list) {
    StepWalkResult out;
    if (list.head == nullptr) return out;
    const Node* curr = list.head;
    for (size_t i = 0; i < list.count; ++i) {
        out.values.push_back(curr->rating);
        curr = curr->next;
    }
    out.endNode = curr;
    out.returnedToStartAfterN = (curr == list.head);
    return out;
}

StepWalkResult walkCircularDoublyBackwardOneCycle(const CircularDoublyList& list) {
    StepWalkResult out;
    if (list.tail == nullptr) return out;
    const Node* curr = list.tail;
    for (size_t i = 0; i < list.count; ++i) {
        out.values.push_back(curr->rating);
        curr = curr->prev;
    }
    out.endNode = curr;
    out.returnedToStartAfterN = (curr == list.tail);
    return out;
}

string checkCircularDoublyInvariants(const CircularDoublyList& list) {
    if (list.count == 0) {
        if (list.head != nullptr || list.tail != nullptr) {
            return "ПОМИЛКА: порожній циклічний двонапрямлений список має ненульові покажчики";
        }
        return "OK (порожній циклічний двонапрямлений список; " + list.mem.status() + ")";
    }
    if (list.head == nullptr || list.tail == nullptr) {
        return "ПОМИЛКА: відсутній head або tail";
    }
    if (list.tail->next != list.head || list.head->prev != list.tail) {
        return "ПОМИЛКА: порушено кільцеве замикання між head і tail";
    }

    const Node* curr = list.head;
    for (size_t i = 0; i < list.count; ++i) {
        if (curr == nullptr || curr->next == nullptr || curr->next->prev != curr) {
            return "ПОМИЛКА: порушено взаємність next/prev у циклічному двонапрямленому списку";
        }
        curr = curr->next;
    }
    if (curr != list.head) {
        return "ПОМИЛКА: прямий цикл не повернувся до head за count кроків";
    }

    const Node* back = list.tail;
    for (size_t i = 0; i < list.count; ++i) {
        if (back == nullptr || back->prev == nullptr || back->prev->next != back) {
            return "ПОМИЛКА: порушено взаємність prev/next у зворотному циклі";
        }
        back = back->prev;
    }
    if (back != list.tail) {
        return "ПОМИЛКА: зворотний цикл не повернувся до tail за count кроків";
    }

    return "OK (обидва кільця коректні, довжина=" + to_string(list.count) + "; " +
           list.mem.status() + ")";
}

void clearCircularDoubly(CircularDoublyList& list) {
    if (list.head == nullptr) {
        list.tail = nullptr;
        list.count = 0;
        return;
    }
    list.tail->next = nullptr;
    list.head->prev = nullptr;
    Node* curr = list.head;
    while (curr != nullptr) {
        Node* nxt = curr->next;
        destroyNode(list.mem, curr);
        curr = nxt;
    }
    list.head = nullptr;
    list.tail = nullptr;
    list.count = 0;
}

void printVec(const string& label, const vector<double>& v) {
    cout << label << ": [";
    for (size_t i = 0; i < v.size(); ++i) {
        cout << v[i] << (i + 1 < v.size() ? ", " : "");
    }
    cout << "]\n";
}

void runAutomatedTests() {
    int passed = 0;

    { DoublyLinkedList d; assert(forwardValues(d).empty() && backwardValues(d).empty()); clearDoubly(d); passed++; }
    { DoublyLinkedList d; insertDoublyAt(d, 0, "С", "34-ІП", 5.0); assert((forwardValues(d) == vector<double>{5.0}) && (backwardValues(d) == vector<double>{5.0})); clearDoubly(d); passed++; }
    { DoublyLinkedList d; for (double x : {1.0, 2.0, 3.0}) insertDoublyAt(d, d.count, "С", "34-ІП", x); assert((backwardValues(d) == vector<double>{3.0, 2.0, 1.0})); clearDoubly(d); passed++; }
    { DoublyLinkedList d; insertDoublyAt(d, 0, "С", "34-ІП", 1.0); insertDoublyAt(d, 1, "С", "34-ІП", 3.0); insertDoublyAt(d, 1, "С", "34-ІП", 2.0); assert((forwardValues(d) == vector<double>{1.0, 2.0, 3.0}) && (backwardValues(d) == vector<double>{3.0, 2.0, 1.0})); clearDoubly(d); passed++; }
    { DoublyLinkedList d; for (double x : {1.0, 2.0, 3.0}) insertDoublyAt(d, d.count, "С", "34-ІП", x); removeDoublyAt(d, 1); assert((forwardValues(d) == vector<double>{1.0, 3.0}) && (backwardValues(d) == vector<double>{3.0, 1.0})); clearDoubly(d); passed++; }
    { DoublyLinkedList d; insertDoublyAt(d, 0, "С", "34-ІП", 7.0); removeDoublyAt(d, 0); assert(forwardValues(d).empty() && backwardValues(d).empty()); clearDoubly(d); passed++; }
    { CircularSinglyList c; appendCircularSingly(c, "С", "34-ІП", 4.0); StepWalkResult w = walkCircularSinglySteps(c, 4); assert(w.returnedToStartAfterN && w.endNode == c.head); clearCircularSingly(c); passed++; }
    { CircularSinglyList c; for (double x : {1.0, 2.0, 3.0}) appendCircularSingly(c, "С", "34-ІП", x); StepWalkResult w = walkCircularSinglySteps(c, 5); assert((w.values == vector<double>{1.0, 2.0, 3.0, 1.0, 2.0})); clearCircularSingly(c); passed++; }
    { CircularSinglyList c; for (double x : {1.0, 2.0, 3.0}) appendCircularSingly(c, "С", "34-ІП", x); assert(checkCircularSinglyInvariants(c).rfind("OK", 0) == 0); clearCircularSingly(c); passed++; }
    { CircularDoublyList cd; for (double x : {1.0, 2.0, 3.0}) appendCircularDoubly(cd, "С", "34-ІП", x); StepWalkResult w = walkCircularDoublyForwardOneCycle(cd); assert((w.values == vector<double>{1.0, 2.0, 3.0}) && w.returnedToStartAfterN); clearCircularDoubly(cd); passed++; }
    { CircularDoublyList cd; for (double x : {1.0, 2.0, 3.0}) appendCircularDoubly(cd, "С", "34-ІП", x); StepWalkResult w = walkCircularDoublyBackwardOneCycle(cd); assert((w.values == vector<double>{3.0, 2.0, 1.0}) && w.returnedToStartAfterN); clearCircularDoubly(cd); passed++; }
    { CircularDoublyList cd; appendCircularDoubly(cd, "С", "34-ІП", 1.0); assert(cd.head->next == cd.head && cd.head->prev == cd.head); clearCircularDoubly(cd); passed++; }
    {
        DoublyLinkedList d; CircularSinglyList c; CircularDoublyList cd;
        for (int i = 0; i < 10; ++i) {
            insertDoublyAt(d, d.count, "С", "34-ІП", i);
            appendCircularSingly(c, "С", "34-ІП", i);
            appendCircularDoubly(cd, "С", "34-ІП", i);
        }
        clearDoubly(d); clearCircularSingly(c); clearCircularDoubly(cd);
        assert(d.mem.active() == 0 && c.mem.active() == 0 && cd.mem.active() == 0);
        passed++;
    }
    {
        DoublyLinkedList d; insertDoublyAt(d, 0, "С", "34-ІП", 10.0); clearDoubly(d);
        insertDoublyAt(d, 0, "С", "34-ІП", 20.0); assert((forwardValues(d) == vector<double>{20.0})); clearDoubly(d); passed++;
    }
    {
        CircularSinglyList c; for (double x : {1.0, 2.0, 3.0}) appendCircularSingly(c, "С", "34-ІП", x);
        StepWalkResult w = walkCircularSinglySteps(c, 100);
        assert(w.values.size() == 100 && w.endNode != nullptr);
        clearCircularSingly(c); passed++;
    }
    {
        DoublyLinkedList d; insertDoublyAt(d, 0, "С", "34-ІП", 2.0); insertDoublyAt(d, 0, "С", "34-ІП", 1.0); insertDoublyAt(d, d.count, "С", "34-ІП", 3.0);
        assert((forwardValues(d) == vector<double>{1.0, 2.0, 3.0}) && (backwardValues(d) == vector<double>{3.0, 2.0, 1.0}));
        clearDoubly(d); passed++;
    }
    {
        DoublyLinkedList d; for (double x : {1.0, 2.0, 3.0}) insertDoublyAt(d, d.count, "С", "34-ІП", x);
        removeDoublyAt(d, 0); removeDoublyAt(d, d.count - 1);
        assert((forwardValues(d) == vector<double>{2.0}) && (backwardValues(d) == vector<double>{2.0}));
        clearDoubly(d); passed++;
    }
    {
        DoublyLinkedList d; insertDoublyAt(d, 0, "С", "34-ІП", 5.0); clearDoubly(d); clearDoubly(d);
        assert(d.mem.active() == 0); passed++;
    }
    {
        DoublyLinkedList d; insertDoublyAt(d, 0, "С", "34-ІП", 1.0); insertDoublyAt(d, 1, "С", "34-ІП", 2.0);
        d.tail->prev = nullptr;
        string diag = checkDoublyInvariants(d);
        assert(diag.rfind("ПОМИЛКА", 0) == 0);
        d.tail->prev = d.head;
        clearDoubly(d); passed++;
    }
    {
        DoublyLinkedList d; for (double x : {4.0, 9.0, 2.0}) insertDoublyAt(d, d.count, "С", "34-ІП", x);
        assert(removeDoublyByKey(d, 9.0) && (forwardValues(d) == vector<double>{4.0, 2.0}));
        clearDoubly(d); passed++;
    }

    cout << "Пройдено автоматизованих сценаріїв (ПР № 13): " << passed << " / 20\n";
}

int main() {
    setlocale(LC_ALL, "");
    runAutomatedTests();

    DoublyLinkedList dbl;
    vector<double> init = {4.0, 9.0, 2.0, 7.0};
    for (size_t i = 0; i < init.size(); ++i) {
        insertDoublyAt(dbl, i, "Студент_" + to_string(i + 1), "34-ІП", init[i]);
    }
    insertDoublyAt(dbl, 2, "Вставлений_5", "34-ІП", 5.0);

    cout << "\n--- Варіант № 1: Двонапрямлений список після вставки 5 на позицію 2 ---\n";
    printVec("Прямий обхід", forwardValues(dbl));
    printVec("Зворотний обхід", backwardValues(dbl));
    cout << "Інваріанти: " << checkDoublyInvariants(dbl) << "\n";

    CircularSinglyList csl;
    for (double v : forwardValues(dbl)) {
        appendCircularSingly(csl, "Студент", "34-ІП", v);
    }
    StepWalkResult walk7 = walkCircularSinglySteps(csl, 7);
    cout << "\n--- Циклічний однонапрямлений список (7 кроків) ---\n";
    printVec("7 кроків обходу", walk7.values);
    cout << "Інваріанти: " << checkCircularSinglyInvariants(csl) << "\n";

    CircularDoublyList cdl;
    for (double v : forwardValues(dbl)) {
        appendCircularDoubly(cdl, "Студент", "34-ІП", v);
    }
    cout << "\n--- Циклічний двонапрямлений список ---\n";
    printVec("Прямий цикл", walkCircularDoublyForwardOneCycle(cdl).values);
    printVec("Зворотний цикл", walkCircularDoublyBackwardOneCycle(cdl).values);
    cout << "Інваріанти: " << checkCircularDoublyInvariants(cdl) << "\n";

    clearDoubly(dbl);
    clearCircularSingly(csl);
    clearCircularDoubly(cdl);

    cout << "\n--- Після повного очищення ---\n";
    cout << "Двонапрямлений: " << checkDoublyInvariants(dbl) << "\n";
    cout << "Циклічний однонапрямлений: " << checkCircularSinglyInvariants(csl) << "\n";
    cout << "Циклічний двонапрямлений: " << checkCircularDoublyInvariants(cdl) << "\n";

    return 0;
}
