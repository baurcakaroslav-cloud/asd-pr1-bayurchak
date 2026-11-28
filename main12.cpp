#include <iostream>
#include <iomanip>
#include <string>
#include <vector>
#include <unordered_set>
#include <cassert>

using namespace std;

struct StudentNode {
    string fullName;
    string group;
    double rating = 0.0;
    StudentNode* next = nullptr;

    StudentNode() = default;
    StudentNode(const string& name, const string& grp, double rt, StudentNode* nxt = nullptr)
        : fullName(name), group(grp), rating(rt), next(nxt) {}
};

struct CommandLogEntry {
    string commandName;
    string params;
    string resultSummary;
    string invariantsStatus;
    string memoryStatus;
};

struct SinglyLinkedList {
    StudentNode* head = nullptr;
    StudentNode* tail = nullptr;
    size_t count = 0;

    size_t createdNodes = 0;
    size_t freedNodes = 0;
    vector<CommandLogEntry> log;
};

struct SearchMatch {
    size_t position = 0;
    const StudentNode* node = nullptr;
};

struct SearchResult {
    bool found = false;
    size_t inspectedNodes = 0;
    vector<SearchMatch> matches;
};

struct OperationReport {
    bool success = false;
    string dataStatus;
    string structureStatus;
    string memoryStatus;
    size_t inspectedNodes = 0;
};

StudentNode* allocateNode(SinglyLinkedList& list, const string& name, const string& grp, double rating, StudentNode* nxt = nullptr) {
    StudentNode* node = new StudentNode(name, grp, rating, nxt);
    list.createdNodes++;
    return node;
}

void releaseNode(SinglyLinkedList& list, StudentNode* node) {
    if (node != nullptr) {
        delete node;
        list.freedNodes++;
    }
}

void clearList(SinglyLinkedList& list) {
    StudentNode* curr = list.head;
    while (curr != nullptr) {
        StudentNode* nextAddr = curr->next;
        releaseNode(list, curr);
        curr = nextAddr;
    }
    list.head = nullptr;
    list.tail = nullptr;
    list.count = 0;
}

vector<double> snapshotKeys(const SinglyLinkedList& list) {
    vector<double> keys;
    const StudentNode* curr = list.head;
    size_t guard = 0;
    const size_t limit = list.count + 20;
    while (curr != nullptr && guard < limit) {
        keys.push_back(curr->rating);
        curr = curr->next;
        guard++;
    }
    return keys;
}

string memoryStatusText(const SinglyLinkedList& list) {
    size_t active = (list.createdNodes >= list.freedNodes) ? (list.createdNodes - list.freedNodes) : 0;
    return "створено=" + to_string(list.createdNodes) +
           ", звільнено=" + to_string(list.freedNodes) +
           ", активних=" + to_string(active);
}

string checkInvariants(const SinglyLinkedList& list, const vector<double>* expectedKeysIfUnchanged = nullptr) {
    if (list.count == 0) {
        if (list.head != nullptr || list.tail != nullptr) {
            return "ПОМИЛКА: count == 0, але head або tail не nullptr";
        }
        if (expectedKeysIfUnchanged != nullptr && !expectedKeysIfUnchanged->empty()) {
            return "ПОМИЛКА: список не залишився незмінним після невдалої операції";
        }
        return "OK (порожній список узгоджений; активних вузлів: " +
               to_string(list.createdNodes - list.freedNodes) + ")";
    }

    if (list.head == nullptr || list.tail == nullptr) {
        return "ПОМИЛКА: ненульовий count, але head або tail порожній";
    }

    unordered_set<const StudentNode*> visited;
    const StudentNode* curr = list.head;
    const StudentNode* lastVisited = nullptr;
    size_t reachable = 0;
    const size_t maxSteps = list.count + 50;

    while (curr != nullptr) {
        if (visited.find(curr) != visited.end()) {
            return "ПОМИЛКА: виявлено цикл у зв'язках списку";
        }
        if (reachable >= maxSteps) {
            return "ПОМИЛКА: перевищено обмеження кроків (можливий цикл або розрив)";
        }
        visited.insert(curr);
        lastVisited = curr;
        reachable++;
        curr = curr->next;
    }

    if (reachable != list.count) {
        return "ПОМИЛКА: кількість досяжних вузлів (" + to_string(reachable) +
               ") не збігається з count (" + to_string(list.count) + ")";
    }

    if (lastVisited != list.tail || list.tail->next != nullptr) {
        return "ПОМИЛКА: некоректний покажчик кінця (tail) або tail->next != nullptr";
    }

    if (expectedKeysIfUnchanged != nullptr) {
        vector<double> currentKeys = snapshotKeys(list);
        if (currentKeys != *expectedKeysIfUnchanged) {
            return "ПОМИЛКА: послідовність елементів змінилася при невдалій операції";
        }
    }

    return "OK (вузлів: " + to_string(reachable) +
           ", tail вірний, циклів немає, активних вузлів: " +
           to_string(list.createdNodes - list.freedNodes) + ")";
}

