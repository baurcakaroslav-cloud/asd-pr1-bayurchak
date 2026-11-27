#include <cctype>
#include <cstddef>
#include <cstring>
#include <iomanip>
#include <iostream>

struct SearchStats {
    long long comparisons = 0;
    long long checkedPositions = 0;
    int matchCount = 0;
};

static int calcCharArrayLength(const char arr[], int maxLen) {
    int len = 0;
    while (len < maxLen && arr[len] != '\0') {
        ++len;
    }
    return len;
}

static char toLowerAscii(char ch) {
    return static_cast<char>(std::tolower(static_cast<unsigned char>(ch)));
}

static bool charsEqual(char a, char b, bool caseSensitive) {
    if (caseSensitive) {
        return a == b;
    }
    return toLowerAscii(a) == toLowerAscii(b);
}

int findFirstOccurrence(
    const char text[],
    int textLen,
    const char pattern[],
    int patLen,
    bool caseSensitive,
    SearchStats& stats
) {
    stats = SearchStats{};

    if (patLen == 0) {
        return 0;
    }
    if (textLen <= 0 || patLen > textLen) {
        return -1;
    }

    const int lastStart = textLen - patLen;
    for (int i = 0; i <= lastStart; ++i) {
        ++stats.checkedPositions;
        int j = 0;
        while (j < patLen) {
            ++stats.comparisons;
            if (!charsEqual(text[i + j], pattern[j], caseSensitive)) {
                break;
            }
            ++j;
        }
        if (j == patLen) {
            stats.matchCount = 1;
            return i;
        }
    }

    return -1;
}

int findAllOccurrences(
    const char text[],
    int textLen,
    const char pattern[],
    int patLen,
    bool caseSensitive,
    int outPositions[],
    int maxOutPositions,
    SearchStats& stats
) {
    stats = SearchStats{};

    if (patLen == 0) {
        if (maxOutPositions > 0) {
            outPositions[0] = 0;
            stats.matchCount = 1;
            return 1;
        }
        return 0;
    }
    if (textLen <= 0 || patLen > textLen) {
        return 0;
    }

    int found = 0;
    const int lastStart = textLen - patLen;
    for (int i = 0; i <= lastStart; ++i) {
        ++stats.checkedPositions;
        int j = 0;
        while (j < patLen) {
            ++stats.comparisons;
            if (!charsEqual(text[i + j], pattern[j], caseSensitive)) {
                break;
            }
            ++j;
        }
        if (j == patLen) {
            if (found < maxOutPositions) {
                outPositions[found] = i;
            }
            ++found;
        }
    }

    stats.matchCount = found;
    return found;
}

int findFirstInt(
    const int arr[],
    int arrLen,
    const int pat[],
    int patLen,
    SearchStats& stats
) {
    stats = SearchStats{};

    if (patLen == 0) {
        return 0;
    }
    if (arrLen <= 0 || patLen > arrLen) {
        return -1;
    }

    const int lastStart = arrLen - patLen;
    for (int i = 0; i <= lastStart; ++i) {
        ++stats.checkedPositions;
        int j = 0;
        while (j < patLen) {
            ++stats.comparisons;
            if (arr[i + j] != pat[j]) {
                break;
            }
            ++j;
        }
        if (j == patLen) {
            stats.matchCount = 1;
            return i;
        }
    }

    return -1;
}

int findAllInt(
    const int arr[],
    int arrLen,
    const int pat[],
    int patLen,
    int outPositions[],
    int maxOutPositions,
    SearchStats& stats
) {
    stats = SearchStats{};

    if (patLen == 0) {
        if (maxOutPositions > 0) {
            outPositions[0] = 0;
            stats.matchCount = 1;
            return 1;
        }
        return 0;
    }
    if (arrLen <= 0 || patLen > arrLen) {
        return 0;
    }

    int found = 0;
    const int lastStart = arrLen - patLen;
    for (int i = 0; i <= lastStart; ++i) {
        ++stats.checkedPositions;
        int j = 0;
        while (j < patLen) {
            ++stats.comparisons;
            if (arr[i + j] != pat[j]) {
                break;
            }
            ++j;
        }
        if (j == patLen) {
            if (found < maxOutPositions) {
                outPositions[found] = i;
            }
            ++found;
        }
    }

    stats.matchCount = found;
    return found;
}

static bool arraysEqual(const int a[], int lenA, const int b[], int lenB) {
    if (lenA != lenB) {
        return false;
    }
    for (int i = 0; i < lenA; ++i) {
        if (a[i] != b[i]) {
            return false;
        }
    }
    return true;
}

static void printPositions(const int pos[], int count) {
    if (count <= 0) {
        std::cout << "немає";
        return;
    }
    for (int i = 0; i < count; ++i) {
        if (i > 0) {
            std::cout << ", ";
        }
        std::cout << pos[i];
    }
}

