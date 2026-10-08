#ifndef BLITZSORT_H
#define BLITZSORT_H

#include <cstring>
#include <algorithm>

namespace blitzsort
{
    // Baseline quicksort
    template <typename T>
    int partition_baseline(T *arr, int low, int high)
    {
        T pivot = arr[high];
        int i = low - 1;
        for (int j = low; j < high; j++)
        {
            if (arr[j] < pivot)
            {
                i++;
                std::swap(arr[i], arr[j]);
            }
        }
        std::swap(arr[i + 1], arr[high]);
        return i + 1;
    }

    template <typename T>
    void quicksort_baseline(T *arr, int low, int high)
    {
        if (low < high)
        {
            int pi = partition_baseline(arr, low, high);
            quicksort_baseline(arr, low, pi - 1);
            quicksort_baseline(arr, pi + 1, high);
        }
    }

    // Insertion sort for small arrays. Better cache behavior than quicksort
    template <typename T>
    void insertion_sort(T *arr, int low, int high)
    {
        for (int i = low + 1; i <= high; i++)
        {
            T key = arr[i];
            int j = i - 1;
            while (j >= low && arr[j] > key)
            {
                arr[j + 1] = arr[j];
                j--;
            }
            arr[j + 1] = key;
        }
    }

    // Cache-optimized quicksort: uses insertion sort for small subarrays
    // and better pivot selection
    template <typename T>
    int partition_optimized(T *arr, int low, int high)
    {
        // Median-of-three pivot selection reduces bad partitions
        int mid = low + (high - low) / 2;
        if (arr[low] > arr[mid])
            std::swap(arr[low], arr[mid]);
        if (arr[low] > arr[high])
            std::swap(arr[low], arr[high]);
        if (arr[mid] > arr[high])
            std::swap(arr[mid], arr[high]);
        std::swap(arr[mid], arr[high]);

        T pivot = arr[high];
        int i = low - 1;
        for (int j = low; j < high; j++)
        {
            if (arr[j] < pivot)
            {
                i++;
                std::swap(arr[i], arr[j]);
            }
        }
        std::swap(arr[i + 1], arr[high]);
        return i + 1;
    }

    template <typename T>
    void quicksort_optimized_impl(T *arr, int low, int high)
    {
        // Threshold: switch to insertion sort for small arrays
        // 24 is optimal for insertion sort across random and mostly-sorted data (tested with experiment)
        const int INSERTION_THRESHOLD = 24;

        while (low < high)
        {
            if (high - low < INSERTION_THRESHOLD)
            {
                insertion_sort(arr, low, high);
                break;
            }

            int pi = partition_optimized(arr, low, high);

            // Tail recursively sort the smaller partition first to reduce stack depth
            if (pi - low < high - pi)
            {
                quicksort_optimized_impl(arr, low, pi - 1);
                low = pi + 1;
            }
            else
            {
                quicksort_optimized_impl(arr, pi + 1, high);
                high = pi - 1;
            }
        }
    }

    template <typename T>
    void quicksort_optimized(T *arr, int low, int high)
    {
        quicksort_optimized_impl(arr, low, high);
    }

}

#endif