void recordLog(SinglyLinkedList& list, const string& cmd, const string& params,
               const string& summary, const string& invStatus) {
    list.log.push_back({cmd, params, summary, invStatus, memoryStatusText(list)});
}

SearchResult findFirstByRating(const SinglyLinkedList& list, double targetRating) {
    SearchResult res;
    const StudentNode* curr = list.head;
    size_t pos = 0;

    while (curr != nullptr) {
        res.inspectedNodes++;
        if (curr->rating == targetRating) {
            res.found = true;
            res.matches.push_back({pos, curr});
            return res;
        }
        curr = curr->next;
        pos++;
    }
    return res;
}

SearchResult findAllByRating(const SinglyLinkedList& list, double targetRating) {
    SearchResult res;
    const StudentNode* curr = list.head;
    size_t pos = 0;

    while (curr != nullptr) {
        res.inspectedNodes++;
        if (curr->rating == targetRating) {
            res.found = true;
            res.matches.push_back({pos, curr});
        }
        curr = curr->next;
        pos++;
    }
    return res;
}

OperationReport insertAtPosition(SinglyLinkedList& list, size_t pos,
                                 const string& name, const string& grp, double rating) {
    vector<double> before = snapshotKeys(list);
    OperationReport rep;

    if (pos > list.count) {
        rep.success = false;
        rep.dataStatus = "Відмова: позиція " + to_string(pos) + " поза межами [0.." + to_string(list.count) + "]";
        rep.structureStatus = checkInvariants(list, &before);
        rep.memoryStatus = memoryStatusText(list);
        recordLog(list, "ВСТАВИТИ_ПОЗ", "pos=" + to_string(pos) + ", val=" + to_string(rating),
                  rep.dataStatus, rep.structureStatus);
        return rep;
    }

    if (pos == 0) {
        StudentNode* node = allocateNode(list, name, grp, rating, list.head);
        list.head = node;
        if (list.tail == nullptr) {
            list.tail = node;
        }
        list.count++;
    } else if (pos == list.count) {
        StudentNode* node = allocateNode(list, name, grp, rating, nullptr);
        if (list.tail == nullptr) {
            list.head = node;
            list.tail = node;
        } else {
            list.tail->next = node;
            list.tail = node;
        }
        list.count++;
    } else {
        StudentNode* prev = list.head;
        for (size_t i = 0; i + 1 < pos; ++i) {
            prev = prev->next;
        }
        StudentNode* node = allocateNode(list, name, grp, rating, prev->next);
        prev->next = node;
        list.count++;
    }

    rep.success = true;
    rep.dataStatus = "Вставлено ключ " + to_string(rating) + " на позицію " + to_string(pos);
    rep.structureStatus = checkInvariants(list);
    rep.memoryStatus = memoryStatusText(list);
    recordLog(list, "ВСТАВИТИ_ПОЗ", "pos=" + to_string(pos) + ", val=" + to_string(rating),
              rep.dataStatus, rep.structureStatus);
    return rep;
}

