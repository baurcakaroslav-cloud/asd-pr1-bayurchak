#include <iostream>
#include <iomanip>

struct SortStats {
    long long comparisons = 0;
    long long moves = 0;
    long long passes = 0;
    bool check_ok = false;
};

// Завдання 1. Підготовка та відновлення масиву
void copyArray(const int* src, int* dst, int n) {
    for (int i = 0; i < n; ++i) {
        dst[i] = src[i];
    }
}

bool arraysEqual(const int* a, const int* b, int n) {
    for (int i = 0; i < n; ++i) {
        if (a[i] != b[i]) {
            return false;
        }
    }
    return true;
}

// Завдання 5. Автоматична перевірка: неспадний порядок + n, сума, сума квадратів
bool verifySort(const int* orig, const int* sorted, int n) {
    for (int i = 1; i < n; ++i) {
        if (sorted[i - 1] > sorted[i]) {
            return false;
        }
    }

    long long sum_orig = 0, sum_sorted = 0;
    unsigned long long sq_orig = 0, sq_sorted = 0;

    for (int i = 0; i < n; ++i) {
        long long x = orig[i];
        long long y = sorted[i];
        sum_orig += x;
        sum_sorted += y;
        sq_orig += static_cast<unsigned long long>(x * x);
        sq_sorted += static_cast<unsigned long long>(y * y);
    }

    return (sum_orig == sum_sorted) && (sq_orig == sq_sorted);
}

// Завдання 2. Пошук позиції для бінарної вставки у відсортованому префіксі [0..rightCount)
// Новий елемент розміщується ПІСЛЯ вже наявних рівних елементів
int findBinaryInsertPos(const int* arr, int rightCount, int key, long long& comparisons) {
    int left = 0;
    int right = rightCount;
    while (left < right) {
        int mid = left + (right - left) / 2;
        ++comparisons;
        if (arr[mid] <= key) {
            left = mid + 1;
        } else {
            right = mid;
        }
    }
    return left;
}

// Завдання 3. Сортування бінарними вставками
void binaryInsertionSort(int* arr, int n, SortStats& stats) {
    stats = SortStats{};
    for (int i = 1; i < n; ++i) {
        ++stats.passes;
        int key = arr[i];
        ++stats.moves;

        int pos = findBinaryInsertPos(arr, i, key, stats.comparisons);

        for (int j = i; j > pos; --j) {
            arr[j] = arr[j - 1];
            ++stats.moves;
        }

        arr[pos] = key;
        ++stats.moves;
    }
}

// Базовий алгоритм (ПР2): звичайні вставки
void basicInsertionSort(int* arr, int n, SortStats& stats) {
    stats = SortStats{};
    for (int i = 1; i < n; ++i) {
        ++stats.passes;
        int key = arr[i];
        ++stats.moves;

        int j = i;
        while (j > 0) {
            ++stats.comparisons;
            if (arr[j - 1] > key) {
                arr[j] = arr[j - 1];
                ++stats.moves;
                --j;
            } else {
                break;
            }
        }
        arr[j] = key;
        ++stats.moves;
    }
}

// Завдання 4. Шейкерне сортування з двома межами та достроковим завершенням
void shakerSort(int* arr, int n, SortStats& stats) {
    stats = SortStats{};
    if (n <= 1) {
        return;
    }

    int left = 0;
    int right = n - 1;

    while (left < right) {
        bool swapped = false;
        int new_right = left;

        // Прямий прохід (зліва направо)
        ++stats.passes;
        for (int i = left; i < right; ++i) {
            ++stats.comparisons;
            if (arr[i] > arr[i + 1]) {
                int tmp = arr[i];
                arr[i] = arr[i + 1];
                arr[i + 1] = tmp;
                stats.moves += 3;
                swapped = true;
                new_right = i;
            }
        }
        right = new_right;

        if (!swapped) {
            break;
        }

        // Зворотний прохід (справа наліво)
        swapped = false;
        int new_left = right;
        ++stats.passes;
        for (int i = right; i > left; --i) {
            ++stats.comparisons;
            if (arr[i - 1] > arr[i]) {
                int tmp = arr[i - 1];
                arr[i - 1] = arr[i];
                arr[i] = tmp;
                stats.moves += 3;
                swapped = true;
                new_left = i;
            }
        }
        left = new_left;

        if (!swapped) {
            break;
        }
    }
}

// Базовий алгоритм (ПР2): класичне бульбашкове сортування
void basicBubbleSort(int* arr, int n, SortStats& stats) {
    stats = SortStats{};
    for (int i = 0; i < n - 1; ++i) {
        ++stats.passes;
        bool swapped = false;
        for (int j = 0; j < n - 1 - i; ++j) {
            ++stats.comparisons;
            if (arr[j] > arr[j + 1]) {
                int tmp = arr[j];
                arr[j] = arr[j + 1];
                arr[j + 1] = tmp;
                stats.moves += 3;
                swapped = true;
            }
        }
        if (!swapped) {
            break;
        }
    }
}

