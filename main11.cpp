#include <iostream>
#include <iomanip>
#include <string>
#include <vector>
#include <unordered_set>
#include <sstream>
#include <cassert>

using namespace std;

// Завдання 1. Структура вузла (Варіант № 1: Студент — ПІБ, група, рейтинг)
struct StudentNode {
    string fullName;        // ПІБ
    string group;           // Група
    double rating = 0.0;    // Рейтинг
    StudentNode* next = nullptr;

    StudentNode() = default;
    StudentNode(const string& name, const string& grp, double rt, StudentNode* nxt = nullptr)
        : fullName(name), group(grp), rating(rt), next(nxt) {}
};

// Стан однонапрямленого списку та лічильники пам'яті
struct SinglyLinkedList {
    StudentNode* head = nullptr;
    StudentNode* tail = nullptr;
    size_t count = 0;

    size_t createdNodes = 0;
    size_t freedNodes = 0;
};

// Завдання 2. Автоматична перевірка узгодженості порожнього стану
bool checkEmptyState(const SinglyLinkedList& list) {
    if (list.count == 0) {
        return (list.head == nullptr && list.tail == nullptr);
    }
    return (list.head != nullptr && list.tail != nullptr);
}

// Створення нового вузла з коректною початковою адресою next = nullptr
StudentNode* createNode(SinglyLinkedList& list, const string& name, const string& grp, double rating) {
    StudentNode* node = new StudentNode(name, grp, rating, nullptr);
    list.createdNodes++;
    return node;
}

// Завдання 3. Створення зі збереженням порядку введення (приєднання в кінець)
void appendBack(SinglyLinkedList& list, const string& name, const string& grp, double rating) {
    StudentNode* node = createNode(list, name, grp, rating);
    if (list.head == nullptr) {
        list.head = node;
        list.tail = node;
    } else {
        list.tail->next = node;
        list.tail = node;
    }
    list.count++;
}

// Завдання 4. Створення у зворотному порядку введення (приєднання на початок)
void prependFront(SinglyLinkedList& list, const string& name, const string& grp, double rating) {
    StudentNode* node = createNode(list, name, grp, rating);
    if (list.head == nullptr) {
        list.head = node;
        list.tail = node;
    } else {
        node->next = list.head;
        list.head = node;
    }
    list.count++;
}

// Завдання 5. Перегляд списку у вигляді таблиці (не змінює head і tail)
void viewListTable(const SinglyLinkedList& list, ostream& out = cout) {
    out << "\n=== Перегляд списку (Варіант № 1: Студент) ===\n";
    if (list.head == nullptr) {
        out << "Список порожній. Переглянуто елементів: 0\n";
        return;
    }

    out << left
        << setw(5)  << "№"
        << setw(26) << "ПІБ"
        << setw(12) << "Група"
        << right << setw(10) << "Рейтинг" << "\n";
    out << string(53, '-') << "\n";

    const StudentNode* curr = list.head; // Окремий робочий покажчик
    size_t idx = 0;
    while (curr != nullptr) {
        ++idx;
        out << left
            << setw(5)  << idx
            << setw(26) << curr->fullName
            << setw(12) << curr->group
            << right << setw(10) << fixed << setprecision(2) << curr->rating << "\n";
        curr = curr->next;
    }
    out << string(53, '-') << "\n";
    out << "Загальна кількість переглянутих елементів: " << idx << "\n";
}