OperationReport insertRelativeToFirstKey(SinglyLinkedList& list, double targetKey, bool beforeFirst,
                                         const string& name, const string& grp, double rating) {
    vector<double> before = snapshotKeys(list);
    OperationReport rep;

    if (list.head == nullptr) {
        rep.success = false;
        rep.dataStatus = "Відмова: ключ " + to_string(targetKey) + " не знайдено (список порожній)";
        rep.structureStatus = checkInvariants(list, &before);
        rep.memoryStatus = memoryStatusText(list);
        recordLog(list, beforeFirst ? "ВСТАВИТИ_ДО_КЛЮЧА" : "ВСТАВИТИ_ПІСЛЯ_КЛЮЧА",
                  "key=" + to_string(targetKey) + ", val=" + to_string(rating),
                  rep.dataStatus, rep.structureStatus);
        return rep;
    }

    StudentNode* prev = nullptr;
    StudentNode* curr = list.head;
    size_t pos = 0;

    while (curr != nullptr) {
        rep.inspectedNodes++;
        if (curr->rating == targetKey) {
            if (beforeFirst) {
                StudentNode* node = allocateNode(list, name, grp, rating, curr);
                if (prev == nullptr) {
                    list.head = node;
                } else {
                    prev->next = node;
                }
                list.count++;
                rep.dataStatus = "Вставлено " + to_string(rating) + " перед першим збігом ключа " +
                                 to_string(targetKey) + " (поз. " + to_string(pos) + ")";
            } else {
                StudentNode* node = allocateNode(list, name, grp, rating, curr->next);
                curr->next = node;
                if (curr == list.tail) {
                    list.tail = node;
                }
                list.count++;
                rep.dataStatus = "Вставлено " + to_string(rating) + " після першого збігу ключа " +
                                 to_string(targetKey) + " (поз. " + to_string(pos) + ")";
            }
            rep.success = true;
            rep.structureStatus = checkInvariants(list);
            rep.memoryStatus = memoryStatusText(list);
            recordLog(list, beforeFirst ? "ВСТАВИТИ_ДО_КЛЮЧА" : "ВСТАВИТИ_ПІСЛЯ_КЛЮЧА",
                      "key=" + to_string(targetKey) + ", val=" + to_string(rating),
                      rep.dataStatus, rep.structureStatus);
            return rep;
        }
        prev = curr;
        curr = curr->next;
        pos++;
    }

    rep.success = false;
    rep.dataStatus = "Відмова: ключ " + to_string(targetKey) + " відсутній (вузол не створено)";
    rep.structureStatus = checkInvariants(list, &before);
    rep.memoryStatus = memoryStatusText(list);
    recordLog(list, beforeFirst ? "ВСТАВИТИ_ДО_КЛЮЧА" : "ВСТАВИТИ_ПІСЛЯ_КЛЮЧА",
              "key=" + to_string(targetKey) + ", val=" + to_string(rating),
              rep.dataStatus, rep.structureStatus);
    return rep;
}

OperationReport removeAtPosition(SinglyLinkedList& list, size_t pos) {
    vector<double> before = snapshotKeys(list);
    OperationReport rep;

    if (list.head == nullptr || pos >= list.count) {
        rep.success = false;
        rep.dataStatus = "Відмова: некоректна позиція видалення " + to_string(pos);
        rep.structureStatus = checkInvariants(list, &before);
        rep.memoryStatus = memoryStatusText(list);
        recordLog(list, "ВИДАЛИТИ_ПОЗ", "pos=" + to_string(pos),
                  rep.dataStatus, rep.structureStatus);
        return rep;
    }

    StudentNode* target = nullptr;
    if (pos == 0) {
        target = list.head;
        StudentNode* nextHead = target->next;
        list.head = nextHead;
        if (list.head == nullptr) {
            list.tail = nullptr;
        }
        releaseNode(list, target);
        list.count--;
    } else {
        StudentNode* prev = list.head;
        for (size_t i = 0; i + 1 < pos; ++i) {
            prev = prev->next;
        }
        target = prev->next;
        StudentNode* afterTarget = target->next;
        prev->next = afterTarget;
        if (target == list.tail) {
            list.tail = prev;
        }
        releaseNode(list, target);
        list.count--;
    }

    rep.success = true;
    rep.dataStatus = "Видалено вузол на позиції " + to_string(pos);
    rep.structureStatus = checkInvariants(list);
    rep.memoryStatus = memoryStatusText(list);
    recordLog(list, "ВИДАЛИТИ_ПОЗ", "pos=" + to_string(pos),
              rep.dataStatus, rep.structureStatus);
    return rep;
}

