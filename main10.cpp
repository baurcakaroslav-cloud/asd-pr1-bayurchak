#include <cstdio>
#include <cstring>
#include <iomanip>
#include <iostream>

constexpr int MAX_CAPACITY = 10000;
constexpr int NAME_BUF_SIZE = 64;
constexpr int GROUP_BUF_SIZE = 24;

struct Student {
    char fullName[NAME_BUF_SIZE];
    char group[GROUP_BUF_SIZE];
    double rating;
};

struct MemoryCounters {
    int allocCount = 0;
    int freeCount = 0;
    int movedCount = 0;

    int activeBlocks() const {
        return allocCount - freeCount;
    }
};

struct DynamicStudentArray {
    Student* data = nullptr;
    int size = 0;
    int capacity = 0;
    MemoryCounters counters;
};

void copyBoundedString(char* dst, int dstSize, const char* src) {
    if (dst == nullptr || dstSize <= 0) {
        return;
    }
    if (src == nullptr) {
        dst[0] = '\0';
        return;
    }
    std::strncpy(dst, src, static_cast<std::size_t>(dstSize - 1));
    dst[dstSize - 1] = '\0';
}

Student makeStudent(const char* name, const char* group, double rating) {
    Student s{};
    copyBoundedString(s.fullName, NAME_BUF_SIZE, name);
    copyBoundedString(s.group, GROUP_BUF_SIZE, group);
    s.rating = rating;
    return s;
}

bool isValidSizeForInit(int n) {
    return n > 0 && n <= MAX_CAPACITY;
}

bool isValidIndex(const DynamicStudentArray& arr, int index) {
    return index >= 0 && index < arr.size;
}

Student* allocSingleStudent(MemoryCounters& counters) {
    Student* ptr = new Student{};
    counters.allocCount += 1;
    return ptr;
}

void freeSingleStudent(Student*& ptr, MemoryCounters& counters) {
    if (ptr != nullptr) {
        delete ptr;
        ptr = nullptr;
        counters.freeCount += 1;
    }
}

Student* allocStudentBlock(int newCapacity, MemoryCounters& counters) {
    if (newCapacity <= 0 || newCapacity > MAX_CAPACITY) {
        return nullptr;
    }
    Student* block = new Student[newCapacity];
    counters.allocCount += 1;
    return block;
}

void freeStudentBlock(Student*& block, MemoryCounters& counters) {
    if (block != nullptr) {
        delete[] block;
        block = nullptr;
        counters.freeCount += 1;
    }
}

void clearArray(DynamicStudentArray& arr) {
    freeStudentBlock(arr.data, arr.counters);
    arr.size = 0;
    arr.capacity = 0;
}

bool initArray(DynamicStudentArray& arr, int initialSize, int initialCapacity = -1) {
    clearArray(arr);
    if (!isValidSizeForInit(initialSize)) {
        return false;
    }
    int cap = (initialCapacity < 0) ? initialSize : initialCapacity;
    if (cap < initialSize || cap > MAX_CAPACITY) {
        return false;
    }
    arr.data = allocStudentBlock(cap, arr.counters);
    if (arr.data == nullptr) {
        return false;
    }
    arr.size = initialSize;
    arr.capacity = cap;
    for (int i = 0; i < arr.size; ++i) {
        arr.data[i] = makeStudent("", "", 0.0);
    }
    return true;
}

bool reallocateBlock(DynamicStudentArray& arr, int newCapacity) {
    if (newCapacity < arr.size || newCapacity > MAX_CAPACITY) {
        return false;
    }
    if (newCapacity == 0) {
        clearArray(arr);
        return true;
    }
    Student* newBlock = allocStudentBlock(newCapacity, arr.counters);
    if (newBlock == nullptr) {
        return false;
    }
    for (int i = 0; i < arr.size; ++i) {
        newBlock[i] = arr.data[i];
        arr.counters.movedCount += 1;
    }
    freeStudentBlock(arr.data, arr.counters);
    arr.data = newBlock;
    arr.capacity = newCapacity;
    return true;
}