// Завдання 6. Діагностичний режим: перегляд адрес вузлів і покажчиків next
void viewDiagnostics(const SinglyLinkedList& list, ostream& out = cout) {
    out << "\n=== Діагностичний перегляд адрес і зв'язків ===\n";
    if (list.head == nullptr) {
        out << "Список порожній (head = nullptr, tail = nullptr).\n";
        return;
    }

    const StudentNode* curr = list.head;
    unordered_set<const StudentNode*> visited;
    size_t idx = 0;
    const size_t maxSteps = list.count + 10;

    while (curr != nullptr && idx < maxSteps) {
        ++idx;
        bool repeated = (visited.find(curr) != visited.end());
        out << "Вузол #" << idx
            << " | ПІБ: " << curr->fullName
            << " | Рейтинг: " << curr->rating
            << " | Адреса: " << static_cast<const void*>(curr)
            << " | next: ";

        if (repeated) {
            out << static_cast<const void*>(curr->next) << " [ПОВТОР / ЦИКЛ]\n";
            break;
        }
        visited.insert(curr);

        if (curr->next == nullptr) {
            out << "nullptr (останній вузол)\n";
        } else {
            out << static_cast<const void*>(curr->next) << "\n";
        }
        curr = curr->next;
    }
}

// Завдання 7. Автоматична перевірка структури списку
struct ValidationReport {
    bool valid = true;
    string status;
    size_t reachable = 0;
};

ValidationReport validateList(const SinglyLinkedList& list) {
    ValidationReport report;

    if (!checkEmptyState(list)) {
        report.valid = false;
        report.status = "Помилка: неузгоджений порожній стан (count == 0, але head або tail не порожній).";
        return report;
    }

    if (list.head == nullptr) {
        report.valid = true;
        report.reachable = 0;
        report.status = "OK: порожній список коректний (0 вузлів).";
        return report;
    }

    unordered_set<const StudentNode*> seen;
    const StudentNode* curr = list.head;
    const StudentNode* lastNode = nullptr;
    const size_t stepLimit = list.count + 50;

    while (curr != nullptr) {
        if (seen.find(curr) != seen.end()) {
            report.valid = false;
            report.status = "Помилка: виявлено можливий цикл у зв'язках списку.";
            return report;
        }
        if (report.reachable >= stepLimit) {
            report.valid = false;
            report.status = "Помилка: перевищено ліміт кроків обходу (можливий цикл або розрив).";
            return report;
        }

        seen.insert(curr);
        lastNode = curr;
        report.reachable++;
        curr = curr->next;
    }

    if (report.reachable != list.count) {
        report.valid = false;
        report.status = "Помилка: невідповідність кількості вузлів (очікувалося " +
                        to_string(list.count) + ", досяжно " + to_string(report.reachable) + ").";
        return report;
    }

    if (lastNode != list.tail || list.tail->next != nullptr) {
        report.valid = false;
        report.status = "Помилка: хибний покажчик кінця (tail) або ненульовий next в останньому вузлі.";
        return report;
    }

    report.valid = true;
    report.status = "OK: структура коректна (вузлів: " + to_string(report.reachable) +
                    ", tail коректний, розривів і циклів немає).";
    return report;
}

// Завдання 8. Послідовне безпечне очищення списку від початку
void clearList(SinglyLinkedList& list) {
    StudentNode* curr = list.head;
    while (curr != nullptr) {
        StudentNode* nextAddr = curr->next;
        delete curr;
        list.freedNodes++;
        curr = nextAddr;
    }
    list.head = nullptr;
    list.tail = nullptr;
    list.count = 0;
}

vector<double> getRatings(const SinglyLinkedList& list) {
    vector<double> ratings;
    const StudentNode* curr = list.head;
    while (curr != nullptr) {
        ratings.push_back(curr->rating);
        curr = curr->next;
    }
    return ratings;
}

