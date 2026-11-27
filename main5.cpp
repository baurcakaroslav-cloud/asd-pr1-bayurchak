#include <iostream>
#include <iomanip>
#include <cstring>

struct SortStats {
    long long comparisons = 0;
    long long swaps = 0;
    long long calls = 0;
    int maxDepth = 0;
};

struct ArrayMetrics {
    int count = 0;
    long long sum = 0;
    unsigned long long sumSquares = 0;
};

ArrayMetrics calcMetrics(const int* arr, int n) {
    ArrayMetrics m;
    m.count = n;
    for (int i = 0; i < n; ++i) {
        long long val = arr[i];
        m.sum += val;
        long long sq = val * val;
        m.sumSquares += static_cast<unsigned long long>(sq);
    }
    return m;
}

bool isNonDecreasing(const int* arr, int n) {
    for (int i = 1; i < n; ++i) {
        if (arr[i - 1] > arr[i]) {
            return false;
        }
    }
    return true;
}

bool arraysEqual(const int* a, const int* b, int n) {
    for (int i = 0; i < n; ++i) {
        if (a[i] != b[i]) {
            return false;
        }
    }
    return true;
}

void copyArray(const int* src, int* dst, int n) {
    for (int i = 0; i < n; ++i) {
        dst[i] = src[i];
    }
}

void swapElements(int& a, int& b, SortStats& stats) {
    if (&a == &b) {
        return;
    }
    int temp = a;
    a = b;
    b = temp;
    ++stats.swaps;
}

void printRange(const int* arr, int left, int right) {
    std::cout << "[";
    for (int i = left; i <= right; ++i) {
        std::cout << arr[i];
        if (i < right) {
            std::cout << ", ";
        }
    }
    std::cout << "]";
}

void printFullArray(const int* arr, int n) {
    std::cout << "[";
    for (int i = 0; i < n; ++i) {
        std::cout << arr[i];
        if (i + 1 < n) {
            std::cout << ", ";
        }
    }
    std::cout << "]";
}

// ============================================================================
// ЗАВДАННЯ 2–3. СХЕМА ЛОМУТО (опорний елемент на правій межі)
// ============================================================================
int partitionLomuto(int* arr, int left, int right, SortStats& stats) {
    int pivot = arr[right];
    int i = left - 1;
    for (int j = left; j < right; ++j) {
        ++stats.comparisons;
        if (arr[j] <= pivot) {
            ++i;
            swapElements(arr[i], arr[j], stats);
        }
    }
    swapElements(arr[i + 1], arr[right], stats);
    return i + 1;
}

void quickSortLomutoRec(int* arr, int totalN, int left, int right, int depth,
                        bool traceEnabled, SortStats& stats) {
    if (left >= right) {
        return;
    }

    ++stats.calls;
    if (depth > stats.maxDepth) {
        stats.maxDepth = depth;
    }

    int pivotValue = arr[right];
    bool doTrace = traceEnabled && (totalN <= 30);

    if (doTrace) {
        std::cout << "  [Ломуто] Виклик #" << stats.calls
                  << " | глибина: " << depth
                  << " | межі: [" << left << ".." << right << "]"
                  << " | pivot: " << pivotValue << "\n";
    }

    int pivotIdx = partitionLomuto(arr, left, right, stats);

    int leftL = left;
    int leftR = pivotIdx - 1;
    int rightL = pivotIdx + 1;
    int rightR = right;

    if (leftL < leftR && (leftR - leftL) >= (right - left)) {
        std::cerr << "  [ПОМИЛКА] Лівий діапазон Ломуто не зменшився!\n";
    }
    if (rightL < rightR && (rightR - rightL) >= (right - left)) {
        std::cerr << "  [ПОМИЛКА] Правий діапазон Ломуто не зменшився!\n";
    }

    if (doTrace) {
        std::cout << "    Після розбиття (pivotIdx=" << pivotIdx << "): ";
        printRange(arr, left, right);
        std::cout << " | Наступні: ";
        if (leftL < leftR) {
            std::cout << "[" << leftL << ".." << leftR << "] ";
        } else {
            std::cout << "[порожньо] ";
        }
        if (rightL < rightR) {
            std::cout << "та [" << rightL << ".." << rightR << "]";
        } else {
            std::cout << "та [порожньо]";
        }
        std::cout << "\n";
    }

    if (leftL < leftR) {
        quickSortLomutoRec(arr, totalN, leftL, leftR, depth + 1, traceEnabled, stats);
    }
    if (rightL < rightR) {
        quickSortLomutoRec(arr, totalN, rightL, rightR, depth + 1, traceEnabled, stats);
    }
}

