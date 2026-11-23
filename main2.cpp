#include <iostream>
#include <iomanip>
#include <random>

struct SortCounters {
    long long comparisons = 0;
    long long moves = 0;
};

struct ArraySummary {
    int count = 0;
    long long sum = 0;
    long long sumSquares = 0;
};

ArraySummary summarizeArray(const int arr[], int n) {
    ArraySummary summary;
    summary.count = n;
    for (int i = 0; i < n; ++i) {
        long long value = arr[i];
        summary.sum += value;
        summary.sumSquares += value * value;
    }
    return summary;
}

bool isSortedNonDecreasing(const int arr[], int n) {
    for (int i = 1; i < n; ++i) {
        if (arr[i] < arr[i - 1]) {
            return false;
        }
    }
    return true;
}

bool verifySorting(const int original[], int origN, const int sorted[], int sortedN) {
    if (!isSortedNonDecreasing(sorted, sortedN)) {
        return false;
    }
    ArraySummary before = summarizeArray(original, origN);
    ArraySummary after = summarizeArray(sorted, sortedN);
    return before.count == after.count &&
           before.sum == after.sum &&
           before.sumSquares == after.sumSquares;
}

void insertionSort(int arr[], int n, SortCounters& counters) {
    counters = {0, 0};
    for (int i = 1; i < n; ++i) {
        int current = arr[i];
        ++counters.moves;

        int j = i - 1;
        while (j >= 0) {
            ++counters.comparisons;
            if (arr[j] > current) {
                arr[j + 1] = arr[j];
                ++counters.moves;
                --j;
            } else {
                break;
            }
        }
        arr[j + 1] = current;
        ++counters.moves;
    }
}

void selectionSort(int arr[], int n, SortCounters& counters) {
    counters = {0, 0};
    for (int i = 0; i < n - 1; ++i) {
        int minIndex = i;
        for (int j = i + 1; j < n; ++j) {
            ++counters.comparisons;
            if (arr[j] < arr[minIndex]) {
                minIndex = j;
            }
        }
        if (minIndex != i) {
            int temp = arr[i];
            ++counters.moves;
            arr[i] = arr[minIndex];
            ++counters.moves;
            arr[minIndex] = temp;
            ++counters.moves;
        }
    }
}

void bubbleSort(int arr[], int n, SortCounters& counters) {
    counters = {0, 0};
    for (int i = 0; i < n - 1; ++i) {
        bool swapped = false;
        for (int j = 0; j < n - 1 - i; ++j) {
            ++counters.comparisons;
            if (arr[j] > arr[j + 1]) {
                int temp = arr[j];
                ++counters.moves;
                arr[j] = arr[j + 1];
                ++counters.moves;
                arr[j + 1] = temp;
                ++counters.moves;
                swapped = true;
            }
        }
        if (!swapped) {
            break;
        }
    }
}

void copyElements(const int src[], int dst[], int n) {
    for (int i = 0; i < n; ++i) {
        dst[i] = src[i];
    }
}

bool sameElements(const int a[], int nA, const int b[], int nB) {
    if (nA != nB) {
        return false;
    }
    for (int i = 0; i < nA; ++i) {
        if (a[i] != b[i]) {
            return false;
        }
    }
    return true;
}

void printElements(const int arr[], int n) {
    std::cout << "[";
    for (int i = 0; i < n; ++i) {
        std::cout << arr[i];
        if (i + 1 < n) {
            std::cout << ", ";
        }
    }
    std::cout << "]";
}

using SortFunction = void(*)(int[], int, SortCounters&);

bool runTestCase(int testNumber, const char* algoName, SortFunction sortFn,
                 const int input[], int n, const int expected[], int expN) {
    constexpr int MAX_TEST_N = 256;
    int buffer[MAX_TEST_N];
    copyElements(input, buffer, n);

    SortCounters counters;
    sortFn(buffer, n, counters);

    bool passed = verifySorting(input, n, buffer, n) && sameElements(buffer, n, expected, expN);
    std::cout << "[" << algoName << "] Тест " << std::setw(2) << testNumber << ": "
              << (passed ? "успішно" : "помилка")
              << " (порівнянь: " << counters.comparisons
              << ", переміщень: " << counters.moves << ")\n";
    if (!passed) {
        std::cout << "  Очікувано: ";
        printElements(expected, expN);
        std::cout << "\n  Фактично:  ";
        printElements(buffer, n);
        std::cout << "\n";
    }
    return passed;
}

