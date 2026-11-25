#include <iostream>
#include <iomanip>

struct Metrics {
    long long comparisons = 0;
    long long moves = 0;
    int stepsCount = 0;
    int stepsUsed[64]{};
    bool verified = false;
};

void calcInvariants(const int* arr, int n, long long& sum, unsigned long long& sqSum) {
    sum = 0;
    sqSum = 0;
    for (int i = 0; i < n; ++i) {
        long long val = static_cast<long long>(arr[i]);
        sum += val;
        sqSum += static_cast<unsigned long long>(val * val);
    }
}

bool verifySort(long long origSum, unsigned long long origSqSum, const int* sorted, int n) {
    if (n <= 0) return true;
    for (int i = 1; i < n; ++i) {
        if (sorted[i - 1] > sorted[i]) {
            return false;
        }
    }
    long long newSum = 0;
    unsigned long long newSqSum = 0;
    calcInvariants(sorted, n, newSum, newSqSum);
    return (newSum == origSum) && (newSqSum == origSqSum);
}

int buildGaps(int n, int mode, int* gaps, int maxGaps) {
    if (n <= 1 || maxGaps <= 0) return 0;
    int count = 0;

    if (mode == 1) {
        for (int gap = n / 2; gap >= 1 && count < maxGaps; gap /= 2) {
            gaps[count++] = gap;
            if (gap == 1) break;
        }
    } else if (mode == 2) {
        if (n == 15) {
            const int preset15[3] = {7, 3, 1};
            for (int i = 0; i < 3 && count < maxGaps; ++i) {
                gaps[count++] = preset15[i];
            }
            return count;
        }

        int temp[64]{};
        int tCount = 0;
        long long h = 1;
        while (h < n && tCount < 64) {
            temp[tCount++] = static_cast<int>(h);
            h = h * 3 + 1;
        }
        for (int i = tCount - 1; i >= 0 && count < maxGaps; --i) {
            gaps[count++] = temp[i];
        }
        if (count == 0 || gaps[count - 1] != 1) {
            if (count < maxGaps) gaps[count++] = 1;
        }
    }
    return count;
}

void printArray(const int* arr, int n) {
    std::cout << "[";
    for (int i = 0; i < n; ++i) {
        if (i > 0) std::cout << ", ";
        std::cout << arr[i];
    }
    std::cout << "]";
}

void printSteps(const int* steps, int count) {
    for (int i = 0; i < count; ++i) {
        if (i > 0) std::cout << " -> ";
        std::cout << steps[i];
    }
}

void shellSort(const int* input, int n, int mode, bool traceSteps, Metrics& out) {
    out = Metrics{};
    long long origSum = 0;
    unsigned long long origSqSum = 0;
    calcInvariants(input, n, origSum, origSqSum);

    int* arr = new int[n > 0 ? n : 1];
    for (int i = 0; i < n; ++i) {
        arr[i] = input[i];
    }

    int gaps[64]{};
    int gapCount = buildGaps(n, mode, gaps, 64);
    out.stepsCount = gapCount;
    for (int g = 0; g < gapCount; ++g) {
        out.stepsUsed[g] = gaps[g];
    }

    for (int g = 0; g < gapCount; ++g) {
        int gap = gaps[g];
        for (int i = gap; i < n; ++i) {
            int temp = arr[i];
            out.moves++;
            int j = i;
            while (j >= gap) {
                out.comparisons++;
                if (arr[j - gap] > temp) {
                    arr[j] = arr[j - gap];
                    out.moves++;
                    j -= gap;
                } else {
                    break;
                }
            }
            if (j != i) {
                arr[j] = temp;
                out.moves++;
            }
        }
        if (traceSteps) {
            std::cout << "  Після кроку gap = " << gap << ": ";
            printArray(arr, n);
            std::cout << "\n";
        }
    }

    out.verified = verifySort(origSum, origSqSum, arr, n);
    delete[] arr;
}

