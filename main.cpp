#include <iostream>
#include <iomanip>
#include <cmath>
#include <cstdint>

using namespace std;

// Варіант 1:
// 1) Прямий пошук в одновимірному масиві цілих чисел заданого розміру
//    з підрахунком кількості порівнянь елементів масиву з ключем.
// 2) Ітеративний бінарний пошук у впорядкованому за неспаданням масиві
//    цілих чисел з підрахунком кількості порівнянь елементів масиву з ключем.
// 3) Дослідження залежності кількості порівнянь від розміру масиву n
//    для n з {10, 50, 100, 500, 1000, 5000, 10000} та обчислення середнього
//    значення за серією запусків.
// 4) Перевірка поведінки бінарного пошуку на невпорядкованому масиві.

struct SearchResult {
    int index;
    long long comparisons;
};

// Детермінований псевдовипадковий генератор (LCG) без зайвих залежностей
uint64_t nextRng(uint64_t& state) {
    state = state * 6364136223846793005ULL + 1442695040888963407ULL;
    return state;
}

int randRange(uint64_t& state, int low, int high) {
    if (high <= low) return low;
    uint64_t span = static_cast<uint64_t>(high - low) + 1ULL;
    return low + static_cast<int>(nextRng(state) % span);
}

// 1. Прямий пошук (лінійний пошук)
SearchResult linearSearch(const int* arr, int n, int key) {
    long long comparisons = 0;
    for (int i = 0; i < n; ++i) {
        ++comparisons;
        if (arr[i] == key) {
            return {i, comparisons};
        }
    }
    return {-1, comparisons};
}

// 2. Ітеративний бінарний пошук (без рекурсії)
SearchResult binarySearchIterative(const int* arr, int n, int key) {
    long long comparisons = 0;
    int left = 0;
    int right = n - 1;

    while (left <= right) {
        int mid = left + (right - left) / 2;
        ++comparisons;
        if (arr[mid] == key) {
            return {mid, comparisons};
        } else if (arr[mid] < key) {
            left = mid + 1;
        } else {
            right = mid - 1;
        }
    }
    return {-1, comparisons};
}

// Заповнення невпорядкованого масиву в купі (heap)
void fillUnsorted(int* arr, int n, uint64_t& rng) {
    for (int i = 0; i < n; ++i) {
        arr[i] = randRange(rng, -50000, 50000);
    }
}

// Заповнення впорядкованого за неспаданням масиву в купі (heap)
void fillSortedNonDecreasing(int* arr, int n, uint64_t& rng) {
    int current = randRange(rng, -10000, 0);
    for (int i = 0; i < n; ++i) {
        current += randRange(rng, 0, 15);
        arr[i] = current;
    }
}

void runUnitChecks() {
    cout << "=== 1. Контрольна перевірка коректності алгоритмів ===\n";

    int sortedSample[] = {-12, -4, 0, 3, 3, 8, 15, 22, 41, 99};
    int m = sizeof(sortedSample) / sizeof(sortedSample[0]);

    SearchResult r1 = linearSearch(sortedSample, m, 15);
    SearchResult r2 = linearSearch(sortedSample, m, 77);
    cout << "Прямий пошук (ключ 15): індекс = " << r1.index
         << ", порівнянь = " << r1.comparisons << "\n";
    cout << "Прямий пошук (ключ 77, відсутній): індекс = " << r2.index
         << ", порівнянь = " << r2.comparisons << "\n";

    SearchResult b1 = binarySearchIterative(sortedSample, m, 15);
    SearchResult b2 = binarySearchIterative(sortedSample, m, 77);
    SearchResult b3 = binarySearchIterative(sortedSample, m, -12);
    cout << "Бінарний пошук (ключ 15): індекс = " << b1.index
         << ", порівнянь = " << b1.comparisons << "\n";
    cout << "Бінарний пошук (ключ 77, відсутній): індекс = " << b2.index
         << ", порівнянь = " << b2.comparisons << "\n";
    cout << "Бінарний пошук (ключ -12, перший елемент): індекс = " << b3.index
         << ", порівнянь = " << b3.comparisons << "\n\n";
}