void quickSortLomuto(int* arr, int n, bool traceEnabled, SortStats& stats) {
    stats = SortStats{};
    if (n > 1) {
        quickSortLomutoRec(arr, n, 0, n - 1, 1, traceEnabled, stats);
    }
}

// ============================================================================
// ЗАВДАННЯ 4–5. СХЕМА ГОАРА (опорне значення з середини ділянки)
// ============================================================================
int partitionHoare(int* arr, int left, int right, SortStats& stats,
                   int& finalI, int& finalJ) {
    int pivot = arr[left + (right - left) / 2];
    int i = left - 1;
    int j = right + 1;

    while (true) {
        do {
            ++i;
            ++stats.comparisons;
        } while (arr[i] < pivot);

        do {
            --j;
            ++stats.comparisons;
        } while (arr[j] > pivot);

        if (i >= j) {
            finalI = i;
            finalJ = j;
            return j;
        }
        swapElements(arr[i], arr[j], stats);
    }
}

void quickSortHoareRec(int* arr, int totalN, int left, int right, int depth,
                       bool traceEnabled, SortStats& stats) {
    if (left >= right) {
        return;
    }

    ++stats.calls;
    if (depth > stats.maxDepth) {
        stats.maxDepth = depth;
    }

    int pivotValue = arr[left + (right - left) / 2];
    bool doTrace = traceEnabled && (totalN <= 30);

    if (doTrace) {
        std::cout << "  [Гоар]   Виклик #" << stats.calls
                  << " | глибина: " << depth
                  << " | межі: [" << left << ".." << right << "]"
                  << " | pivot: " << pivotValue << "\n";
    }

    int finalI = 0;
    int finalJ = 0;
    int boundary = partitionHoare(arr, left, right, stats, finalI, finalJ);

    int leftL = left;
    int leftR = boundary;
    int rightL = boundary + 1;
    int rightR = right;

    if (leftL < leftR && (leftR - leftL) >= (right - left)) {
        std::cerr << "  [ПОМИЛКА] Лівий діапазон Гоара не зменшився!\n";
    }
    if (rightL < rightR && (rightR - rightL) >= (right - left)) {
        std::cerr << "  [ПОМИЛКА] Правий діапазон Гоара не зменшився!\n";
    }

    if (doTrace) {
        std::cout << "    Після розбиття (межа=" << boundary
                  << ", i=" << finalI << ", j=" << finalJ << "): ";
        printRange(arr, left, right);
        std::cout << " | Наступні: ";
        if (leftL < leftR) {
            std::cout << "[" << leftL << ".." << leftR << "] ";
        } else {
            std::cout << "[порожньо] ";
        }
        if (rightL < rightR) {
            std::cout << "та [" << rightL << ".." << rightR << "]";
        } else {
            std::cout << "та [порожньо]";
        }
        std::cout << "\n";
    }

    if (leftL < leftR) {
        quickSortHoareRec(arr, totalN, leftL, leftR, depth + 1, traceEnabled, stats);
    }
    if (rightL < rightR) {
        quickSortHoareRec(arr, totalN, rightL, rightR, depth + 1, traceEnabled, stats);
    }
}

void quickSortHoare(int* arr, int n, bool traceEnabled, SortStats& stats) {
    stats = SortStats{};
    if (n > 1) {
        quickSortHoareRec(arr, n, 0, n - 1, 1, traceEnabled, stats);
    }
}