OperationReport removeByRating(SinglyLinkedList& list, double targetKey, bool removeAll) {
    vector<double> before = snapshotKeys(list);
    OperationReport rep;

    if (list.head == nullptr) {
        rep.success = false;
        rep.dataStatus = "Відмова: список порожній, ключ " + to_string(targetKey) + " не знайдено";
        rep.structureStatus = checkInvariants(list, &before);
        rep.memoryStatus = memoryStatusText(list);
        recordLog(list, removeAll ? "ВИДАЛИТИ_ВСІ" : "ВИДАЛИТИ_ПЕРШЕ",
                  "key=" + to_string(targetKey), rep.dataStatus, rep.structureStatus);
        return rep;
    }

    StudentNode* prev = nullptr;
    StudentNode* curr = list.head;
    size_t deletedCount = 0;

    while (curr != nullptr) {
        rep.inspectedNodes++;
        StudentNode* nextNode = curr->next;

        if (curr->rating == targetKey) {
            if (prev == nullptr) {
                list.head = nextNode;
            } else {
                prev->next = nextNode;
            }
            if (curr == list.tail) {
                list.tail = prev;
            }
            releaseNode(list, curr);
            list.count--;
            deletedCount++;

            curr = nextNode;
            if (!removeAll) {
                break;
            }
        } else {
            prev = curr;
            curr = nextNode;
        }
    }

    if (list.head == nullptr) {
        list.tail = nullptr;
        list.count = 0;
    }

    if (deletedCount == 0) {
        rep.success = false;
        rep.dataStatus = "Відмова: ключ " + to_string(targetKey) + " не знайдено (список без змін)";
        rep.structureStatus = checkInvariants(list, &before);
    } else {
        rep.success = true;
        rep.dataStatus = (removeAll ? "Видалено всіх збігів ключа " : "Видалено перший збіг ключа ") +
                         to_string(targetKey) + ": " + to_string(deletedCount);
        rep.structureStatus = checkInvariants(list);
    }

    rep.memoryStatus = memoryStatusText(list);
    recordLog(list, removeAll ? "ВИДАЛИТИ_ВСІ" : "ВИДАЛИТИ_ПЕРШЕ",
              "key=" + to_string(targetKey), rep.dataStatus, rep.structureStatus);
    return rep;
}

void printSequence(const SinglyLinkedList& list, const string& label = "Список") {
    cout << label << " (кількість=" << list.count << "): ";
    const StudentNode* curr = list.head;
    size_t guard = 0;
    while (curr != nullptr && guard < list.count + 10) {
        cout << curr->rating;
        if (curr->next != nullptr) cout << " -> ";
        curr = curr->next;
        guard++;
    }
    if (list.head == nullptr) cout << "порожньо";
    cout << "\n";
}