// Завдання 8. Автоматизовані тести (24 перевірки сортування + 5 перевірок позиції)
void runAutomatedTests() {
    int passed_sort = 0;
    int total_sort = 0;
    int passed_pos = 0;
    int total_pos = 0;

    struct PosCase {
        int prefix[6];
        int len;
        int key;
        int expected_pos;
        const char* desc;
    };

    PosCase pos_tests[5] = {
        {{10, 20, 30, 40, 50, 0}, 5, 5,  0, "початок ділянки"},
        {{10, 20, 30, 40, 50, 0}, 5, 25, 2, "середина ділянки"},
        {{10, 20, 30, 40, 50, 0}, 5, 60, 5, "кінець ділянки"},
        {{2, 5, 5, 5, 9, 0},    5, 5,  4, "після рівних елементів"},
        {{-10, -3, 0, 0, 8, 0}, 5, 0,  4, "після нулів з від'ємними"}
    };

    std::cout << "=== Тести пошуку позиції бінарної вставки ===\n";
    for (int i = 0; i < 5; ++i) {
        ++total_pos;
        long long cmp = 0;
        int actual = findBinaryInsertPos(pos_tests[i].prefix, pos_tests[i].len, pos_tests[i].key, cmp);
        bool ok = (actual == pos_tests[i].expected_pos);
        if (ok) ++passed_pos;
        std::cout << "  PosTest #" << (i + 1) << " (" << pos_tests[i].desc << "): "
                  << (ok ? "PASS" : "FAIL") << " [pos=" << actual
                  << ", exp=" << pos_tests[i].expected_pos << "]\n";
    }

    struct SortCase {
        int input[12];
        int expected[12];
        int n;
        const char* name;
    };

    SortCase sort_tests[12] = {
        {{}, {}, 0, "1. Порожній масив"},
        {{55}, {55}, 1, "2. Один елемент"},
        {{1, 2, 3, 4, 5}, {1, 2, 3, 4, 5}, 5, "3. Впорядкований набір"},
        {{5, 4, 3, 2, 1}, {1, 2, 3, 4, 5}, 5, "4. Зворотний порядок"},
        {{4, 2, 5, 1, 3}, {1, 2, 3, 4, 5}, 5, "5. Змішаний порядок"},
        {{3, 1, 3, 2, 1}, {1, 1, 2, 3, 3}, 5, "6. Повторення"},
        {{0, -4, 7, -1, 3}, {-4, -1, 0, 3, 7}, 5, "7. Від'ємні числа"},
        {{8, 8, 8, 8}, {8, 8, 8, 8}, 4, "8. Однакові значення"},
        {{2, 11, 2}, {2, 2, 11}, 3, "9. Два переставлені елементи"},
        {{1, 2, 3, 5, 4, 6}, {1, 2, 3, 4, 5, 6}, 6, "10. Майже впорядкований набір"},
        {{-20, 15, -20, 0, 9}, {-20, -20, 0, 9, 15}, 5, "11. Власний: дублікати та від'ємні"},
        {{100, 50, 25, 75, 10, 90}, {10, 25, 50, 75, 90, 100}, 6, "12. Власний: 6 елементів"}
    };

    std::cout << "\n=== Автоматизовані тести сортування ===\n";
    int work[12];
    for (int t = 0; t < 12; ++t) {
        ++total_sort;
        copyArray(sort_tests[t].input, work, sort_tests[t].n);
        SortStats st_bin{};
        binaryInsertionSort(work, sort_tests[t].n, st_bin);
        bool ok_bin = arraysEqual(work, sort_tests[t].expected, sort_tests[t].n) &&
                      verifySort(sort_tests[t].input, work, sort_tests[t].n);
        if (ok_bin) ++passed_sort;

        ++total_sort;
        copyArray(sort_tests[t].input, work, sort_tests[t].n);
        SortStats st_shk{};
        shakerSort(work, sort_tests[t].n, st_shk);
        bool ok_shk = arraysEqual(work, sort_tests[t].expected, sort_tests[t].n) &&
                      verifySort(sort_tests[t].input, work, sort_tests[t].n);
        if (ok_shk) ++passed_sort;

        std::cout << "  " << sort_tests[t].name
                  << " | BinIns: " << (ok_bin ? "PASS" : "FAIL")
                  << " | Shaker: " << (ok_shk ? "PASS" : "FAIL") << "\n";
    }

    std::cout << "\nПідсумок тестів: сортування " << passed_sort << "/" << total_sort
              << ", позиції " << passed_pos << "/" << total_pos << "\n\n";
}