// ============================================================================
// ЗАВДАННЯ 7. АВТОМАТИЧНА ПЕРЕВІРКА ПОРЯДКУ ТА СКЛАДУ МАСИВУ
// ============================================================================
bool verifyResult(const char* schemeName, const int* original, int n, const int* sorted) {
    bool orderOk = isNonDecreasing(sorted, n);
    ArrayMetrics before = calcMetrics(original, n);
    ArrayMetrics after = calcMetrics(sorted, n);

    bool metricsOk = (before.count == after.count) &&
                      (before.sum == after.sum) &&
                      (before.sumSquares == after.sumSquares);

    bool passed = orderOk && metricsOk;
    std::cout << "  [" << schemeName << "] Статус: "
              << (passed ? "PASS" : "FAIL")
              << " | порядок=" << (orderOk ? "OK" : "ERR")
              << " | n=" << after.count
              << " | sum=" << after.sum
              << " | sumSq=" << after.sumSquares << "\n";
    return passed;
}

// ============================================================================
// ПЕРЕВІРКИ ФУНКЦІЙ РОЗБИТТЯ (8 ПЕРЕВІРОК: ПО 4 НА КОЖНУ СХЕМУ)
// ============================================================================
bool checkLomutoPartitionRule(const int* arr, int left, int right, int p) {
    if (p < left || p > right) {
        return false;
    }
    int pivot = arr[p];
    for (int k = left; k < p; ++k) {
        if (arr[k] > pivot) {
            return false;
        }
    }
    for (int k = p + 1; k <= right; ++k) {
        if (arr[k] <= pivot) {
            return false;
        }
    }
    return true;
}

bool checkHoarePartitionRule(const int* arr, int left, int right, int p) {
    if (p < left || p >= right) {
        return false;
    }
    int maxLeft = arr[left];
    for (int k = left + 1; k <= p; ++k) {
        if (arr[k] > maxLeft) {
            maxLeft = arr[k];
        }
    }
    int minRight = arr[p + 1];
    for (int k = p + 2; k <= right; ++k) {
        if (arr[k] < minRight) {
            minRight = arr[k];
        }
    }
    return maxLeft <= minRight;
}