void insertionSort(const int* input, int n, Metrics& out) {
    out = Metrics{};
    long long origSum = 0;
    unsigned long long origSqSum = 0;
    calcInvariants(input, n, origSum, origSqSum);

    int* arr = new int[n > 0 ? n : 1];
    for (int i = 0; i < n; ++i) {
        arr[i] = input[i];
    }

    out.stepsCount = (n > 1) ? 1 : 0;
    if (out.stepsCount == 1) out.stepsUsed[0] = 1;

    for (int i = 1; i < n; ++i) {
        int key = arr[i];
        out.moves++;
        int j = i - 1;
        while (j >= 0) {
            out.comparisons++;
            if (arr[j] > key) {
                arr[j + 1] = arr[j];
                out.moves++;
                --j;
            } else {
                break;
            }
        }
        if (j + 1 != i) {
            arr[j + 1] = key;
            out.moves++;
        }
    }

    out.verified = verifySort(origSum, origSqSum, arr, n);
    delete[] arr;
}

void generateDataset(int* arr, int n, int type, unsigned int seed) {
    if (type == 0) {
        for (int i = 0; i < n; ++i) arr[i] = i * 2;
    } else if (type == 1) {
        for (int i = 0; i < n; ++i) arr[i] = (n - i) * 2;
    } else if (type == 2) {
        for (int i = 0; i < n; ++i) arr[i] = i * 2;
        unsigned int s = seed;
        int swaps = (n >= 20) ? n / 20 : 1;
        for (int k = 0; k < swaps; ++k) {
            s = s * 1664525u + 1013904223u;
            int idx = static_cast<int>(s % static_cast<unsigned int>(n - 1));
            int tmp = arr[idx];
            arr[idx] = arr[idx + 1];
            arr[idx + 1] = tmp;
        }
    } else {
        unsigned int s = seed;
        for (int i = 0; i < n; ++i) {
            s = s * 1664525u + 1013904223u;
            arr[i] = static_cast<int>((s % 100000u)) - 50000;
        }
    }
}

void runAutomatedTests() {
    struct TestCase {
        const char* name;
        int n;
        int data[25];
    };

    TestCase tests[12] = {
        {"1. Порожній масив", 0, {}},
        {"2. Один елемент", 1, {44}},
        {"3. Вже впорядкований", 5, {1, 2, 3, 4, 5}},
        {"4. Зворотний порядок", 5, {5, 4, 3, 2, 1}},
        {"5. Змішаний порядок", 5, {7, 2, 9, 1, 5}},
        {"6. Повторення", 5, {3, 1, 3, 2, 1}},
        {"7. Від'ємні значення", 5, {0, -6, 8, -2, 4}},
        {"8. Однакові елементи", 4, {6, 6, 6, 6}},
        {"9. Мінімальна перестановка", 3, {2, 11, 2}},
        {"10. Майже впорядкований", 6, {1, 2, 4, 3, 5, 6}},
        {"11. Власний (непарний, 7 ел.)", 7, {15, -3, 22, 0, 8, -3, 11}},
        {"12. Власний (21 елемент)", 21, {42, -10, 17, 5, 88, 0, -10, 31, 12, 64, 9, 23, -1, 55, 19, 73, 4, 28, 14, 60, 2}}
    };

    int passed = 0;
    std::cout << "=== АВТОМАТИЗОВАНІ ТЕСТИ (24 ПЕРЕВІРКИ) ===\n";
    for (int t = 0; t < 12; ++t) {
        for (int mode = 1; mode <= 2; ++mode) {
            Metrics m;
            shellSort(tests[t].data, tests[t].n, mode, false, m);
            if (m.verified) ++passed;
            std::cout << "Тест " << std::setw(2) << (t * 2 + mode)
                      << " | " << tests[t].name
                      << " | Кроки: " << (mode == 1 ? "Ділення навпіл" : "Потроєння + 1")
                      << " | Статус: " << (m.verified ? "успішно" : "ПОМИЛКА") << "\n";
        }
    }
    std::cout << "Пройдено тестів: " << passed << " / 24\n\n";
}

