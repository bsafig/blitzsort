# blitzsort
Using hardware-aware practices to optimize the quicksort algorithm.


### Build & Run
g++ -O3 -march=native -std=c++17 -o test_blitzsort.exe test_blitzsort.cpp
./test_blitzsort.cpp

### Results
- On random data: Small overhead from optimizations
- On worst-case data: 128x speedup by preventing O(n²)
- On practical data (mostly sorted): 1.5-2x speedup