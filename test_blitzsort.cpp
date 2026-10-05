#include "blitzsort.h"
#include <iostream>
#include <vector>
#include <cstring>
#include <algorithm>
#include <chrono>
#include <random>
#include <iomanip>
#include <cmath>

using namespace std;
using namespace std::chrono;

template <typename T>
bool arrays_equal(const T *a, const T *b, int n)
{
    for (int i = 0; i < n; i++)
    {
        if (a[i] != b[i])
            return false;
    }
    return true;
}

void fill_random(int *arr, int n, int seed = 0)
{
    mt19937 gen(seed);
    uniform_int_distribution<> dis(0, 1000000);
    for (int i = 0; i < n; i++)
    {
        arr[i] = dis(gen);
    }
}

void fill_reverse(int *arr, int n)
{
    for (int i = 0; i < n; i++)
    {
        arr[i] = n - i;
    }
}

void fill_mostly_sorted(int *arr, int n, int seed = 0)
{
    for (int i = 0; i < n; i++)
    {
        arr[i] = i;
    }
    // Shuffle for some chaos
    mt19937 gen(seed);
    uniform_int_distribution<> dis(0, n - 1);
    int swaps = n / 20;
    for (int i = 0; i < swaps; i++)
    {
        int idx1 = dis(gen);
        int idx2 = dis(gen);
        swap(arr[idx1], arr[idx2]);
    }
}

// Benchmark sort
template <typename SortFunc>
double benchmark(SortFunc sort_func, int *arr, int n, int iterations = 1)
{
    int *temp = new int[n];
    auto start = high_resolution_clock::now();
    for (int iter = 0; iter < iterations; iter++)
    {
        memcpy(temp, arr, n * sizeof(int));
        sort_func(temp, n);
    }
    auto end = high_resolution_clock::now();
    delete[] temp;
    return duration<double, milli>(end - start).count() / iterations;
}

int main()
{
    cout << fixed << setprecision(2);
    cout << "\n=== Blitzsort Benchmark ===\n\n";

    // Correctness
    cout << "Test 1: Correctness Check\n";
    cout << string(50, '-') << "\n";
    {
        const int n = 10000;
        int *baseline = new int[n];
        int *optimized = new int[n];
        int *std_sorted = new int[n];

        fill_random(baseline, n, 42);
        memcpy(optimized, baseline, n * sizeof(int));
        memcpy(std_sorted, baseline, n * sizeof(int));

        blitzsort::quicksort_baseline(baseline, 0, n - 1);
        blitzsort::quicksort_optimized(optimized, 0, n - 1);
        std::sort(std_sorted, std_sorted + n);

        bool baseline_correct = arrays_equal(baseline, std_sorted, n);
        bool optimized_correct = arrays_equal(optimized, std_sorted, n);

        cout << "Baseline quicksort: " << (baseline_correct ? "PASS" : "FAIL") << "\n";
        cout << "Optimized quicksort: " << (optimized_correct ? "PASS" : "FAIL") << "\n";

        delete[] baseline;
        delete[] optimized;
        delete[] std_sorted;
    }

    // Performance on random data
    cout << "\nTest 2: Performance on Random Data\n";
    cout << string(50, '-') << "\n";
    cout << "Array Size    | Baseline (ms) | Optimized (ms) | std::sort (ms) | Speedup\n";
    cout << string(75, '-') << "\n";

    int sizes[] = {1000, 10000, 100000};
    for (int size : sizes)
    {
        int *data = new int[size];
        fill_random(data, size, 123);

        int iterations = size <= 10000 ? 5 : 1;

        double baseline_time = benchmark([](int *a, int n)
                                         { blitzsort::quicksort_baseline(a, 0, n - 1); }, data, size, iterations);

        double optimized_time = benchmark([](int *a, int n)
                                          { blitzsort::quicksort_optimized(a, 0, n - 1); }, data, size, iterations);

        double std_time = benchmark([](int *a, int n)
                                    { std::sort(a, a + n); }, data, size, iterations);

        double speedup = baseline_time / optimized_time;

        cout << setw(13) << size
             << "| " << setw(13) << baseline_time
             << "| " << setw(14) << optimized_time
             << "| " << setw(14) << std_time
             << "| " << setw(6) << speedup << "x\n";

        delete[] data;
    }

    // Worst-case behavior (reverse sorted)
    cout << "\nTest 3: Reverse-Sorted Data (Worst Case for Naive Quicksort)\n";
    cout << string(50, '-') << "\n";
    cout << "Array Size    | Baseline (ms) | Optimized (ms) | Status\n";
    cout << string(60, '-') << "\n";

    int reverse_sizes[] = {1000, 10000}; // Skip 100K+ (baseline is O(n²))
    for (int size : reverse_sizes)
    {
        int *data = new int[size];
        fill_reverse(data, size);

        int iterations = 5;

        double baseline_time = benchmark([](int *a, int n)
                                         { blitzsort::quicksort_baseline(a, 0, n - 1); }, data, size, iterations);

        double optimized_time = benchmark([](int *a, int n)
                                          { blitzsort::quicksort_optimized(a, 0, n - 1); }, data, size, iterations);

        double speedup = baseline_time / optimized_time;

        cout << setw(13) << size
             << "| " << setw(13) << baseline_time
             << "| " << setw(14) << optimized_time
             << "| " << setw(6) << speedup << "x faster\n";

        delete[] data;
    }
    cout << "(Larger sizes skipped: baseline is O(n²) on reverse-sorted)\n";

    // Mostly sorted data
    cout << "\nTest 4: Mostly-Sorted Data (Best Case for Cache Optimization)\n";
    cout << string(50, '-') << "\n";
    cout << "Array Size    | Baseline (ms) | Optimized (ms) | Speedup\n";
    cout << string(60, '-') << "\n";

    int mostly_sizes[] = {1000, 10000, 100000}; // Skip 1M (too slow)
    for (int size : mostly_sizes)
    {
        int *data = new int[size];
        fill_mostly_sorted(data, size, 456);

        int iterations = size <= 10000 ? 10 : (size <= 100000 ? 3 : 1);

        double baseline_time = benchmark([](int *a, int n)
                                         { blitzsort::quicksort_baseline(a, 0, n - 1); }, data, size, iterations);

        double optimized_time = benchmark([](int *a, int n)
                                          { blitzsort::quicksort_optimized(a, 0, n - 1); }, data, size, iterations);

        double speedup = baseline_time / optimized_time;

        cout << setw(13) << size
             << "| " << setw(13) << baseline_time
             << "| " << setw(14) << optimized_time
             << "| " << setw(6) << speedup << "x\n";

        delete[] data;
    }

    cout << "\n=== Summary ===\n";
    cout << "Cache optimization techniques used:\n";
    cout << "  1. Median-of-three pivot selection (reduces bad partitions)\n";
    cout << "  2. Insertion sort for small subarrays (better cache locality)\n";
    cout << "  3. Tail recursion optimization (reduces stack depth/space)\n";
    cout << "  4. Smaller partition first (keeps working set small)\n";
    cout << "\nThreshold: switch to insertion sort at array size 16.\n";
    cout << "\nExpected improvements:\n";
    cout << "  - Random data: 10-30% faster\n";
    cout << "  - Reverse data: 40-50% faster (median-of-three helps)\n";
    cout << "  - Mostly sorted: 60-80% faster (insertion sort on small chunks)\n";

    return 0;
}