void runAllTests() {
    struct TestData {
        int number;
        const int* input;
        int n;
        const int* expected;
        int expN;
    };

    const int t1_in[] = {}, t1_exp[] = {};
    const int t2_in[] = {7}, t2_exp[] = {7};
    const int t3_in[] = {1, 2, 3, 4, 5}, t3_exp[] = {1, 2, 3, 4, 5};
    const int t4_in[] = {5, 4, 3, 2, 1}, t4_exp[] = {1, 2, 3, 4, 5};
    const int t5_in[] = {4, 1, 5, 2, 3}, t5_exp[] = {1, 2, 3, 4, 5};
    const int t6_in[] = {3, 1, 3, 2, 1}, t6_exp[] = {1, 1, 2, 3, 3};
    const int t7_in[] = {0, -5, 8, -2, 4}, t7_exp[] = {-5, -2, 0, 4, 8};
    const int t8_in[] = {6, 6, 6, 6}, t8_exp[] = {6, 6, 6, 6};
    const int t9_in[] = {2, 11, 2}, t9_exp[] = {2, 2, 11};
    const int t10_in[] = {9, -1, 9, 0, -1, 5}, t10_exp[] = {-1, -1, 0, 5, 9, 9};
    const int t11_in[] = {-10, -20, -30, -5}, t11_exp[] = {-30, -20, -10, -5};
    const int t12_in[] = {100, -100, 50, -50, 0, 50}, t12_exp[] = {-100, -50, 0, 50, 50, 100};

    TestData suite[] = {
        {1,  t1_in,  0, t1_exp,  0},
        {2,  t2_in,  1, t2_exp,  1},
        {3,  t3_in,  5, t3_exp,  5},
        {4,  t4_in,  5, t4_exp,  5},
        {5,  t5_in,  5, t5_exp,  5},
        {6,  t6_in,  5, t6_exp,  5},
        {7,  t7_in,  5, t7_exp,  5},
        {8,  t8_in,  4, t8_exp,  4},
        {9,  t9_in,  3, t9_exp,  3},
        {10, t10_in, 6, t10_exp, 6},
        {11, t11_in, 4, t11_exp, 4},
        {12, t12_in, 6, t12_exp, 6}
    };

    struct AlgoItem {
        const char* name;
        SortFunction fn;
    };
    AlgoItem algos[] = {
        {"Вставками", insertionSort},
        {"Вибором", selectionSort},
        {"Бульбашкове", bubbleSort}
    };

    int okCount = 0;
    int totalCount = 0;
    for (const auto& algo : algos) {
        for (const auto& item : suite) {
            ++totalCount;
            if (runTestCase(item.number, algo.name, algo.fn, item.input, item.n, item.expected, item.expN)) {
                ++okCount;
            }
        }
    }
    std::cout << "Успішних перевірок: " << okCount << " з " << totalCount << "\n\n";
}

void runComparisonExperiment() {
    constexpr int N = 100;
    int ascending[N];
    int descending[N];
    int mixed[N];
    int work[N];

    for (int i = 0; i < N; ++i) {
        ascending[i] = i;
        descending[i] = N - 1 - i;
    }

    std::mt19937 gen(42);
    std::uniform_int_distribution<int> dist(-500, 500);
    for (int i = 0; i < N; ++i) {
        mixed[i] = dist(gen);
    }

    struct DatasetItem {
        const char* type;
        const int* data;
    };
    DatasetItem datasets[] = {
        {"Зростання", ascending},
        {"Спадання", descending},
        {"Змішаний", mixed}
    };

    struct AlgoItem {
        const char* name;
        SortFunction fn;
    };
    AlgoItem algos[] = {
        {"Вставки", insertionSort},
        {"Вибір", selectionSort},
        {"Бульбашковий", bubbleSort}
    };

    std::cout << std::left
              << std::setw(15) << "Алгоритм"
              << std::setw(14) << "Тип набору"
              << std::setw(14) << "Порівняння"
              << std::setw(14) << "Переміщення"
              << "Перевірка\n";

    for (const auto& algo : algos) {
        for (const auto& ds : datasets) {
            copyElements(ds.data, work, N);
            SortCounters counters;
            algo.fn(work, N, counters);
            bool ok = verifySorting(ds.data, N, work, N);
            std::cout << std::left
                      << std::setw(15) << algo.name
                      << std::setw(14) << ds.type
                      << std::setw(14) << counters.comparisons
                      << std::setw(14) << counters.moves
                      << (ok ? "успішно" : "помилка") << "\n";
        }
    }
    std::cout << "\n";
}

void runVariantOne() {
    constexpr int N = 10;
    const int initial[N] = {18, 4, 27, 9, 2, 15, 31, 6, 20, 11};
    int sorted[N];

    std::cout << "Варіант 1. Початковий масив: ";
    printElements(initial, N);
    std::cout << "\n";

    struct AlgoItem {
        const char* name;
        SortFunction fn;
    };
    AlgoItem algos[] = {
        {"Сортування вставками", insertionSort},
        {"Сортування вибором", selectionSort},
        {"Бульбашкове сортування", bubbleSort}
    };

    for (const auto& algo : algos) {
        copyElements(initial, sorted, N);
        SortCounters counters;
        algo.fn(sorted, N, counters);
        bool ok = verifySorting(initial, N, sorted, N);

        std::cout << algo.name << " -> ";
        printElements(sorted, N);
        std::cout << " | порівнянь: " << counters.comparisons
                  << ", переміщень: " << counters.moves
                  << ", перевірка: " << (ok ? "успішно" : "помилка") << "\n";
    }
}

int main() {
    runAllTests();
    runComparisonExperiment();
    runVariantOne();
    return 0;
}
