#include <iostream>
#include <iomanip>
#include <cstring>
#include <cstdlib>

struct StageStats {
    long long comparisons = 0;
    long long swaps = 0;
    long long siftCalls = 0;

    void reset() {
        comparisons = 0;
        swaps = 0;
        siftCalls = 0;
    }

    StageStats operator+(const StageStats& other) const {
        return {
            comparisons + other.comparisons,
            swaps + other.swaps,
            siftCalls + other.siftCalls
        };
    }
};

struct HeapSortStats {
    StageStats build;
    StageStats extract;

    StageStats total() const {
        return build + extract;
    }
};

struct ArraySignature {
    int n = 0;
    long long sum = 0;
    long long sumSq = 0;
};

ArraySignature calcSignature(const int* arr, int n) {
    ArraySignature sig;
    sig.n = n;
    for (int i = 0; i < n; ++i) {
        long long v = arr[i];
        sig.sum += v;
        sig.sumSq += v * v;
    }
    return sig;
}

bool isNonDecreasing(const int* arr, int n) {
    for (int i = 1; i < n; ++i) {
        if (arr[i - 1] > arr[i]) {
            return false;
        }
    }
    return true;
}

bool checkSignatureEqual(const ArraySignature& a, const ArraySignature& b) {
    return a.n == b.n && a.sum == b.sum && a.sumSq == b.sumSq;
}

void printArray(const int* arr, int n) {
    std::cout << "[";
    for (int i = 0; i < n; ++i) {
        if (i > 0) std::cout << ", ";
        std::cout << arr[i];
    }
    std::cout << "]";
}

// 1. Перевірка властивості максимальної купи (max-heap) для активної частини [0 .. heapSize - 1]
// Повертає -1, якщо властивість виконується, або індекс першого порушника.
int findMaxHeapViolation(const int* arr, int heapSize, long long* cmpCounter = nullptr) {
    if (heapSize <= 1) return -1;
    for (int i = 0; i < heapSize / 2; ++i) {
        int left = 2 * i + 1;
        int right = 2 * i + 2;

        if (left < heapSize) {
            if (cmpCounter) ++(*cmpCounter);
            if (arr[i] < arr[left]) {
                return i;
            }
        }
        if (right < heapSize) {
            if (cmpCounter) ++(*cmpCounter);
            if (arr[i] < arr[right]) {
                return i;
            }
        }
    }
    return -1;
}

bool isMaxHeap(const int* arr, int heapSize) {
    return findMaxHeapViolation(arr, heapSize, nullptr) == -1;
}

// 2. Відновлення властивості максимальної купи від вузла i в межах активного розміру heapSize
void siftDown(int* arr, int heapSize, int i, StageStats& stats) {
    ++stats.siftCalls;
    int current = i;
    while (true) {
        int left = 2 * current + 1;
        int right = 2 * current + 2;
        int largest = current;

        if (left < heapSize) {
            ++stats.comparisons;
            if (arr[left] > arr[largest]) {
                largest = left;
            }
        }
        if (right < heapSize) {
            ++stats.comparisons;
            if (arr[right] > arr[largest]) {
                largest = right;
            }
        }

        if (largest == current) {
            break;
        }

        int tmp = arr[current];
        arr[current] = arr[largest];
        arr[largest] = tmp;
        ++stats.swaps;

        current = largest;
    }
}

// 3. Побудова максимальної купи з довільного масиву довжини n
void buildMaxHeap(int* arr, int n, StageStats& buildStats, bool trace = false) {
    buildStats.reset();
    if (n <= 1) return;

    int startIdx = (n / 2) - 1;
    for (int i = startIdx; i >= 0; --i) {
        siftDown(arr, n, i, buildStats);
        if (trace && n <= 20) {
            std::cout << "  [Побудова] вузол=" << i
                      << " | активний розмір=" << n
                      << " | масив: ";
            printArray(arr, n);
            std::cout << "\n";
        }
    }
}

// 4. Етап вилучення / сортування з фіксацією статистики обох етапів
void heapSortWithStats(int* arr, int n, HeapSortStats& stats, bool traceBuild = false, bool traceFirst3Extracts = false) {
    stats.build.reset();
    stats.extract.reset();

    if (n <= 0) return;

    buildMaxHeap(arr, n, stats.build, traceBuild);

    int extractStep = 0;
    for (int heapSize = n; heapSize > 1; --heapSize) {
        int lastIdx = heapSize - 1;
        int extractedVal = arr[0];

        int tmp = arr[0];
        arr[0] = arr[lastIdx];
        arr[lastIdx] = tmp;
        ++stats.extract.swaps;

        int activeSize = lastIdx;
        if (activeSize > 1) {
            siftDown(arr, activeSize, 0, stats.extract);
        }

        ++extractStep;
        if (traceFirst3Extracts && extractStep <= 3) {
            bool activeHeapOk = isMaxHeap(arr, activeSize);
            bool sortedTailOk = isNonDecreasing(arr + activeSize, n - activeSize);
            std::cout << "  [Вилучення #" << extractStep << "] вилучено=" << extractedVal
                      << " | активна купа=" << activeSize
                      << " | max-heap=" << (activeHeapOk ? "OK" : "FAIL")
                      << " | хвіст=" << (sortedTailOk ? "OK" : "FAIL")
                      << " | масив: ";
            printArray(arr, n);
            std::cout << "\n";
        }
    }
}