void runAutomatedTests() {
    int passed = 0;

    { SinglyLinkedList l; SearchResult r = findFirstByRating(l, 5.0); assert(!r.found && r.inspectedNodes == 0); clearList(l); passed++; }
    { SinglyLinkedList l; assert(insertAtPosition(l, 0, "С1", "34-ІП", 5.0).success); assert((snapshotKeys(l) == vector<double>{5.0})); clearList(l); passed++; }
    { SinglyLinkedList l; insertAtPosition(l, 0, "С1", "34-ІП", 5.0); assert(removeAtPosition(l, 0).success); assert(snapshotKeys(l).empty()); clearList(l); passed++; }
    { SinglyLinkedList l; insertAtPosition(l, 0, "С1", "34-ІП", 1.0); insertAtPosition(l, 1, "С3", "34-ІП", 3.0); assert(insertAtPosition(l, 1, "С2", "34-ІП", 2.0).success); assert((snapshotKeys(l) == vector<double>{1.0, 2.0, 3.0})); clearList(l); passed++; }
    { SinglyLinkedList l; insertAtPosition(l, 0, "С1", "34-ІП", 1.0); insertAtPosition(l, 1, "С2", "34-ІП", 2.0); assert(insertAtPosition(l, 0, "С0", "34-ІП", 0.0).success); assert((snapshotKeys(l) == vector<double>{0.0, 1.0, 2.0})); clearList(l); passed++; }
    { SinglyLinkedList l; insertAtPosition(l, 0, "С1", "34-ІП", 1.0); insertAtPosition(l, 1, "С2", "34-ІП", 2.0); assert(insertAtPosition(l, 2, "С3", "34-ІП", 3.0).success); assert((snapshotKeys(l) == vector<double>{1.0, 2.0, 3.0})); clearList(l); passed++; }
    { SinglyLinkedList l; for (double v : {1.0, 2.0, 3.0}) insertAtPosition(l, l.count, "С", "34-ІП", v); assert(removeAtPosition(l, 0).success); assert((snapshotKeys(l) == vector<double>{2.0, 3.0})); clearList(l); passed++; }
    { SinglyLinkedList l; for (double v : {1.0, 2.0, 3.0}) insertAtPosition(l, l.count, "С", "34-ІП", v); assert(removeAtPosition(l, 1).success); assert((snapshotKeys(l) == vector<double>{1.0, 3.0})); clearList(l); passed++; }
    { SinglyLinkedList l; for (double v : {1.0, 2.0, 3.0}) insertAtPosition(l, l.count, "С", "34-ІП", v); assert(removeAtPosition(l, 2).success); assert((snapshotKeys(l) == vector<double>{1.0, 2.0})); clearList(l); passed++; }
    { SinglyLinkedList l; for (double v : {1.0, 2.0, 2.0, 3.0}) insertAtPosition(l, l.count, "С", "34-ІП", v); assert(removeByRating(l, 2.0, false).success); assert((snapshotKeys(l) == vector<double>{1.0, 2.0, 3.0})); clearList(l); passed++; }
    { SinglyLinkedList l; for (double v : {2.0, 2.0, 1.0, 2.0}) insertAtPosition(l, l.count, "С", "34-ІП", v); assert(removeByRating(l, 2.0, true).success); assert((snapshotKeys(l) == vector<double>{1.0})); clearList(l); passed++; }
    { SinglyLinkedList l; for (double v : {1.0, 2.0, 3.0}) insertAtPosition(l, l.count, "С", "34-ІП", v); assert(!insertAtPosition(l, 5, "X", "34-ІП", 99.0).success); assert((snapshotKeys(l) == vector<double>{1.0, 2.0, 3.0})); clearList(l); passed++; }
    { SinglyLinkedList l; for (double v : {1.0, 2.0, 3.0}) insertAtPosition(l, l.count, "С", "34-ІП", v); assert(!removeAtPosition(l, static_cast<size_t>(-1)).success); assert((snapshotKeys(l) == vector<double>{1.0, 2.0, 3.0})); clearList(l); passed++; }
    { SinglyLinkedList l; for (double v : {1.0, 2.0, 3.0}) insertAtPosition(l, l.count, "С", "34-ІП", v); assert(insertRelativeToFirstKey(l, 3.0, false, "С9", "34-ІП", 9.0).success); assert((snapshotKeys(l) == vector<double>{1.0, 2.0, 3.0, 9.0})); clearList(l); passed++; }
    { SinglyLinkedList l; for (double v : {1.0, 2.0, 3.0}) insertAtPosition(l, l.count, "С", "34-ІП", v); assert(insertRelativeToFirstKey(l, 1.0, true, "С0", "34-ІП", 0.0).success); assert((snapshotKeys(l) == vector<double>{0.0, 1.0, 2.0, 3.0})); clearList(l); passed++; }
    { SinglyLinkedList l; for (double v : {7.0, 7.0, 4.0, 7.0, 7.0}) insertAtPosition(l, l.count, "С", "34-ІП", v); assert(removeByRating(l, 7.0, true).success); assert((snapshotKeys(l) == vector<double>{4.0})); clearList(l); passed++; }
    { SinglyLinkedList l; insertAtPosition(l, 0, "А", "34-ІП", 10.0); assert(removeAtPosition(l, 0).success); assert(insertAtPosition(l, 0, "Б", "34-ІП", 20.0).success); assert((snapshotKeys(l) == vector<double>{20.0})); clearList(l); passed++; }
    { SinglyLinkedList l; insertAtPosition(l, 0, "А", "34-ІП", 4.0); size_t beforeCreated = l.createdNodes; assert(!insertRelativeToFirstKey(l, 999.0, true, "Зайвий", "34-ІП", 50.0).success); assert(l.createdNodes == beforeCreated); clearList(l); passed++; }
    { SinglyLinkedList l; for (double v : {4.0, 9.0, 2.0, 7.0, 9.0}) insertAtPosition(l, l.count, "С", "34-ІП", v); SearchResult all = findAllByRating(l, 9.0); assert(all.found && all.matches.size() == 2); clearList(l); passed++; }
    {
        SinglyLinkedList l;
        assert(insertAtPosition(l, 0, "С1", "34-ІП", 4.0).success);
        assert(insertAtPosition(l, 1, "С2", "34-ІП", 9.0).success);
        assert(insertRelativeToFirstKey(l, 9.0, false, "С3", "34-ІП", 5.0).success);
        assert(!removeAtPosition(l, 10).success);
        assert(removeByRating(l, 9.0, false).success);
        assert(insertAtPosition(l, 0, "С0", "34-ІП", 1.0).success);
        assert(removeAtPosition(l, 0).success);
        assert(insertAtPosition(l, l.count, "С4", "34-ІП", 8.0).success);
        assert(removeByRating(l, 4.0, true).success);
        assert(removeByRating(l, 5.0, true).success);
        clearList(l);
        assert(l.createdNodes == l.freedNodes && l.head == nullptr && l.tail == nullptr);
        passed++;
    }

    cout << "Пройдено автоматизованих сценаріїв: " << passed << " / 20\n";
}

