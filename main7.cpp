#include <iostream>
#include <iomanip>
#include <cstring>
#include <climits>

struct Stats {
    long long comparisons = 0; // зіставлення значень лівої і правої частин
    long long auxWrites = 0;   // записи у допоміжний масив
    long long mainWrites = 0;  // записи в основний масив
    long long merges = 0;      // кількість завершених злиттів
    int maxDepth = 0;          // максимальна глибина рекурсії

    long long totalWrites() const {
        return auxWrites + mainWrites;
    }

    void reset() {
        comparisons = 0;
        auxWrites = 0;
        mainWrites = 0;
        merges = 0;
        maxDepth = 0;
    }
};

void printSlice(const int* arr, int left, int right) {
    std::cout << "[";
    for (int i = left; i < right; ++i) {
        if (i > left) std::cout << ", ";
        std::cout << arr[i];
    }
    std::cout << "]";
}

void printIndent(int depth) {
    for (int i = 0; i < depth; ++i) {
        std::cout << "  ";
    }
}

// 1. Злиття двох сусідніх упорядкованих частин [left, mid) та [mid, right)
void mergeParts(int* arr, int* temp, int left, int mid, int right,
                Stats& st, bool trace = false, int depth = 0) {
    if (left >= mid || mid >= right) {
        return;
    }

    if (trace) {
        printIndent(depth);
        std::cout << "MERGE (глибина " << depth << "): L=";
        printSlice(arr, left, mid);
        std::cout << " + R=";
        printSlice(arr, mid, right);
    }

    int i = left;
    int j = mid;
    int k = left;

    while (i < mid && j < right) {
        ++st.comparisons;
        if (arr[i] <= arr[j]) {
            temp[k++] = arr[i++];
        } else {
            temp[k++] = arr[j++];
        }
        ++st.auxWrites;
    }

    while (i < mid) {
        temp[k++] = arr[i++];
        ++st.auxWrites;
    }

    while (j < right) {
        temp[k++] = arr[j++];
        ++st.auxWrites;
    }

    for (int idx = left; idx < right; ++idx) {
        arr[idx] = temp[idx];
        ++st.mainWrites;
    }

    ++st.merges;

    if (trace) {
        std::cout << " -> ";
        printSlice(arr, left, right);
        std::cout << "\n";
    }
}

// 3. Рекурсивний поділ [left, right)
void mergeSortRecursive(int* arr, int* temp, int left, int right,
                        Stats& st, bool trace, int depth) {
    if (depth > st.maxDepth) {
        st.maxDepth = depth;
    }

    if (right - left <= 1) {
        if (trace) {
            printIndent(depth);
            std::cout << "BASE (глибина " << depth << ", межі [" << left << ", " << right << ")): ";
            printSlice(arr, left, right);
            std::cout << "\n";
        }
        return;
    }

    int mid = left + (right - left) / 2;

    if (trace) {
        printIndent(depth);
        std::cout << "SPLIT (глибина " << depth << ", межі [" << left << ", " << right
                  << "), mid=" << mid << "): ";
        printSlice(arr, left, right);
        std::cout << "\n";
    }

    mergeSortRecursive(arr, temp, left, mid, st, trace, depth + 1);
    mergeSortRecursive(arr, temp, mid, right, st, trace, depth + 1);
    mergeParts(arr, temp, left, mid, right, st, trace, depth);
}

void mergeSort(int* arr, int n, int* temp, Stats& st, bool trace = false) {
    st.reset();
    if (n <= 1) {
        return;
    }
    mergeSortRecursive(arr, temp, 0, n, st, trace, 1);
}

