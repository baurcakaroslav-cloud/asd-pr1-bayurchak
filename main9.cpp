#include <iostream>
#include <cstring>

using namespace std;

const int MAX_PAT = 256;
const int MAX_TXT = 4096;
const int MAX_MATCHES = 1024;

// Побудова префікс-функції для зразка pat довжини m
void computePrefixFunction(const char* pat, int m, int* pi, int* prefixTransitions = nullptr) {
    if (prefixTransitions) {
        *prefixTransitions = 0;
    }
    if (m <= 0) {
        return;
    }
    pi[0] = 0;
    int k = 0;
    for (int i = 1; i < m; ++i) {
        while (k > 0 && pat[k] != pat[i]) {
            k = pi[k - 1];
            if (prefixTransitions) {
                (*prefixTransitions)++;
            }
        }
        if (pat[k] == pat[i]) {
            ++k;
        }
        pi[i] = k;
    }
}

// Перевірка префікс-функції з виводом першої розбіжності
bool checkPrefixFunction(const char* pat, const int* expected, int expectedLen, int testNum) {
    int m = static_cast<int>(strlen(pat));
    int pi[MAX_PAT] = {0};
    computePrefixFunction(pat, m, pi, nullptr);

    if (m != expectedLen) {
        cout << "[Тест pi #" << testNum << " FAIL] Довжина зразка " << m
             << " != очікуваній " << expectedLen << "\n";
        return false;
    }

    for (int i = 0; i < m; ++i) {
        if (pi[i] != expected[i]) {
            cout << "[Тест pi #" << testNum << " FAIL] Зразок \"" << pat
                 << "\", індекс=" << i << ", символ='" << pat[i]
                 << "', очікувано=" << expected[i] << ", отримано=" << pi[i] << "\n";
            return false;
        }
    }

    cout << "[Тест pi #" << testNum << " OK] Зразок \"" << pat << "\" -> pi: ";
    for (int i = 0; i < m; ++i) {
        cout << pi[i] << (i + 1 < m ? ", " : "");
    }
    cout << "\n";
    return true;
}

// Пошук першого входження алгоритмом КМП
int kmpFindFirst(const char* txt, const char* pat, long long* comparisons = nullptr, long long* prefixShifts = nullptr) {
    if (comparisons) *comparisons = 0;
    if (prefixShifts) *prefixShifts = 0;

    int n = static_cast<int>(strlen(txt));
    int m = static_cast<int>(strlen(pat));

    if (m == 0) {
        return 0;
    }
    if (n < m) {
        return -1;
    }

    int pi[MAX_PAT] = {0};
    computePrefixFunction(pat, m, pi, nullptr);

    int q = 0;
    for (int i = 0; i < n; ++i) {
        while (q > 0 && pat[q] != txt[i]) {
            if (comparisons) ++(*comparisons);
            q = pi[q - 1];
            if (prefixShifts) ++(*prefixShifts);
        }
        if (q > 0 || pat[0] == txt[i]) {
            if (comparisons) ++(*comparisons);
        }
        if (pat[q] == txt[i]) {
            ++q;
        }
        if (q == m) {
            return i - m + 1;
        }
    }
    return -1;
}

// Пошук усіх входжень (включно з накладеними) алгоритмом КМП
int kmpFindAll(const char* txt, const char* pat, int* positions, int maxPos,
               long long* comparisons = nullptr, long long* prefixShifts = nullptr) {
    if (comparisons) *comparisons = 0;
    if (prefixShifts) *prefixShifts = 0;

    int n = static_cast<int>(strlen(txt));
    int m = static_cast<int>(strlen(pat));
    int foundCount = 0;

    if (m == 0) {
        if (maxPos > 0) {
            positions[0] = 0;
            foundCount = 1;
        }
        return foundCount;
    }
    if (n < m) {
        return 0;
    }

    int pi[MAX_PAT] = {0};
    computePrefixFunction(pat, m, pi, nullptr);

    int q = 0;
    for (int i = 0; i < n; ++i) {
        while (q > 0 && pat[q] != txt[i]) {
            if (comparisons) ++(*comparisons);
            q = pi[q - 1];
            if (prefixShifts) ++(*prefixShifts);
        }
        if (comparisons) ++(*comparisons);
        if (pat[q] == txt[i]) {
            ++q;
        }
        if (q == m) {
            if (foundCount < maxPos) {
                positions[foundCount++] = i - m + 1;
            }
            q = pi[q - 1];
            if (prefixShifts) ++(*prefixShifts);
        }
    }
    return foundCount;
}