bool appendPlusOne(DynamicStudentArray& arr, const Student& item) {
    if (arr.size >= MAX_CAPACITY) {
        return false;
    }
    if (arr.size == arr.capacity) {
        int nextCap = arr.capacity + 1;
        if (nextCap > MAX_CAPACITY || !reallocateBlock(arr, nextCap)) {
            return false;
        }
    }
    arr.data[arr.size] = item;
    arr.size += 1;
    return true;
}

bool appendDoubling(DynamicStudentArray& arr, const Student& item) {
    if (arr.size >= MAX_CAPACITY) {
        return false;
    }
    if (arr.size == arr.capacity) {
        int nextCap = (arr.capacity == 0) ? 1 : arr.capacity * 2;
        if (nextCap > MAX_CAPACITY) {
            nextCap = MAX_CAPACITY;
        }
        if (nextCap <= arr.capacity || !reallocateBlock(arr, nextCap)) {
            return false;
        }
    }
    arr.data[arr.size] = item;
    arr.size += 1;
    return true;
}

bool removeAtIndex(DynamicStudentArray& arr, int index, bool autoShrink = false) {
    if (!isValidIndex(arr, index)) {
        return false;
    }
    if (arr.size == 1) {
        clearArray(arr);
        return true;
    }
    int newSize = arr.size - 1;
    int targetCap = newSize;
    if (autoShrink && arr.capacity >= 4 && newSize <= arr.capacity / 4) {
        targetCap = arr.capacity / 2;
        if (targetCap < newSize) {
            targetCap = newSize;
        }
    }
    Student* newBlock = allocStudentBlock(targetCap, arr.counters);
    if (newBlock == nullptr) {
        return false;
    }
    int writeIdx = 0;
    for (int i = 0; i < arr.size; ++i) {
        if (i == index) {
            continue;
        }
        newBlock[writeIdx] = arr.data[i];
        arr.counters.movedCount += 1;
        writeIdx += 1;
    }
    freeStudentBlock(arr.data, arr.counters);
    arr.data = newBlock;
    arr.size = newSize;
    arr.capacity = targetCap;
    return true;
}

int findFirstByGroup(const DynamicStudentArray& arr, const char* targetGroup) {
    if (arr.data == nullptr || arr.size == 0 || targetGroup == nullptr) {
        return -1;
    }
    for (int i = 0; i < arr.size; ++i) {
        if (std::strcmp(arr.data[i].group, targetGroup) == 0) {
            return i;
        }
    }
    return -1;
}

bool updateRatingByIndex(DynamicStudentArray& arr, int index, double newRating) {
    if (!isValidIndex(arr, index) || arr.data == nullptr) {
        return false;
    }
    arr.data[index].rating = newRating;
    return true;
}

double averageRatingByGroup(const DynamicStudentArray& arr, const char* targetGroup) {
    if (arr.data == nullptr || arr.size == 0 || targetGroup == nullptr) {
        return 0.0;
    }
    double sum = 0.0;
    int count = 0;
    for (int i = 0; i < arr.size; ++i) {
        if (std::strcmp(arr.data[i].group, targetGroup) == 0) {
            sum += arr.data[i].rating;
            count += 1;
        }
    }
    return (count > 0) ? (sum / static_cast<double>(count)) : 0.0;
}

void printTable(const DynamicStudentArray& arr) {
    if (arr.size == 0 || arr.data == nullptr) {
        std::cout << "[Порожній масив (size = 0, capacity = " << arr.capacity << ")]\n";
        return;
    }
    std::cout << std::left << std::setw(5) << "#"
              << std::setw(28) << "ПІБ"
              << std::setw(12) << "Група"
              << std::right << std::setw(10) << "Рейтинг\n";
    for (int i = 0; i < arr.size; ++i) {
        std::cout << std::left << std::setw(5) << (i + 1)
                  << std::setw(28) << arr.data[i].fullName
                  << std::setw(12) << arr.data[i].group
                  << std::right << std::setw(10) << std::fixed << std::setprecision(2)
                  << arr.data[i].rating << "\n";
    }
}

struct GrowthStats {
    int reallocations;
    int movedElements;
};