// 6. Автоматична перевірка результату (неспадність + n, сума, сума квадратів)
bool checkSortedArray(const int* original, const int* sorted, int n) {
    for (int i = 1; i < n; ++i) {
        if (sorted[i - 1] > sorted[i]) {
            std::cout << "  [ПОМИЛКА] Порушено порядок на позиції " << i << "\n";
            return false;
        }
    }

    long long sumOrig = 0, sumSorted = 0;
    long double sqOrig = 0.0L, sqSorted = 0.0L;

    for (int i = 0; i < n; ++i) {
        long long x = original[i];
        long long y = sorted[i];
        sumOrig += x;
        sumSorted += y;
        sqOrig += static_cast<long double>(x) * static_cast<long double>(x);
        sqSorted += static_cast<long double>(y) * static_cast<long double>(y);
    }

    if (sumOrig != sumSorted) {
        std::cout << "  [ПОМИЛКА] Сума елементів змінилася (" << sumOrig << " -> " << sumSorted << ")\n";
        return false;
    }

    long double diff = sqOrig - sqSorted;
    if (diff < 0) diff = -diff;
    if (diff > 1e6L) {
        std::cout << "  [ПОМИЛКА] Сума квадратів елементів не збігається\n";
        return false;
    }

    return true;
}

bool sameArray(const int* a, const int* b, int n) {
    for (int i = 0; i < n; ++i) {
        if (a[i] != b[i]) return false;
    }
    return true;
}

// 2. Окремі тести злиття (6 перевірок)
bool runMergeTests(int* temp) {
    struct TestCase {
        const char* title;
        int n;
        int mid;
        int input[10];
        int expected[10];
    };

    TestCase cases[6] = {
        {"Однакова довжина (3 + 3)", 6, 3, {2, 6, 9, 1, 4, 8}, {1, 2, 4, 6, 8, 9}},
        {"Різна довжина (2 + 4)", 6, 2, {5, 12, 1, 3, 7, 10}, {1, 3, 5, 7, 10, 12}},
        {"З порожньою правою частиною", 4, 4, {-4, 0, 3, 7}, {-4, 0, 3, 7}},
        {"З повтореннями", 6, 3, {3, 5, 5, 3, 5, 8}, {3, 3, 5, 5, 5, 8}},
        {"Від'ємні числа", 6, 3, {-15, -8, -1, -12, -5, 2}, {-15, -12, -8, -5, -1, 2}},
        {"Ліва частина більша за праву", 6, 3, {10, 20, 30, 1, 2, 3}, {1, 2, 3, 10, 20, 30}}
    };

    bool okAll = true;
    for (int c = 0; c < 6; ++c) {
        int buf[10];
        std::memcpy(buf, cases[c].input, sizeof(int) * cases[c].n);
        Stats st;
        mergeParts(buf, temp, 0, cases[c].mid, cases[c].n, st, false, 0);

        bool pass = sameArray(buf, cases[c].expected, cases[c].n) &&
                    checkSortedArray(cases[c].input, buf, cases[c].n);
        std::cout << "  Злиття #" << (c + 1) << " (" << cases[c].title << "): "
                  << (pass ? "OK" : "FAIL") << "\n";
        if (!pass) {
            okAll = false;
            std::cout << "    Вхід: "; printSlice(cases[c].input, 0, cases[c].n); std::cout << "\n";
            std::cout << "    Факт: "; printSlice(buf, 0, cases[c].n); std::cout << "\n";
        }
    }
    return okAll;
}