void runHeapPropertyChecks() {
    std::cout << "=== ПЕРЕВІРКИ ВЛАСТИВОСТІ MAX-HEAP (3 ПРАВИЛЬНІ + 3 НЕПРАВИЛЬНІ) ===\n";

    int valid1[] = {9, 7, 8, 2, 3, 4, 1};
    int valid2[] = {50, 30, 40, 10, 20, 35, 25};
    int valid3[] = {15, 15, 10, 5, 12};

    int invalid1[] = {5, 8, 7, 2, 3};       // порушення у корені (індекс 0: 5 < 8)
    int invalid2[] = {20, 10, 15, 12, 8};   // порушення у вузлі 1 (10 < 12)
    int invalid3[] = {30, 25, 10, 5, 18};   // порушення у вузлі 2 (10 < 18)

    struct CaseItem {
        const char* label;
        const int* data;
        int size;
        bool expectValid;
    };

    CaseItem cases[] = {
        {"Правильна купа #1", valid1, 7, true},
        {"Правильна купа #2", valid2, 7, true},
        {"Правильна купа #3", valid3, 5, true},
        {"Порушена купа #1 (корінь < дитини)", invalid1, 5, false},
        {"Порушена купа #2 (вузол 1 < дитини)", invalid2, 5, false},
        {"Порушена купа #3 (вузол 2 < дитини)", invalid3, 5, false},
    };

    int passed = 0;
    for (const auto& c : cases) {
        int viol = findMaxHeapViolation(c.data, c.size, nullptr);
        bool actualValid = (viol == -1);
        bool ok = (actualValid == c.expectValid);
        if (ok) ++passed;

        std::cout << "  " << c.label << ": " << (ok ? "PASS" : "FAIL");
        if (actualValid) {
            std::cout << " (коректна max-heap)\n";
        } else {
            std::cout << " (порушення на індексі " << viol << ")\n";
        }
    }
    std::cout << "Успішних перевірок властивості купи: " << passed << " з 6\n\n";
}

void runAutomatedTests() {
    std::cout << "=== АВТОМАТИЗОВАНІ ТЕСТИ (14 НАБОРІВ) ===\n";

    struct TestCase {
        int id;
        const char* name;
        const int* data;
        int n;
    };

    int t1[]  = {};
    int t2[]  = {7};
    int t3[]  = {1, 2};
    int t4[]  = {2, 1};
    int t5[]  = {1, 2, 3, 4, 5};
    int t6[]  = {5, 4, 3, 2, 1};
    int t7[]  = {4, 1, 5, 2, 3};
    int t8[]  = {3, 1, 3, 2, 1};
    int t9[]  = {0, -5, 8, -2, 4};
    int t10[] = {6, 6, 6, 6};
    int t11[] = {9, -1, 9, 0, -1, 5};
    int t12[] = {10, 7, 8, 4, 2, 1, 3};
    int t13[] = {-12, -4, -12, 0, 7, -4, 3};          // власний #1: від'ємні значення та дублікати
    int t14[] = {85, 12, 64, 3, 99, 41, 28, 73, 50};   // власний #2: ширший діапазон значень

    TestCase tests[] = {
        {1,  "порожній масив", t1, 0},
        {2,  "один елемент", t2, 1},
        {3,  "два впорядковані", t3, 2},
        {4,  "два переставлені", t4, 2},
        {5,  "зростання", t5, 5},
        {6,  "спадання", t6, 5},
        {7,  "змішаний порядок", t7, 5},
        {8,  "повторення", t8, 5},
        {9,  "від'ємні числа", t9, 5},
        {10, "усі рівні", t10, 4},
        {11, "комбінований випадок", t11, 6},
        {12, "неповний останній рівень", t12, 7},
        {13, "власний: від'ємні та дублі", t13, 7},
        {14, "власний: ширший діапазон", t14, 9},
    };

    int sortPassed = 0;
    int heapBuildChecks = 0;
    int heapBuildPassed = 0;

    for (const auto& tc : tests) {
        ArraySignature before = calcSignature(tc.data, tc.n);
        int* buf = (tc.n > 0) ? new int[tc.n] : nullptr;
        for (int i = 0; i < tc.n; ++i) buf[i] = tc.data[i];

        StageStats bStats;
        buildMaxHeap(buf, tc.n, bStats, false);

        bool heapOk = true;
        if (tc.n >= 5) {
            ++heapBuildChecks;
            heapOk = isMaxHeap(buf, tc.n);
            if (heapOk) ++heapBuildPassed;
        }

        for (int i = 0; i < tc.n; ++i) buf[i] = tc.data[i];
        HeapSortStats fullStats;
        heapSortWithStats(buf, tc.n, fullStats, false, false);

        ArraySignature after = calcSignature(buf, tc.n);
        bool orderOk = isNonDecreasing(buf, tc.n);
        bool sigOk = checkSignatureEqual(before, after);
        bool ok = orderOk && sigOk && heapOk;
        if (ok) ++sortPassed;

        std::cout << "  Тест #" << std::setw(2) << tc.id << " (" << tc.name << "): "
                  << (ok ? "PASS" : "FAIL")
                  << " |