void runComplexityExperiments() {
    const int sizes[] = {10, 50, 100, 500, 1000, 5000, 10000};
    const int numSizes = sizeof(sizes) / sizeof(sizes[0]);
    const int trialsPerSize = 200;

    uint64_t rng = 20260101ULL;

    cout << "=== 2. Експериментальне дослідження кількості порівнянь (серія: "
         << trialsPerSize << " запусків на кожен n) ===\n";
    cout << left
         << setw(8)  << "n"
         << setw(16) << "LinAvg(found)"
         << setw(16) << "LinMax(notFnd)"
         << setw(16) << "BinAvg(found)"
         << setw(16) << "BinAvg(notFnd)"
         << setw(14) << "TeorLog2(n)" << "\n";
    cout << "----------------------------------------------------------------------------------------\n";

    for (int s = 0; s < numSizes; ++s) {
        int n = sizes[s];
        int* arr = new int[n];

        long long sumLinFound = 0;
        long long maxLinNotFound = 0;
        long long sumBinFound = 0;
        long long sumBinNotFound = 0;

        for (int t = 0; t < trialsPerSize; ++t) {
            fillSortedNonDecreasing(arr, n, rng);

            int targetIdx = randRange(rng, 0, n - 1);
            int existingKey = arr[targetIdx];
            int missingKey = arr[n - 1] + 100;

            SearchResult lf = linearSearch(arr, n, existingKey);
            SearchResult ln = linearSearch(arr, n, missingKey);
            SearchResult bf = binarySearchIterative(arr, n, existingKey);
            SearchResult bn = binarySearchIterative(arr, n, missingKey);

            sumLinFound += lf.comparisons;
            if (ln.comparisons > maxLinNotFound) {
                maxLinNotFound = ln.comparisons;
            }
            sumBinFound += bf.comparisons;
            sumBinNotFound += bn.comparisons;
        }

        double linAvgFound = static_cast<double>(sumLinFound) / trialsPerSize;
        double binAvgFound = static_cast<double>(sumBinFound) / trialsPerSize;
        double binAvgNotFound = static_cast<double>(sumBinNotFound) / trialsPerSize;
        double log2n = log2(static_cast<double>(n));

        cout << left
             << setw(8)  << n
             << setw(16) << fixed << setprecision(2) << linAvgFound
             << setw(16) << maxLinNotFound
             << setw(16) << fixed << setprecision(2) << binAvgFound
             << setw(16) << fixed << setprecision(2) << binAvgNotFound
             << setw(14) << fixed << setprecision(2) << log2n << "\n";

        delete[] arr;
    }
    cout << "\n";
}

void runUnsortedBinaryCheck() {
    cout << "=== 3. Поведінка ітеративного бінарного пошуку на невпорядкованому масиві ===\n";
    const int n = 12;
    int* unsorted = new int[n];
    uint64_t rng = 987654321ULL;
    fillUnsorted(unsorted, n, rng);

    // Гарантовано помістимо відомий ключ у середину невпорядкованого масиву
    int plantedKey = 777;
    unsorted[n - 2] = plantedKey;

    cout << "Елементи невпорядкованого масиву: ";
    for (int i = 0; i < n; ++i) {
        cout << unsorted[i] << (i + 1 < n ? ", " : "\n");
    }

    SearchResult res = binarySearchIterative(unsorted, n, plantedKey);
    cout << "Пошук ключа " << plantedKey
         << " бінарним пошуком без попереднього сортування: індекс = " << res.index
         << ", порівнянь = " << res.comparisons << "\n";
    cout << "Примітка: оскільки масив не впорядкований за неспаданням, "
         << "бінарний пошук може відкинути частину масиву, де знаходиться ключ, "
         << "і повернути -1 навіть за наявності елемента.\n\n";

    delete[] unsorted;
}

int main() {
    setlocale(LC_ALL, "");
    runUnitChecks();
    runComplexityExperiments();
    runUnsortedBinaryCheck();
    return 0;
}