// Генерація 4 типів масиву довжиною n (детерміновано при кожному запуску)
void generatePatternArray(int* dst, int n, int pattern_id) {
    // 0 = Зростання, 1 = Спадання, 2 = Майже впорядкований, 3 = Змішаний
    if (pattern_id == 0) {
        for (int i = 0; i < n; ++i) dst[i] = i + 1;
    } else if (pattern_id == 1) {
        for (int i = 0; i < n; ++i) dst[i] = n - i;
    } else if (pattern_id == 2) {
        for (int i = 0; i < n; ++i) dst[i] = i + 1;
        for (int i = 1; i + 1 < n; i += (n >= 100 ? 25 : 3)) {
            int tmp = dst[i];
            dst[i] = dst[i + 1];
            dst[i + 1] = tmp;
        }
    } else {
        unsigned int seed = 1337u;
        for (int i = 0; i < n; ++i) {
            seed = seed * 1664525u + 1013904223u;
            dst[i] = static_cast<int>((seed >> 16) % 10000) + 1;
        }
    }
}

// Завдання 6 та 7. Експеримент на розмірах 10, 100, 1000 та порівняння з базовими алгоритмами
void runExperiment() {
    const int sizes[3] = {10, 100, 1000};
    const char* pattern_names[4] = {
        "Зростання",
        "Спадання",
        "Майже впорядкований",
        "Змішаний"
    };

    int base[1000];
    int work[1000];

    std::cout << "=== Експеримент та порівняння з базовими алгоритмами ===\n";
    for (int s_idx = 0; s_idx < 3; ++s_idx) {
        int n = sizes[s_idx];
        std::cout << "\n--- Розмір масиву N = " << n << " ---\n";
        std::cout << std::left << std::setw(21) << "Набір"
                  << std::setw(14) << "Алгоритм"
                  << std::right << std::setw(12) << "Порівнянь"
                  << std::setw(14) << "Переміщень"
                  << std::setw(10) << "Проходів"
                  << std::setw(10) << "Перевір.\n";

        for (int p = 0; p < 4; ++p) {
            generatePatternArray(base, n, p);

            SortStats st_b_ins{}, st_ins{}, st_shk{}, st_bub{};

            copyArray(base, work, n);
            binaryInsertionSort(work, n, st_b_ins);
            st_b_ins.check_ok = verifySort(base, work, n);

            copyArray(base, work, n);
            basicInsertionSort(work, n, st_ins);
            st_ins.check_ok = verifySort(base, work, n);

            copyArray(base, work, n);
            shakerSort(work, n, st_shk);
            st_shk.check_ok = verifySort(base, work, n);

            copyArray(base, work, n);
            basicBubbleSort(work, n, st_bub);
            st_bub.check_ok = verifySort(base, work, n);

            auto printRow = [&](const char* alg, const SortStats& st) {
                std::cout << std::left << std::setw(21) << pattern_names[p]
                          << std::setw(14) << alg
                          << std::right << std::setw(12) << st.comparisons
                          << std::setw(14) << st.moves
                          << std::setw(10) << st.passes
                          << std::setw(10) << (st.check_ok ? "OK" : "ERR") << "\n";
            };

            printRow("BinInsertion", st_b_ins);
            printRow("Insertion", st_ins);
            printRow("ShakerSort", st_shk);
            printRow("BubbleSort", st_bub);
        }
    }
    std::cout << "\n";
}

// Виконання індивідуального варіанта № 1
void runVariant1() {
    const int n = 12;
    const int initial[n] = {23, 5, 18, 2, 27, 9, 14, 1, 20, 7, 16, 4};
    int work[n];

    std::cout << "=== Індивідуальний варіант № 1 (N = 12) ===\nПочаткова послідовність: ";
    for (int i = 0; i < n; ++i) {
        std::cout << initial[i] << (i + 1 < n ? ", " : "\n");
    }

    copyArray(initial, work, n);
    SortStats st_bin{};
    binaryInsertionSort(work, n, st_bin);
    st_bin.check_ok = verifySort(initial, work, n);

    std::cout << "\n[Бінарні вставки]\nРезультат: ";
    for (int i = 0; i < n; ++i) {
        std::cout << work[i] << (i + 1 < n ? " " : "\n");
    }
    std::cout << "Порівнянь: " << st_bin.comparisons
              << ", Переміщень: " << st_bin.moves
              << ", Проходів: " << st_bin.passes
              << ", Перевірка: " << (st_bin.check_ok ? "УСПІШНО" : "ПОМИЛКА") << "\n";

    copyArray(initial, work, n);
    SortStats st_shk{};
    shakerSort(work, n, st_shk);
    st_shk.check_ok = verifySort(initial, work, n);

    std::cout << "\n[Шейкерне сортування]\nРезультат: ";
    for (int i = 0; i < n; ++i) {
        std::cout << work[i] << (i + 1 < n ? " " : "\n");
    }
    std::cout << "Порівнянь: " << st_shk.comparisons
              << ", Переміщень: " << st_shk.moves
              << ", Проходів: " << st_shk.passes
              << ", Перевірка: " << (st_shk.check_ok ? "УСПІШНО" : "ПОМИЛКА") << "\n";
}

int main() {
    runAutomatedTests();
    runExperiment();
    runVariant1();
    return 0;
}