GrowthStats simulateGrowth(int additions, bool doubling) {
    DynamicStudentArray sim;
    int beforeAllocs = sim.counters.allocCount;
    int beforeMoved = sim.counters.movedCount;

    for (int i = 0; i < additions; ++i) {
        Student s = makeStudent("Студент", "34-ІП", 80.0 + (i % 20));
        if (doubling) {
            appendDoubling(sim, s);
        } else {
            appendPlusOne(sim, s);
        }
    }

    int totalAllocs = sim.counters.allocCount - beforeAllocs;
    int reallocs = (totalAllocs > 0) ? (totalAllocs - 1) : 0;
    int moved = sim.counters.movedCount - beforeMoved;
    clearArray(sim);
    return {reallocs, moved};
}

void printGrowthComparison() {
    const int counts[3] = {10, 100, 1000};
    std::cout << "\n=== Порівняння стратегій збільшення місткості ===\n";
    std::cout << std::left << std::setw(12) << "Додавань"
              << std::setw(24) << "+1 (перевид./перенес.)"
              << std::setw(24) << "*2 (перевид./перенес.)"
              << std::setw(26) << "Зекономлено (пер./перен.)\n";
    for (int i = 0; i < 3; ++i) {
        int n = counts[i];
        GrowthStats sPlus = simulateGrowth(n, false);
        GrowthStats sDbl = simulateGrowth(n, true);
        int savedRealloc = sPlus.reallocations - sDbl.reallocations;
        int savedMoved = sPlus.movedElements - sDbl.movedElements;

        char bufPlus[32];
        char bufDbl[32];
        char bufSaved[32];
        std::snprintf(bufPlus, sizeof(bufPlus), "%d / %d", sPlus.reallocations, sPlus.movedElements);
        std::snprintf(bufDbl, sizeof(bufDbl), "%d / %d", sDbl.reallocations, sDbl.movedElements);
        std::snprintf(bufSaved, sizeof(bufSaved), "%d / %d", savedRealloc, savedMoved);

        std::cout << std::left << std::setw(12) << n
                  << std::setw(24) << bufPlus
                  << std::setw(24) << bufDbl
                  << std::setw(26) << bufSaved << "\n";
    }
}