void runCommandScenario() {
    SinglyLinkedList list;
    vector<double> initVals = {4.0, 9.0, 2.0, 7.0, 9.0};
    for (size_t i = 0; i < initVals.size(); ++i) {
        insertAtPosition(list, i, "Студент_" + to_string(i + 1), "34-ІП", initVals[i]);
    }
    printSequence(list, "Початковий список");

    SearchResult first9 = findFirstByRating(list, 9.0);
    recordLog(list, "ПОШУК_ПЕРШЕ", "key=9",
              first9.found ? ("перший на поз. " + to_string(first9.matches[0].position)) : "не знайдено",
              checkInvariants(list));

    SearchResult all9 = findAllByRating(list, 9.0);
    recordLog(list, "ПОШУК_УСІ", "key=9",
              "збігів=" + to_string(all9.matches.size()),
              checkInvariants(list));

    insertAtPosition(list, 2, "Новий_5", "34-ІП", 5.0);
    printSequence(list, "Після вставки 5 на поз. 2");

    removeByRating(list, 9.0, true);
    printSequence(list, "Після видалення всіх 9");

    clearList(list);
    cout << "Фінальний стан: " << checkInvariants(list) << " | " << memoryStatusText(list) << "\n";
}

int main() {
    setlocale(LC_ALL, "");
    runAutomatedTests();
    runCommandScenario();
    return 0;
}