static void runAutomatedTests() {
    struct CharTestCase {
        const char* label;
        const char* text;
        const char* pattern;
        bool caseSensitive;
        int expectedFirst;
        int expectedAll[64];
        int expectedAllCount;
    };

    const CharTestCase charTests[] = {
        {"Тест 1",  "algorithm",      "algo",      true,  0,  {0}, 1},
        {"Тест 2",  "algorithm",      "rith",      true,  4,  {4}, 1},
        {"Тест 3",  "algorithm",      "thm",       true,  6,  {6}, 1},
        {"Тест 4",  "algorithm",      "sort",      true,  -1, {}, 0},
        {"Тест 5",  "aaaaaa",         "aa",        true,  0,  {0, 1, 2, 3, 4}, 5},
        {"Тест 6",  "abababaaba",     "aba",       true,  0,  {0, 2, 4}, 3},
        {"Тест 7",  "abcabcd",        "abcd",      true,  3,  {3}, 1},
        {"Тест 8",  "Data data DATA", "data",      true,  5,  {5}, 1},
        {"Тест 9",  "one two two",    "two",       true,  4,  {4, 8}, 2},
        {"Тест 12", "",               "abc",       true,  -1, {}, 0},
        {"Власний текстовий 1 (регістр)", "Data data DATA", "data", false, 0, {0, 5, 10}, 3},
        {"Власний текстовий 2 (накладання)", "aaaa", "aa", true, 0, {0, 1, 2}, 3},
        {"Порожній зразок", "data structures", "", true, 0, {0}, 1}
    };

    struct IntTestCase {
        const char* label;
        int arr[32];
        int arrLen;
        int pat[16];
        int patLen;
        int expectedFirst;
        int expectedAll[16];
        int expectedAllCount;
    };

    const IntTestCase intTests[] = {
        {"Тест 10", {1, 2, 1, 2, 1}, 5, {1, 2, 1}, 3, 0, {0, 2}, 2},
        {"Тест 11", {4, 5, 6}, 3, {5, 7}, 2, -1, {}, 0},
        {"Власний числовий 1 (накладання)", {7, 7, 7, 7}, 4, {7, 7}, 2, 0, {0, 1, 2}, 3},
        {"Власний числовий 2 (довший зразок)", {10, 20}, 2, {10, 20, 30}, 3, -1, {}, 0}
    };

    int passedFirst = 0;
    int passedAll = 0;
    const int totalTests = static_cast<int>(sizeof(charTests) / sizeof(charTests[0])) +
                           static_cast<int>(sizeof(intTests) / sizeof(intTests[0]));

    std::cout << "\n=== АВТОМАТИЗОВАНІ ТЕСТИ (17 ТЕСТІВ) ===\n";

    for (const auto& tc : charTests) {
        const int tLen = calcCharArrayLength(tc.text, 4096);
        const int pLen = calcCharArrayLength(tc.pattern, 4096);
        SearchStats s1{}, s2{};
        int actualAll[64]{};

        const int actualFirst = findFirstOccurrence(tc.text, tLen, tc.pattern, pLen, tc.caseSensitive, s1);
        const int actualAllCount = findAllOccurrences(tc.text, tLen, tc.pattern, pLen, tc.caseSensitive, actualAll, 64, s2);

        const bool okFirst = (actualFirst == tc.expectedFirst);
        const bool okAll = arraysEqual(actualAll, actualAllCount, tc.expectedAll, tc.expectedAllCount);

        if (okFirst) ++passedFirst;
        if (okAll) ++passedAll;

        std::cout << "[" << tc.label << "] Перше: " << (okFirst ? "OK" : "FAIL")
                  << " (" << actualFirst << " / очік. " << tc.expectedFirst << ")"
                  << " | Усі: " << (okAll ? "OK" : "FAIL")
                  << " (порівн. 1-й/усі: " << s1.comparisons << "/" << s2.comparisons << ")\n";
    }

    for (const auto& tc : intTests) {
        SearchStats s1{}, s2{};
        int actualAll[64]{};

        const int actualFirst = findFirstInt(tc.arr, tc.arrLen, tc.pat, tc.patLen, s1);
        const int actualAllCount = findAllInt(tc.arr, tc.arrLen, tc.pat, tc.patLen, actualAll, 64, s2);

        const bool okFirst = (actualFirst == tc.expectedFirst);
        const bool okAll = arraysEqual(actualAll, actualAllCount, tc.expectedAll, tc.expectedAllCount);

        if (okFirst) ++passedFirst;
        if (okAll) ++passedAll;

        std::cout << "[" << tc.label << "] Перше: " << (okFirst ? "OK" : "FAIL")
                  << " (" << actualFirst << " / очік. " << tc.expectedFirst << ")"
                  << " | Усі: " << (okAll ? "OK" : "FAIL")
                  << " (порівн. 1-й/усі: " << s1.comparisons << "/" << s2.comparisons << ")\n";
    }

    std::cout << "Підсумок: перше входження = " << passedFirst << "/" << totalTests
              << ", усі входження = " << passedAll << "/" << totalTests << "\n";
}