void runVariant1() {
    const int var1[15] = {28, 6, 19, 3, 25, 11, 1, 22, 8, 17, 4, 14, 30, 9, 2};
    const int n = 15;

    std::cout << "=== ВАРІАНТ № 1 (n = 15) ===\n";
    std::cout << "Початковий масив: ";
    printArray(var1, n);
    std::cout << "\n";

    Metrics mHalf, mKnuth, mIns;

    std::cout << "\n[Ділення навпіл — проміжні стани]\n";
    shellSort(var1, n, 1, true, mHalf);
    std::cout << "Порівняння: " << mHalf.comparisons
              << " | Переміщення: " << mHalf.moves
              << " | Використано кроків: " << mHalf.stepsCount
              << " | Кроки: ";
    printSteps(mHalf.stepsUsed, mHalf.stepsCount);
    std::cout << " | Перевірка: " << (mHalf.verified ? "успішно" : "помилка") << "\n";

    std::cout << "\n[Потроєння + 1 — проміжні стани]\n";
    shellSort(var1, n, 2, true, mKnuth);
    std::cout << "Порівняння: " << mKnuth.comparisons
              << " | Переміщення: " << mKnuth.moves
              << " | Використано кроків: " << mKnuth.stepsCount
              << " | Кроки: ";
    printSteps(mKnuth.stepsUsed, mKnuth.stepsCount);
    std::cout << " | Перевірка: " << (mKnuth.verified ? "успішно" : "помилка") << "\n";

    insertionSort(var1, n, mIns);
    std::cout << "\n[Звичайні вставки]\n";
    std::cout << "Порівняння: " << mIns.comparisons
              << " | Переміщення: " << mIns.moves
              << " | Використано кроків: " << mIns.stepsCount
              << " | Перевірка: " << (mIns.verified ? "успішно" : "помилка") << "\n\n";
}

void runLargeExperiment() {
    const int sizes[3] = {100, 1000, 10000};
    const char* types[4] = {"Впорядкований", "Зворотний", "Майже впорядк.", "Змішаний"};

    std::cout << "=== ВЕЛИКИЙ ЕКСПЕРИМЕНТ (100, 1 000, 10 000) ===\n";
    std::cout << std::left
              << std::setw(8) << "Розмір"
              << std::setw(16) << "Тип даних"
              << std::setw(15) << "Алгоритм"
              << std::setw(13) << "Порівняння"
              << std::setw(13) << "Переміщення"
              << std::setw(8) << "Кроки"
              << "Перевірка\n";
    std::cout << "----------------------------------------------------------------------------------\n";

    for (int s = 0; s < 3; ++s) {
        int n = sizes[s];
        int* buf = new int[n];
        for (int t = 0; t < 4; ++t) {
            generateDataset(buf, n, t, 424242u + static_cast<unsigned int>(n * 17 + t));

            Metrics mIns, mHalf, mKnuth;
            insertionSort(buf, n, mIns);
            shellSort(buf, n, 1, false, mHalf);
            shellSort(buf, n, 2, false, mKnuth);

            auto printRow = [&](const char* alg, const Metrics& m) {
                std::cout << std::left
                          << std::setw(8) << n
                          << std::setw(16) << types[t]
                          << std::setw(15) << alg
                          << std::setw(13) << m.comparisons
                          << std::setw(13) << m.moves
                          << std::setw(8) << m.stepsCount
                          << (m.verified ? "успішно" : "помилка") << "\n";
            };

            printRow("Вставки", mIns);
            printRow("Шелл n/2", mHalf);
            printRow("Шелл +1", mKnuth);
        }
        delete[] buf;
    }
}

int main() {
    std::ios::sync_with_stdio(false);
    std::cin.tie(nullptr);

    runAutomatedTests();
    runVariant1();
    runLargeExperiment();

    return 0;
}