// 7. Автоматизовані тести MergeSort (12 обов'язкових + 2 власні = 14)
bool runMergeSortTests(int* temp) {
    struct CaseItem {
        int num;
        const char* desc;
        int n;
        int in[16];
        int exp[16];
    };

    CaseItem items[14] = {
        {1,  "Порожній масив",          0, {}, {}},
        {2,  "Один елемент",            1, {7}, {7}},
        {3,  "Два впорядковані",        2, {1, 2}, {1, 2}},
        {4,  "Два переставлені",        2, {2, 1}, {1, 2}},
        {5,  "Зростання",               5, {1, 2, 3, 4, 5}, {1, 2, 3, 4, 5}},
        {6,  "Спадання",                5, {5, 4, 3, 2, 1}, {1, 2, 3, 4, 5}},
        {7,  "Непарна довжина",         5, {4, 1, 5, 2, 3}, {1, 2, 3, 4, 5}},
        {8,  "Повторення",              5, {3, 1, 3, 2, 1}, {1, 1, 2, 3, 3}},
        {9,  "Від'ємні числа",          5, {0, -5, 8, -2, 4}, {-5, -2, 0, 4, 8}},
        {10, "Усі рівні",               4, {6, 6, 6, 6}, {6, 6, 6, 6}},
        {11, "Комбінований випадок",    7, {9, -1, 9, 0, -1, 5, 9}, {-1, -1, 0, 5, 9, 9, 9}},
        {12, "Кілька рівнів поділу",    7, {8, 3, 7, 2, 6, 1, 5}, {1, 2, 3, 5, 6, 7, 8}},
        {13, "Власний: 15 елементів",   15,
            {12, -4, 19, 0, 7, -11, 25, 3, 9, -4, 16, 1, 6, 14, 2},
            {-11, -4, -4, 0, 1, 2, 3, 6, 7, 9, 12, 14, 16, 19, 25}},
        {14, "Власний: крайні int",     5,
            {INT_MAX, 0, INT_MIN, INT_MAX, INT_MIN},
            {INT_MIN, INT_MIN, 0, INT_MAX, INT_MAX}}
    };

    bool okAll = true;
    for (int t = 0; t < 14; ++t) {
        int work[16];
        std::memcpy(work, items[t].in, sizeof(int) * items[t].n);
        Stats st;
        mergeSort(work, items[t].n, temp, st, false);

        bool pass = sameArray(work, items[t].exp, items[t].n) &&
                    checkSortedArray(items[t].in, work, items[t].n);
        std::cout << "  Тест #" << std::setw(2) << items[t].num << " ("
                  << std::left << std::setw(25) << items[t].desc << std::right << "): "
                  << (pass ? "PASS" : "FAIL")
                  << " | пор=" << st.comparisons
                  << ", зап=" << st.totalWrites()
                  << ", злит=" << st.merges
                  << ", гл=" << st.maxDepth << "\n";

        if (!pass) {
            okAll = false;
            std::cout << "    Вхід: "; printSlice(items[t].in, 0, items[t].n); std::cout << "\n";
            std::cout << "    Факт: "; printSlice(work, 0, items[t].n); std::cout << "\n";
        }
    }
    return okAll;
}

// 8. QuickSort і HeapSort для порівняння з MergeSort
int partitionQuick(int* arr, int low, int high, Stats& st) {
    int pivot = arr[high];
    int i = low - 1;
    for (int j = low; j < high; ++j) {
        ++st.comparisons;
        if (arr[j] <= pivot) {
            ++i;
            if (i != j) {
                int tmp = arr[i];
                arr[i] = arr[j];
                arr[j] = tmp;
                st.mainWrites += 2;
            }
        }
    }
    if (i + 1 != high) {
        int tmp = arr[i + 1];
        arr[i + 1] = arr[high];
        arr[high] = tmp;
        st.mainWrites += 2;
    }
    return i + 1;
}

void quickSortRec(int* arr, int low, int high, Stats& st, int depth) {
    if (low >= high) return;
    if (depth > st.maxDepth) st.maxDepth = depth;
    int p = partitionQuick(arr, low, high, st);
    if (p - 1 > low) quickSortRec(arr, low, p - 1, st, depth + 1);
    if (p + 1 < high) quickSortRec(arr, p + 1, high, st, depth + 1);
}

void quickSort(int* arr, int n, Stats& st) {
    st.reset();
    if (n > 1) quickSortRec(arr, 0, n - 1, st, 1);
}

void siftDown(int* arr, int n, int i, Stats& st, int depth) {
    if (depth > st.maxDepth) st.maxDepth = depth;
    int largest = i;
    int l = 2 * i + 1;
    int r = 2 * i + 2;

    if (l < n) {
        ++st.comparisons;
        if (arr[l] > arr[largest]) largest = l;
    }
    if (r < n) {
        ++st.comparisons;
        if (arr[r] > arr[largest]) largest = r;
    }

    if (largest != i) {
        int tmp = arr[i];
        arr[i] = arr[largest];
        arr[largest] = tmp;
        st.mainWrites += 2;
        siftDown(arr, n, largest, st, depth + 1);
    }
}