int runAllTests() {
    int passed = 0;
    auto check = [&](bool cond, const char* name) {
        if (cond) {
            passed += 1;
            std::cout << "[OK] " << name << "\n";
        } else {
            std::cout << "[FAIL] " << name << "\n";
        }
    };

    {
        DynamicStudentArray a;
        check(a.size == 0 && a.data == nullptr && a.counters.activeBlocks() == 0,
              "T01: порожня структура — перегляд без помилки");
    }
    {
        DynamicStudentArray a;
        bool ok = removeAtIndex(a, 0);
        check(!ok && a.size == 0 && a.data == nullptr && a.counters.activeBlocks() == 0,
              "T02: порожня структура — видалення відхилено");
    }
    {
        DynamicStudentArray a;
        initArray(a, 1, 1);
        a.data[0] = makeStudent("Іваненко І. І.", "34-ІП", 92.5);
        int idx = findFirstByGroup(a, "34-ІП");
        clearArray(a);
        check(idx == 0 && a.counters.activeBlocks() == 0,
              "T03: один елемент — пошук наявного повертає індекс 0");
    }
    {
        DynamicStudentArray a;
        initArray(a, 1, 1);
        a.data[0] = makeStudent("Петренко П. П.", "34-ІП", 88.0);
        bool ok = removeAtIndex(a, 0);
        check(ok && a.size == 0 && a.capacity == 0 && a.data == nullptr &&
              a.counters.activeBlocks() == 0,
              "T04: один елемент — видалення дає розмір 0 і data == nullptr");
    }
    {
        DynamicStudentArray a;
        initArray(a, 3, 3);
        a.data[0] = makeStudent("Студент 1", "34-ІП", 80.0);
        a.data[1] = makeStudent("Студент 2", "34-ІП", 85.0);
        a.data[2] = makeStudent("Студент 3", "35-ІП", 90.0);
        bool ok = appendDoubling(a, makeStudent("Студент 4", "34-ІП", 95.0));
        bool sameOld = (std::strcmp(a.data[0].fullName, "Студент 1") == 0) &&
                       (std::strcmp(a.data[2].fullName, "Студент 3") == 0);
        bool lastNew = (std::strcmp(a.data[3].fullName, "Студент 4") == 0);
        clearArray(a);
        check(ok && a.size == 4 && sameOld && lastNew && a.counters.activeBlocks() == 0,
              "T05: три елементи -> додавання дає розмір 4 і зберігає дані");
    }
    {
        DynamicStudentArray a;
        initArray(a, 3, 3);
        a.data[0] = makeStudent("Перший", "34-ІП", 70.0);
        a.data[1] = makeStudent("Другий", "34-ІП", 80.0);
        a.data[2] = makeStudent("Третій", "34-ІП", 90.0);
        bool ok = removeAtIndex(a, 0);
        bool orderOk = (a.size == 2) &&
                       (std::strcmp(a.data[0].fullName, "Другий") == 0) &&
                       (std::strcmp(a.data[1].fullName, "Третій") == 0);
        clearArray(a);
        check(ok && orderOk && a.counters.activeBlocks() == 0,
              "T06: три елементи -> видалення першого зберігає порядок");
    }
    {
        DynamicStudentArray a;
        initArray(a, 3, 3);
        a.data[0] = makeStudent("Перший", "34-ІП", 70.0);
        a.data[1] = makeStudent("Середній", "34-ІП", 80.0);
        a.data[2] = makeStudent("Останній", "34-ІП", 90.0);
        bool ok = removeAtIndex(a, 1);
        bool orderOk = (a.size == 2) &&
                       (std::strcmp(a.data[0].fullName, "Перший") == 0) &&
                       (std::strcmp(a.data[1].fullName, "Останній") == 0);
        clearArray(a);
        check(ok && orderOk && a.counters.activeBlocks() == 0,
              "T07: три елементи -> видалення середнього зберігає порядок");
    }
    {
        DynamicStudentArray a;
        initArray(a, 3, 3);
        a.data[0] = makeStudent("Перший", "34-ІП", 70.0);
        a.data[1] = makeStudent("Другий", "34-ІП", 80.0);
        a.data[2] = makeStudent("Останній", "34-ІП", 90.0);
        bool ok = removeAtIndex(a, 2);
        bool orderOk = (a.size == 2) &&
                       (std::strcmp(a.data[0].fullName, "Перший") == 0) &&
                       (std::strcmp(a.data[1].fullName, "Другий") == 0);
        clearArray(a);
        check(ok && orderOk && a.counters.activeBlocks() == 0,
              "T08: три елементи -> видалення останнього зберігає порядок");
    }
    {
        DynamicStudentArray a;
        initArray(a, 3, 3);
        a.data[0] = makeStudent("A", "34-ІП", 80.0);
        a.data[1] = makeStudent("B", "34-ІП", 85.0);
        a.data[2] = makeStudent("C", "34-ІП", 90.0);
        bool ok = removeAtIndex(a, -1);
        clearArray(a);
        check(!ok && a.counters.activeBlocks() == 0,
              "T09: три елементи -> індекс -1 відхилено без зміни");
    }
    {
        DynamicStudentArray a;
        initArray(a, 3, 3);
        a.data[0] = makeStudent("A", "34-ІП", 80.0);
        a.data[1] = makeStudent("B", "34-ІП", 85.0);
        a.data[2] = makeStudent("C", "34-ІП", 90.0);
        bool ok = removeAtIndex(a, 3);
        clearArray(a);
        check(!ok && a.counters.activeBlocks() == 0,
              "T10: три елементи -> індекс == size (3) відхилено");
    }
    {
        DynamicStudentArray a;
        initArray(a, 2, 2);
        int oldCap = a.capacity;
        a.data[0] = makeStudent("S1", "34-ІП", 85.0);
        a.data[1] = makeStudent("S2", "34-ІП", 90.0);
        bool ok = appendDoubling(a, makeStudent("S3", "34-ІП", 95.0));
        bool capGrew = (a.capacity > oldCap) && (a.size == 3);
        clearArray(a);
        check(ok && capGrew && a.counters.activeBlocks() == 0,
              "T11: заповнена місткість -> додавання збільшує місткість");
    }
    {
        DynamicStudentArray a;
        for (int i = 0; i < 100; ++i) {
            appendDoubling(a, makeStudent("Студент", "34-ІП", 75.0 + (i % 25)));
        }
        while (a.size > 0) {
            removeAtIndex(a, a.size - 1, true);
        }
        check(a.size == 0 && a.data == nullptr && a.counters.activeBlocks() == 0,
              "T12: 100 елементів -> послідовне очищення, 0 активних блоків");
    }
    {
        DynamicStudentArray a;
        initArray(a, 5, 5);
        for (int i = 0; i < 5; ++i) {
            a.data[i] = makeStudent("Перша хвиля", "34-ІП", 80.0);
        }
        clearArray(a);
        for (int i = 0; i < 6; ++i) {
            appendDoubling(a, makeStudent("Друга хвиля", "35-ІП", 91.0));
        }
        bool ok = (a.size == 6) && (std::strcmp(a.data[5].group, "35-ІП") == 0);
        clearArray(a);
        check(ok && a.counters.activeBlocks() == 0,
              "T13 (власн.): повторне заповнення після повного очищення");
    }
    {
        DynamicStudentArray a;
        bool allOk = true;
        for (int step = 0; step < 50; ++step) {
            allOk = allOk && appendDoubling(a, makeStudent("Черга", "34-ІП", 84.0));
            allOk = allOk && appendDoubling(a, makeStudent("Черга2", "34-ІП", 86.0));
            allOk = allOk && removeAtIndex(a, 0, true);
        }
        bool finalCount = (a.size == 50);
        clearArray(a);
        check(allOk && finalCount && a.counters.activeBlocks() == 0,
              "T14 (власн.): 50 чергувань додавання і видалення");
    }
    {
        DynamicStudentArray a;
        initArray(a, 2, 2);
        a.data[0] = makeStudent("Іван", "34-ІП", 88.0);
        a.data[1] = makeStudent("Олена", "35-ІП", 94.0);
        int found = findFirstByGroup(a, "99-ІП");
        bool unchanged = (a.size == 2) &&
                         (std::strcmp(a.data[0].group, "34-ІП") == 0) &&
                         (std::strcmp(a.data[1].group, "35-ІП") == 0);
        clearArray(a);
        check(found == -1 && unchanged && a.counters.activeBlocks() == 0,
              "T15 (власн.): невдалий пошук повертає -1 і не змінює масив");
    }

    return passed;
}