int runPartitionChecks() {
    int passedCount = 0;
    SortStats dummy;
    int fi = 0, fj = 0;

    std::cout << "\n=== ПЕРЕВІРКИ ФУНКЦІЙ РОЗБИТТЯ (8 ПЕРЕВІРОК) ===\n";

    // Ломуто 1: опорний елемент найменший (останній елемент = 2)
    {
        int a[] = {10, 5, 8, 2};
        int p = partitionLomuto(a, 0, 3, dummy);
        bool ok = checkLomutoPartitionRule(a, 0, 3, p) && (p == 0);
        if (ok) ++passedCount;
        std::cout << "  Ломуто #1 (pivot найменший): " << (ok ? "PASS" : "FAIL")
                  << " | p=" << p << " -> ";
        printFullArray(a, 4);
        std::cout << "\n";
    }

    // Ломуто 2: опорний елемент найбільший (останній елемент = 9)
    {
        int a[] = {4, 1, 7, 9};
        int p = partitionLomuto(a, 0, 3, dummy);
        bool ok = checkLomutoPartitionRule(a, 0, 3, p) && (p == 3);
        if (ok) ++passedCount;
        std::cout << "  Ломуто #2 (pivot найбільший): " << (ok ? "PASS" : "FAIL")
                  << " | p=" << p << " -> ";
        printFullArray(a, 4);
        std::cout << "\n";
    }

    // Ломуто 3: опорний елемент середній за значенням (останній елемент = 5)
    {
        int a[] = {9, 1, 5};
        int p = partitionLomuto(a, 0, 2, dummy);
        bool ok = checkLomutoPartitionRule(a, 0, 2, p) && (p == 1);
        if (ok) ++passedCount;
        std::cout << "  Ломуто #3 (pivot середній):   " << (ok ? "PASS" : "FAIL")
                  << " | p=" << p << " -> ";
        printFullArray(a, 3);
        std::cout << "\n";
    }

    // Ломуто 4: опорний елемент повторюється (останній елемент = 5)
    {
        int a[] = {5, 2, 5, 1, 5};
        int p = partitionLomuto(a, 0, 4, dummy);
        bool ok = checkLomutoPartitionRule(a, 0, 4, p);
        if (ok) ++passedCount;
        std::cout << "  Ломуто #4 (pivot повторюється):" << (ok ? "PASS" : "FAIL")
                  << " | p=" << p << " -> ";
        printFullArray(a, 5);
        std::cout << "\n";
    }

    // Гоар 1: два елементи у зворотному порядку
    {
        int a[] = {2, 1};
        int p = partitionHoare(a, 0, 1, dummy, fi, fj);
        bool ok = checkHoarePartitionRule(a, 0, 1, p);
        if (ok) ++passedCount;
        std::cout << "  Гоар #1 (2 елементи [2, 1]):   " << (ok ? "PASS" : "FAIL")
                  << " | p=" << p << ", i=" << fi << ", j=" << fj << " -> ";
        printFullArray(a, 2);
        std::cout << "\n";
    }

    // Гоар 2: два елементи у зростаючому порядку
    {
        int a[] = {1, 2};
        int p = partitionHoare(a, 0, 1, dummy, fi, fj);
        bool ok = checkHoarePartitionRule(a, 0, 1, p);
        if (ok) ++passedCount;
        std::cout << "  Гоар #2 (2 елементи [1, 2]):   " << (ok ? "PASS" : "FAIL")
                  << " | p=" << p << ", i=" << fi << ", j=" << fj << " -> ";
        printFullArray(a, 2);
        std::cout << "\n";
    }

    // Гоар 3: усі елементи однакові
    {
        int a[] = {6, 6, 6, 6};
        int p = partitionHoare(a, 0, 3, dummy, fi, fj);
        bool ok = checkHoarePartitionRule(a, 0, 3, p);
        if (ok) ++passedCount;
        std::cout << "  Гоар #3 (усі рівні [6..6]):   " << (ok ? "PASS" : "FAIL")
                  << " | p=" << p << ", i=" << fi << ", j=" << fj << " -> ";
        printFullArray(a, 4);
        std::cout << "\n";
    }

    // Гоар 4: змішаний масив із повтореннями
    {
        int a[] = {8, 3, 5, 3, 9, 1, 5};
        int p = partitionHoare(a, 0, 6, dummy, fi, fj);
        bool ok = checkHoarePartitionRule(a, 0, 6, p);
        if (ok) ++passedCount;
        std::cout << "  Гоар #4 (змішаний з повторам.):" << (ok ? "PASS" : "FAIL")
                  << " | p=" << p << ", i=" << fi << ", j=" << fj << " -> ";
        printFullArray(a, 7);
        std::cout << "\n";
    }

    std::cout << "Успішних перевірок розбиття: " << passedCount << " з 8\n";
    return passedCount;
}

// ============================================================================
// АВТОМАТИЗОВАНІ ТЕСТИ СОРТУВАННЯ (12 БАЗОВИХ/ГРАНИЧНИХ + 2 ВЛАСНІ = 14 НАБОРІВ)
// ============================================================================
struct TestCase {
    int id;
    const char* purpose;
    int n;
    const int* data;
    const int* expected;
};