// Трасування пошуку КМП
void kmpTrace(const char* txt, const char* pat) {
    int n = static_cast<int>(strlen(txt));
    int m = static_cast<int>(strlen(pat));

    cout << "\n=== ТРАСУВАННЯ КМП: текст=\"" << txt << "\", зразок=\"" << pat << "\" ===\n";
    if (m == 0) {
        cout << "Порожній зразок -> входження на позиції 0.\n";
        return;
    }
    if (n < m) {
        cout << "Зразок довший за текст -> результат -1.\n";
        return;
    }

    int pi[MAX_PAT] = {0};
    computePrefixFunction(pat, m, pi, nullptr);

    cout << "Префікс-функція зразка: ";
    for (int j = 0; j < m; ++j) {
        cout << pat[j] << "[" << j << "]=" << pi[j] << (j + 1 < m ? ", " : "");
    }
    cout << "\n";

    int q = 0;
    for (int i = 0; i < n; ++i) {
        while (q > 0 && pat[q] != txt[i]) {
            int oldQ = q;
            q = pi[q - 1];
            cout << "  i=" << i << ", j=" << oldQ
                 << ", pat[" << oldQ << "]='" << pat[oldQ]
                 << "' != txt[" << i << "]='" << txt[i]
                 << "' -> перехід за pi[" << (oldQ - 1) << "]: q=" << q << "\n";
        }

        bool match = (pat[q] == txt[i]);
        int prevQ = q;
        if (match) {
            ++q;
        }

        cout << "  Крок i=" << i << ", j=" << prevQ
             << ", txt[" << i << "]='" << txt[i]
             << "' vs pat[" << prevQ << "]='" << pat[prevQ]
             << "' -> " << (match ? "ЗБІГ" : "НЕЗБІГ")
             << ", q=" << q << "\n";

        if (q == m) {
            cout << "  >>> ПОВНИЙ ЗБІГ! Позиція: " << (i - m + 1);
            q = pi[q - 1];
            cout << ", перехід для накладень: q=" << q << "\n";
        }
    }
}

// Прямий пошук підрядка
int naiveSearchAll(const char* txt, const char* pat, int* positions, int maxPos, long long* comparisons) {
    if (comparisons) *comparisons = 0;
    int n = static_cast<int>(strlen(txt));
    int m = static_cast<int>(strlen(pat));
    int count = 0;

    if (m == 0) {
        if (maxPos > 0) {
            positions[0] = 0;
            count = 1;
        }
        return count;
    }
    if (n < m) {
        return 0;
    }

    for (int i = 0; i <= n - m; ++i) {
        int j = 0;
        while (j < m) {
            if (comparisons) ++(*comparisons);
            if (txt[i + j] != pat[j]) {
                break;
            }
            ++j;
        }
        if (j == m) {
            if (count < maxPos) {
                positions[count++] = i;
            }
        }
    }
    return count;
}

void runPrefixTests() {
    cout << "\n--- АВТОМАТИЗОВАНІ ТЕСТИ ПРЕФІКС-ФУНКЦІЇ ---\n";
    bool ok = true;

    int t1[] = {0};
    ok &= checkPrefixFunction("a", t1, 1, 1);

    int t2[] = {0, 1, 2, 3};
    ok &= checkPrefixFunction("aaaa", t2, 4, 2);

    int t3[] = {0, 0, 0, 0};
    ok &= checkPrefixFunction("abcd", t3, 4, 3);

    int t4[] = {0, 0, 1, 2, 3, 0, 1};
    ok &= checkPrefixFunction("ababaca", t4, 7, 4);

    int t5[] = {0, 1, 0, 1, 2, 2, 3};
    ok &= checkPrefixFunction("aabaaab", t5, 7, 5);

    int t6[] = {0, 0, 0, 1, 2, 1, 2, 3, 4, 5};
    ok &= checkPrefixFunction("abcababcab", t6, 10, 6);

    cout << "Статус тестів префікс-функції: " << (ok ? "ПРОЙДЕНО" : "Є ПОМИЛКИ") << "\n";
}