int main() {
    std::cout << "=== Завдання 1: Динамічний об'єкт Student ===\n";
    MemoryCounters objCounters;
    Student* single = allocSingleStudent(objCounters);
    *single = makeStudent("Коваленко Андрій Олегович", "34-ІП", 91.5);
    std::cout << "Створено об'єкт: " << single->fullName
              << " | Група: " << single->group
              << " | Рейтинг: " << single->rating << "\n";
    single->rating = 94.0;
    std::cout << "Оновлений рейтинг: " << single->rating << "\n";
    freeSingleStudent(single, objCounters);
    std::cout << "Після звільнення: pointer is " << (single == nullptr ? "nullptr" : "non-null")
              << ", активних блоків: " << objCounters.activeBlocks() << "\n\n";

    std::cout << "=== Завдання 2–5: Динамічний масив структур ===\n";
    DynamicStudentArray groupArr;
    initArray(groupArr, 3, 4);
    groupArr.data[0] = makeStudent("Баюрчак Ярослав Олександрович", "34-ІП", 92.0);
    groupArr.data[1] = makeStudent("Мельник Марія Сергіївна", "34-ІП", 88.0);
    groupArr.data[2] = makeStudent("Ткачук Олег Ігорович", "35-ІП", 95.0);
    printTable(groupArr);

    int idx = findFirstByGroup(groupArr, "34-ІП");
    if (idx >= 0) {
        updateRatingByIndex(groupArr, idx, 96.0);
    }
    double avg34 = averageRatingByGroup(groupArr, "34-ІП");
    std::cout << "Середній рейтинг групи 34-ІП: " << std::fixed << std::setprecision(2)
              << avg34 << "\n";
    std::cout << "Активних блоків до очищення: " << groupArr.counters.activeBlocks() << "\n";
    clearArray(groupArr);
    std::cout << "Активних блоків після очищення: " << groupArr.counters.activeBlocks() << "\n\n";

    std::cout << "=== Автоматизовані тести (15 тестів) ===\n";
    int okCount = runAllTests();
    std::cout << "Пройдено тестів: " << okCount << " / 15\n";

    printGrowthComparison();
    return (okCount == 15) ? 0 : 1;
}