// Автоматизовані тести (15 тестів: 12 обов'язкових + 3 власні)
void runTests() {
    int passed = 0;

    // 1. Без елементів -> у кінець -> порожньо
    {
        SinglyLinkedList l;
        assert(validateList(l).valid);
        assert(getRatings(l).empty());
        clearList(l);
        assert(l.createdNodes == l.freedNodes);
        passed++;
    }

    // 2. [7] -> у кінець -> [7]
    {
        SinglyLinkedList l;
        appendBack(l, "Студент 1", "34-ІП", 7.0);
        assert(validateList(l).valid);
        assert((getRatings(l) == vector<double>{7.0}));
        clearList(l);
        assert(l.createdNodes == 1 && l.freedNodes == 1);
        passed++;
    }

    // 3. [7] -> на початок -> [7]
    {
        SinglyLinkedList l;
        prependFront(l, "Студент 1", "34-ІП", 7.0);
        assert(validateList(l).valid);
        assert((getRatings(l) == vector<double>{7.0}));
        clearList(l);
        assert(l.createdNodes == 1 && l.freedNodes == 1);
        passed++;
    }

    // 4. [1, 2, 3] -> у кінець -> [1, 2, 3]
    {
        SinglyLinkedList l;
        for (double v : {1.0, 2.0, 3.0}) appendBack(l, "Студент", "34-ІП", v);
        assert(validateList(l).valid);
        assert((getRatings(l) == vector<double>{1.0, 2.0, 3.0}));
        clearList(l);
        assert(l.createdNodes == l.freedNodes);
        passed++;
    }

    // 5. [1, 2, 3] -> на початок -> [3, 2, 1]
    {
        SinglyLinkedList l;
        for (double v : {1.0, 2.0, 3.0}) prependFront(l, "Студент", "34-ІП", v);
        assert(validateList(l).valid);
        assert((getRatings(l) == vector<double>{3.0, 2.0, 1.0}));
        clearList(l);
        assert(l.createdNodes == l.freedNodes);
        passed++;
    }

    // 6. [5, 5, 5] -> у кінець -> [5, 5, 5]
    {
        SinglyLinkedList l;
        for (double v : {5.0, 5.0, 5.0}) appendBack(l, "Студент", "34-ІП", v);
        assert(validateList(l).valid);
        assert((getRatings(l) == vector<double>{5.0, 5.0, 5.0}));
        clearList(l);
        assert(l.createdNodes == l.freedNodes);
        passed++;
    }

    // 7. [-2, 0, 4] -> у кінець -> [-2, 0, 4]
    {
        SinglyLinkedList l;
        for (double v : {-2.0, 0.0, 4.0}) appendBack(l, "Студент", "34-ІП", v);
        assert(validateList(l).valid);
        assert((getRatings(l) == vector<double>{-2.0, 0.0, 4.0}));
        clearList(l);
        assert(l.createdNodes == l.freedNodes);
        passed++;
    }

    // 8. [-2, 0, 4] -> на початок -> [4, 0, -2]
    {
        SinglyLinkedList l;
        for (double v : {-2.0, 0.0, 4.0}) prependFront(l, "Студент", "34-ІП", v);
        assert(validateList(l).valid);
        assert((getRatings(l) == vector<double>{4.0, 0.0, -2.0}));
        clearList(l);
        assert(l.createdNodes == l.freedNodes);
        passed++;
    }

    // 9. 10 елементів -> у кінець -> порядок введення
    {
        SinglyLinkedList l;
        for (int i = 1; i <= 10; ++i) appendBack(l, "Студент " + to_string(i), "34-ІП", static_cast<double>(i));
        assert(validateList(l).valid && l.count == 10);
        vector<double> r = getRatings(l);
        for (int i = 0; i < 10; ++i) assert(r[i] == static_cast<double>(i + 1));
        clearList(l);
        assert(l.createdNodes == l.freedNodes);
        passed++;
    }

    // 10. 10 елементів -> на початок -> зворотний порядок
    {
        SinglyLinkedList l;
        for (int i = 1; i <= 10; ++i) prependFront(l, "Студент " + to_string(i), "34-ІП", static_cast<double>(i));
        assert(validateList(l).valid && l.count == 10);
        vector<double> r = getRatings(l);
        for (int i = 0; i < 10; ++i) assert(r[i] == static_cast<double>(10 - i));
        clearList(l);
        assert(l.createdNodes == l.freedNodes);
        passed++;
    }

    // 11. 3 елементи -> два перегляди -> однаковий результат
    {
        SinglyLinkedList l;
        appendBack(l, "Іванов А.А.", "34-ІП", 85.0);
        appendBack(l, "Петренко Б.Б.", "34-ІП", 90.0);
        appendBack(l, "Сидоренко В.В.", "34-ІП", 88.0);
        ostringstream v1, v2;
        viewListTable(l, v1);
        viewListTable(l, v2);
        assert(v1.str() == v2.str());
        assert(validateList(l).valid);
        clearList(l);
        passed++;
    }

    // 12. 100 елементів -> створити й очистити -> порожній стан без витоків
    {
        SinglyLinkedList l;
        for (int i = 1; i <= 100; ++i) appendBack(l, "Студент " + to_string(i), "34-ІП", 80.0);
        assert(validateList(l).valid && l.count == 100);
        clearList(l);
        assert(validateList(l).valid && l.count == 0);
        assert(l.createdNodes == 100 && l.freedNodes == 100);
        passed++;
    }

    // 13. Власний тест 1: повторне створення після очищення
    {
        SinglyLinkedList l;
        appendBack(l, "Перший", "34-ІП", 78.0);
        clearList(l);
        assert(validateList(l).valid);
        appendBack(l, "Після очищення", "34-ІП", 93.0);
        assert(validateList(l).valid && l.count == 1);
        assert(l.head->fullName == "Після очищення");
        clearList(l);
        assert(l.createdNodes == l.freedNodes);
        passed++;
    }

    // 14. Власний тест 2: два послідовні цикли створення та очищення
    {
        SinglyLinkedList l;
        for (int c = 0; c < 2; ++c) {
            appendBack(l, "A_" + to_string(c), "34-ІП", 81.0);
            prependFront(l, "B_" + to_string(c), "34-ІП", 87.0);
            assert(validateList(l).valid && l.count == 2);
            clearList(l);
            assert(validateList(l).valid && l.count == 0);
        }
        assert(l.createdNodes == 4 && l.freedNodes == 4);
        passed++;
    }

    // 15. Власний тест 3: перевірка діагностичного режиму та виявлення циклу
    {
        SinglyLinkedList l;
        appendBack(l, "Студент А", "34-ІП", 91.0);
        appendBack(l, "Студент Б", "34-ІП", 86.0);
        ostringstream diag;
        viewDiagnostics(l, diag);
        assert(!diag.str().empty());
        assert(validateList(l).valid);

        l.tail->next = l.head; // Моделювання помилкового циклу
        assert(!validateList(l).valid);
        l.tail->next = nullptr; // Відновлення зв'язку перед очищенням
        clearList(l);
        assert(l.createdNodes == l.freedNodes);
        passed++;
    }

    cout << "Успішно пройдено автоматизованих тестів: " << passed << " / 15\n";
}