void runSearchComparisonTest(int num, const char* txt, const char* pat, int expectedFirst,
                           const int* expectedAll, int expectedAllCount) {
    int kmpPos[MAX_MATCHES] = {0};
    int naivePos[MAX_MATCHES] = {0};
    long long kmpCmp = 0, kmpShifts = 0, naiveCmp = 0;

    int first = kmpFindFirst(txt, pat, &kmpCmp, &kmpShifts);
    int allCount = kmpFindAll(txt, pat, kmpPos, MAX_MATCHES, &kmpCmp, &kmpShifts);
    int naiveCount = naiveSearchAll(txt, pat, naivePos, MAX_MATCHES, &naiveCmp);

    bool firstOk = (first == expectedFirst);
    bool allOk = (allCount == expectedAllCount);
    for (int i = 0; i < expectedAllCount && allOk; ++i) {
        if (kmpPos[i] != expectedAll[i]) {
            allOk = false;
        }
    }

    cout << "Тест #" << num << " | txt=\"" << txt << "\" | pat=\"" << pat << "\""
         << " | first=" << first << " (оч. " << expectedFirst << ")"
         << " | all=[";
    for (int i = 0; i < allCount; ++i) {
        cout << kmpPos[i] << (i + 1 < allCount ? ", " : "");
    }
    cout << "] | КМП порівн.=" << kmpCmp
         << ", зсувів=" << kmpShifts
         << " | Наївний порівн.=" << naiveCmp
         << " | " << ((firstOk && allOk && allCount == naiveCount) ? "OK" : "FAIL") << "\n";
}

void runSearchTests() {
    cout << "\n--- АВТОМАТИЗОВАНІ ТЕСТИ ПОШУКУ ТА ПОРІВНЯННЯ З ПРЯМИМ ---\n";
    int e1[] = {4};
    runSearchComparisonTest(1, "algorithm", "rith", 4, e1, 1);

    int e2[] = {0, 1, 2};
    runSearchComparisonTest(2, "aaaaaa", "aa", 0, e2, 3);

    int e3[] = {0, 2, 4};
    runSearchComparisonTest(3, "abababa", "aba", 0, e3, 3);

    int e4[] = {};
    runSearchComparisonTest(4, "abcabcd", "xyz", -1, e4, 0);

    int e5[] = {1, 4};
    runSearchComparisonTest(5, "mississippi", "issi", 1, e5, 2);

    int e6[] = {3};
    runSearchComparisonTest(6, "abcabcabdabcabd", "abcabd", 3, e6, 1);

    int e7[] = {};
    runSearchComparisonTest(7, "no matches here", "xyz", -1, e7, 0);

    int e8[] = {};
    runSearchComparisonTest(8, "", "a", -1, e8, 0);

    int e9[] = {0, 3, 6};
    runSearchComparisonTest(9, "aabaaabaaab", "aab", 0, e9, 3);

    int e10[] = {0, 5};
    runSearchComparisonTest(10, "ababxababx", "ababx", 0, e10, 2);
}

int main() {
    runPrefixTests();
    runSearchTests();

    // Варіант № 1
    const char* v1Txt = "ababcabcabababd";
    const char* v1Pat = "ababd";
    int v1Pos[MAX_MATCHES] = {0};
    long long v1Cmp = 0, v1Shifts = 0, v1NaiveCmp = 0;

    int v1First = kmpFindFirst(v1Txt, v1Pat, nullptr, nullptr);
    int v1Count = kmpFindAll(v1Txt, v1Pat, v1Pos, MAX_MATCHES, &v1Cmp, &v1Shifts);
    naiveSearchAll(v1Txt, v1Pat, nullptr, 0, &v1NaiveCmp);

    cout << "\n--- ВАРІАНТ № 1 ---\n";
    cout << "Текст: " << v1Txt << ", Зразок: " << v1Pat << "\n";
    cout << "Перше входження: " << v1First << ", всього входжень: " << v1Count << "\n";
    cout << "КМП порівнянь: " << v1Cmp << ", переходів за префіксом: " << v1Shifts
         << ", прямий пошук порівнянь: " << v1NaiveCmp << "\n";

    kmpTrace("abababa", "aba");

    return 0;
}
```[cite: 1, 9]