void heapSort(int* arr, int n, Stats& st) {
    st.reset();
    if (n <= 1) return;

    for (int i = n / 2 - 1; i >= 0; --i) {
        siftDown(arr, n, i, st, 1);
    }
    for (int i = n - 1; i > 0; --i) {
        int tmp = arr[0];
        arr[0] = arr[i];
        arr[i] = tmp;
        st.mainWrites += 2;
        siftDown(arr, i, 0, st, 1);
    }
}

void printCompareTable(const char* label, const int* src, int n, int* temp) {
    int aM[64], aQ[64], aH[64];
    std::memcpy(aM, src, sizeof(int) * n);
    std::memcpy(aQ, src, sizeof(int) * n);
    std::memcpy(aH, src, sizeof(int) * n);

    Stats sM, sQ, sH;
    mergeSort(aM, n, temp, sM, false);
    quickSort(aQ, n, sQ);
    heapSort(aH, n, sH);

    checkSortedArray(src, aM, n);
    checkSortedArray(src, aQ, n);
    checkSortedArray(src, aH, n);

    std::cout << "\n--- " << label << " (n = " << n << ") ---\n";
    std::cout << "Результат: "; printSlice(aM, 0, n); std::cout << "\n";
    std::cout << std::left << std::setw(12) << "Алгоритм"
              << std::setw(12) << "Порівняння"
              << std::setw(12) << "Доп.записи"
              << std::setw(12) << "Осн.записи"
              << std::setw(12) << "Усього зап."
              << std::setw(10) << "Злиття"
              << std::setw(10) << "Глибина" << "\n";

    auto row = [](const char* name, const Stats& s) {
        std::cout << std::left << std::setw(12) << name
                  << std::setw(12) << s.comparisons
                  << std::setw(12) << s.auxWrites
                  << std::setw(12) << s.mainWrites
                  << std::setw(12) << s.totalWrites()
                  << std::setw(10) << s.merges
                  << std::setw(10) << s.maxDepth << "\n";
    };

    row("MergeSort", sM);
    row("QuickSort", sQ);
    row("HeapSort", sH);
}

int main() {
    const int MAX_N = 1024;
    int* temp = new int[MAX_N];

    std::cout << "=== 1. Окремі тести злиття ===\n";
    bool mOk = runMergeTests(temp);

    std::cout << "\n=== 2. Автоматизовані тести MergeSort ===\n";
    bool sOk = runMergeSortTests(temp);

    if (!mOk || !sOk) {
        std::cout << "Виявлено помилку в тестах.\n";
        delete[] temp;
        return 1;
    }

    // Трасування на масиві з 8 елементів
    std::cout << "\n=== 3. Трасування поділу і злиття (8 елементів) ===\n";
    int traceData[8] = {14, 5, 19, 2, 11, 8, 24, 6};
    int traceBackup[8];
    std::memcpy(traceBackup, traceData, sizeof(traceData));
    Stats traceSt;
    mergeSort(traceData, 8, temp, traceSt, true);
    checkSortedArray(traceBackup, traceData, 8);
    std::cout << "Статистика трасування: порівнянь=" << traceSt.comparisons
              << ", доп.записів=" << traceSt.auxWrites
              << ", осн.записів=" << traceSt.mainWrites
              << ", усього записів=" << traceSt.totalWrites()
              << ", злиттів=" << traceSt.merges
              << ", макс.глибина=" << traceSt.maxDepth << "\n";

    // Варіант № 1 (16 елементів)
    int variantData[16] = {22, 5, 18, 2, 27, 10, 7, 20, 1, 16, 9, 25, 4, 14, 29, 6};
    printCompareTable("Порівняння на індивідуальному наборі (Варіант № 1)", variantData, 16, temp);

    delete[] temp;
    return 0;
}