int runAutomatedSortTests() {
    const int t1_in[]  = {};
    const int t1_exp[] = {};

    const int t2_in[]  = {7};
    const int t2_exp[] = {7};

    const int t3_in[]  = {1, 2};
    const int t3_exp[] = {1, 2};

    const int t4_in[]  = {2, 1};
    const int t4_exp[] = {1, 2};

    const int t5_in[]  = {1, 2, 3, 4, 5};
    const int t5_exp[] = {1, 2, 3, 4, 5};

    const int t6_in[]  = {5, 4, 3, 2, 1};
    const int t6_exp[] = {1, 2, 3, 4, 5};

    const int t7_in[]  = {4, 1, 5, 2, 3};
    const int t7_exp[] = {1, 2, 3, 4, 5};

    const int t8_in[]  = {3, 1, 3, 2, 1};
    const int t8_exp[] = {1, 1, 2, 3, 3};

    const int t9_in[]  = {0, -5, 8, -2, 4};
    const int t9_exp[] = {-5, -2, 0, 4, 8};

    const int t10_in[] = {6, 6, 6, 6};
    const int t10_exp[] = {6, 6, 6, 6};

    const int t11_in[] = {9, -1, 9, 0, -1, 5};
    const int t11_exp[] = {-1, -1, 0, 5, 9, 9};

    const int t12_in[] = {3, 2, 1, 4, 5};
    const int t12_exp[] = {1, 2, 3, 4, 5};

    // Два власні тести
    const int t13_in[] = {-10, -10, -3, 0, -7, -3, 2};
    const int t13_exp[] = {-10, -10, -7, -3, -3, 0, 2};

    const int t14_in[] = {100, 1, 50, 25, 75, 2, 99, 50};
    const int t14_exp[] = {1, 2, 25, 50, 50, 75, 99, 100};

    TestCase tests[14] = {
        {1,  "порожній масив",           0, t1_in,  t1_exp},
        {2,  "один елемент",             1, t2_in,  t2_exp},
        {3,  "два впорядковані",         2, t3_in,  t3_exp},
        {4,  "два переставлені",         2, t4_in,  t4_exp},
        {5,  "зростання",                5, t5_in,  t5_exp},
        {6,  "спадання",                 5, t6_in,  t6_exp},
        {7,  "змішаний порядок",         5, t7_in,  t7_exp},
        {8,  "повторення",               5, t8_in,  t8_exp},
        {9,  "від'ємні числа",           5, t9_in,  t9_exp},
        {10, "усі рівні",                4, t10_in, t10_exp},
        {11, "комбінований випадок",     6, t11_in, t11_exp},
        {12, "частково впорядкований",   5, t12_in, t12_exp},
        {13, "власний: від'ємні дублі",  7, t13_in, t13_exp},
        {14, "власний: ширший діапазон", 8, t14_in, t14_exp}
    };

    int passedSorts = 0;
    std::cout << "\n=== АВТОМАТИЗОВАНІ ТЕСТИ СОРТУВАННЯ (14 НАБОРІВ × 2 СХЕМИ = 28 ЗАПУСКІВ) ===\n";

    for (int idx = 0; idx < 14; ++idx) {
        const TestCase& tc = tests[idx];
        std::cout << "Тест #" << tc.id << " (" << tc.purpose << "):\n";

        int* workL = (tc.n > 0) ? new int[tc.n] : nullptr;
        int* workH = (tc.n > 0) ? new int[tc.n] : nullptr;
        if (tc.n > 0) {
            copyArray(tc.data, workL, tc.n);
            copyArray(tc.data, workH, tc.n);
        }

        SortStats stL, stH;
        quickSortLomuto(workL, tc.n, false, stL);
        quickSortHoare(workH, tc.n, false, stH);

        bool okL = verifyResult("Ломуто", tc.data, tc.n, workL) &&
                   (tc.n == 0 || arraysEqual(workL, tc.expected, tc.n));
        bool okH = verifyResult("Гоар  ", tc.data, tc.n, workH) &&
                   (tc.n == 0 || arraysEqual(workH, tc.expected, tc.n));

        if (okL) ++passedSorts;
        if (okH) ++passedSorts;

        delete[] workL;
        delete[] workH;
    }

    std::cout << "Успішних сортувань: " << passedSorts << " з 28\n";
    return passedSorts;
}

// ============================================================================
// ЗАВДАННЯ 8. ПОРІВНЯЛЬНИЙ ЕКСПЕРИМЕНТ (12 ПАР ЗАПУСКІВ)
// ============================================================================
enum DataKind {
    KIND_ASC = 0,
    KIND_DESC = 1,
    KIND_MIXED = 2,
    KIND_DUPLICATES = 3
};

const char* dataKindName(DataKind kind) {
    switch (kind) {
        case KIND_ASC:        return "Зростання";
        case KIND_DESC:       return "Спадання";
        case KIND_MIXED:      return "Змішаний";
        case KIND_DUPLICATES: return "Багато однакових";
        default:              return "Невідомо";
    }
}

void generateDataset(int* arr, int n, DataKind kind, unsigned int seed = 20261127u) {
    unsigned int state = seed;
    for (int i = 0; i < n; ++i) {
        if (kind == KIND_ASC) {
            arr[i] = i;
        } else if (kind == KIND_DESC) {
            arr[i] = n - i;
        } else if (kind == KIND_MIXED) {
            state = state * 1664525u + 1013904223u;
            arr[i] = static_cast<int>((state >> 16) % 100000u);
        } else if (kind == KIND_DUPLICATES) {
            arr[i] = (i % 8 == 0) ? (i % 10) : 77;
        }
    }
}