int main() {
    setlocale(LC_ALL, "");

    runTests();

    // Демонстрація на вхідних значеннях 1, 2, 3 у двох режимах створення
    SinglyLinkedList directList;
    SinglyLinkedList reverseList;

    struct InputItem {
        string name;
        string group;
        double rating;
    };

    vector<InputItem> items = {
        {"Студент 1", "34-ІП", 1.0},
        {"Студент 2", "34-ІП", 2.0},
        {"Студент 3", "34-ІП", 3.0}
    };

    for (const auto& item : items) {
        appendBack(directList, item.name, item.group, item.rating);
        prependFront(reverseList, item.name, item.group, item.rating);
    }

    cout << "\n--- Режим 1: Створення зі збереженням порядку (1 -> 2 -> 3) ---";
    viewListTable(directList);
    cout << validateList(directList).status << "\n";
    viewDiagnostics(directList);

    cout << "\n--- Режим 2: Створення у зворотному порядку (3 -> 2 -> 1) ---";
    viewListTable(reverseList);
    cout << validateList(reverseList).status << "\n";
    viewDiagnostics(reverseList);

    clearList(directList);
    clearList(reverseList);

    cout << "\nПісля очищення directList: " << validateList(directList).status
         << " | створено=" << directList.createdNodes
         << ", звільнено=" << directList.freedNodes << "\n";
    cout << "Після очищення reverseList: " << validateList(reverseList).status
         << " | створено=" << reverseList.createdNodes
         << ", звільнено=" << reverseList.freedNodes << "\n";

    return 0;
}