static void runScaleExperiment() {
    const int sizes[] = {100, 1000, 10000};
    const char* scenarios[] = {
        "A (на початку)",
        "B (у середині)",
        "C (у кінці)",
        "D (відсутній, довгі часткові збіги)"
    };
    const char pat[] = "aaaaab";
    const int patLen = calcCharArrayLength(pat, 64);

    std::cout << "\n=== ЕКСПЕРИМЕНТ З РОЗМІРОМ ТЕКСТУ (12 ЗАПУСКІВ) ===\n";
    std::cout << std::left << std::setw(8) << "Розмір"
              << std::setw(38) << "Сценарій"
              << std::setw(10) << "1-й ін."
              << std::setw(14) << "Порівн.(1-й)"
              << std::setw(12) << "Поз.(1-й)"
              << std::setw(14) << "Порівн.(усі)"
              << "Усі індекси\n";

    for (int n : sizes) {
        char base[10005]{};
        for (int k = 0; k < n; ++k) {
            base[k] = 'a';
        }
        base[n] = '\0';

        for (int s = 0; s < 4; ++s) {
            char work[10005]{};
            std::memcpy(work, base, static_cast<std::size_t>(n));
            work[n] = '\0';

            if (s == 0) {
                std::memcpy(work, pat, static_cast<std::size_t>(patLen));
            } else if (s == 1) {
                const int mid = n / 2;
                std::memcpy(work + mid, pat, static_cast<std::size_t>(patLen));
            } else if (s == 2) {
                const int endPos = n - patLen;
                std::memcpy(work + endPos, pat, static_cast<std::size_t>(patLen));
            }

            SearchStats stFirst{}, stAll{};
            int positions[256]{};
            const int firstIdx = findFirstOccurrence(work, n, pat, patLen, true, stFirst);
            const int allCount = findAllOccurrences(work, n, pat, patLen, true, positions, 256, stAll);

            std::cout << std::left << std::setw(8) << n
                      << std::setw(38) << scenarios[s]
                      << std::setw(10) << firstIdx
                      << std::setw(14) << stFirst.comparisons
                      << std::setw(12) << stFirst.checkedPositions
                      << std::setw(14) << stAll.comparisons;
            printPositions(positions, allCount);
            std::cout << "\n";
        }
    }
}

int main() {
    std::setlocale(LC_ALL, "");

    char variantText[512] = "data structures and data analysis";
    char variantPattern[128] = "data";
    const int tLen = calcCharArrayLength(variantText, 512);
    const int pLen = calcCharArrayLength(variantPattern, 128);

    SearchStats stFirstSens{}, stAllSens{};
    SearchStats stFirstInsens{}, stAllInsens{};
    int posSens[64]{};
    int posInsens[64]{};

    const int firstSens = findFirstOccurrence(variantText, tLen, variantPattern, pLen, true, stFirstSens);
    const int countSens = findAllOccurrences(variantText, tLen, variantPattern, pLen, true, posSens, 64, stAllSens);

    const int firstInsens = findFirstOccurrence(variantText, tLen, variantPattern, pLen, false, stFirstInsens);
    const int countInsens = findAllOccurrences(variantText, tLen, variantPattern, pLen, false, posInsens, 64, stAllInsens);

    std::cout << "=== РЕЗУЛЬТАТИ ПОШУКУ (ВАРІАНТ 1) ===\n";
    std::cout << "Текст: \"" << variantText << "\" (довжина " << tLen << ")\n";
    std::cout << "Зразок: \"" << variantPattern << "\" (довжина " << pLen << ")\n\n";

    std::cout << "[З урахуванням регістру]\n";
    std::cout << "  Перше входження: " << firstSens
              << " (порівнянь: " << stFirstSens.comparisons
              << ", перевірених позицій: " << stFirstSens.checkedPositions << ")\n";
    std::cout << "  Усі входження (" << countSens << "): ";
    printPositions(posSens, countSens);
    std::cout << " (порівнянь: " << stAllSens.comparisons
              << ", перевірених позицій: " << stAllSens.checkedPositions << ")\n\n";

    std::cout << "[Без урахування регістру]\n";
    std::cout << "  Перше входження: " << firstInsens
              << " (порівнянь: " << stFirstInsens.comparisons
              << ", перевірених позицій: " << stFirstInsens.checkedPositions << ")\n";
    std::cout << "  Усі входження (" << countInsens << "): ";
    printPositions(posInsens, countInsens);
    std::cout << " (порівнянь: " << stAllInsens.comparisons
              << ", перевірених позицій: " << stAllInsens.checkedPositions << ")\n";

    runAutomatedTests();
    runScaleExperiment();

    return 0;
}