void runComparativeExperiment() {
    const int sizes[3] = {100, 1000, 10000};
    const DataKind kinds[4] = {KIND_ASC, KIND_DESC, KIND_MIXED, KIND_DUPLICATES};

    std::cout << "\n=== ПОРІВНЯЛЬНИЙ ЕКСПЕРИМЕНТ (12 ПАР ЗАПУСКІВ) ===\n";
    std::cout << std::left
              << std::setw(8)  << "Розмір"
              << std::setw(20) << "Набір даних"
              << std::setw(34) << "Ломуто (порівн / обміни / глиб)"
              << std::setw(34) << "Гоар (порівн / обміни / глиб)" << "\n";
    std::cout << std::string(96, '-') << "\n";

    int pairIndex = 0;
    for (int s = 0; s < 3; ++s) {
        int n = sizes[s];
        for (int k = 0; k < 4; ++k) {
            ++pairIndex;
            DataKind kind = kinds[k];

            int* original = new int[n];
            int* bufL = new int[n];
            int* bufH = new int[n];

            // Однаковий генератор та ідентичний буфер перед кожним запуском
            generateDataset(original, n, kind, 20261127u + static_cast<unsigned int>(n));
            copyArray(original, bufL, n);
            copyArray(original, bufH, n);

            SortStats stL, stH;
            quickSortLomuto(bufL, n, false, stL);
            quickSortHoare(bufH, n, false, stH);

            bool okL = verifyResult("Ломуто", original, n, bufL);
            bool okH = verifyResult("Гоар  ", original, n, bufH);
            if (!okL || !okH) {
                std::cerr << "Помилка перевірки в експерименті #" << pairIndex << "\n";
            }

            std::cout << std::left
                      << std::setw(8)  << n
                      << std::setw(20) << dataKindName(kind);

            char cellL[64];
            char cellH[64];
            std::snprintf(cellL, sizeof(cellL), "%lld / %lld / %d",
                          stL.comparisons, stL.swaps, stL.maxDepth);
            std::snprintf(cellH, sizeof(cellH), "%lld / %lld / %d",
                          stH.comparisons, stH.swaps, stH.maxDepth);

            std::cout << std::setw(34) << cellL
                      << std::setw(34) << cellH << "\n";

            delete[] original;
            delete[] bufL;
            delete[] bufH;
        }
    }
}

// ============================================================================
// ГОЛОВНА ПРОГРАМА: ТРАСУВАННЯ ТА ІНДИВІДУАЛЬНИЙ ВАРІАНТ
// ============================================================================
int main() {
    std::cout << "Практична робота 5: Реалізація QuickSort (Ломуто та Гоар)\n";

    // Приклад трасування для невеликого масиву (<= 30 елементів)
    const int traceSample[] = {24, 5, 18, 2, 29, 11, 7, 21, 1, 16};
    const int traceN = sizeof(traceSample) / sizeof(traceSample[0]);

    std::cout << "\n=== ТРАСУВАННЯ РЕКУРСІЇ НА МАСИВІ З " << traceN << " ЕЛЕМЕНТІВ ===\n";
    std::cout << "Початковий масив: ";
    printFullArray(traceSample, traceN);
    std::cout << "\n";

    int* trL = new int[traceN];
    int* trH = new int[traceN];
    copyArray(traceSample, trL, traceN);
    copyArray(traceSample, trH, traceN);

    SortStats trStatsL, trStatsH;
    quickSortLomuto(trL, traceN, true, trStatsL);
    verifyResult("Ломуто (трасування)", traceSample, traceN, trL);

    quickSortHoare(trH, traceN, true, trStatsH);
    verifyResult("Гоар (трасування)  ", traceSample, traceN, trH);

    delete[] trL;
    delete[] trH;

    // Перевірки розбиття та автоматизовані тести
    int partOk = runPartitionChecks();
    int sortOk = runAutomatedSortTests();

    if (partOk < 8 || sortOk < 28) {
        std::cerr << "\nУвага: мінімальний поріг тестів не досягнуто!\n";
    }

    // Порівняльний експеримент (12 пар запусків)
    runComparativeExperiment();

    return 0;
}
