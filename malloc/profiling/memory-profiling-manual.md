# 1. Create a test program that implements these patterns:
```c test-allocator.c
// Core allocation patterns
void test_repeated_alloc_free(int count, size_t size);
void test_growing_allocs(int start_size, int growth_factor, int count);
void test_random_allocs(int min_size, int max_size, int count);
void test_edge_cases(void);
void test_fragmentation_pattern(void);

// Metrics collection
struct allocator_metrics {
    size_t total_allocations;
    size_t total_frees;
    size_t current_heap_size;
    size_t peak_heap_size;
    size_t failed_allocations;
    // Add more metrics
};
```

After writing a test program that utilizes `block_alloc()`, compile it against the memory allocator file, in this case: `block-alloc.c` so the test program uses it as a memory interface:
```bash
gcc -g test-allocator.c block-alloc.c -o test-allocator.out
```

# 2. Analysis Tools Usage:

## Cache Analysis:
To get the complete cache statistics, run cachegrind with these options:

```bash
valgrind --tool=cachegrind --cache-sim=yes --branch-sim=yes ./test-allocator
```
- This will create a cachegrind.out.PID file with the results
- Use cg_annotate: After running your program, use the cg_annotate tool to analyze the generated Cachegrind output file.  This tool can combine the results of multiple Cachegrind output files and can also perform a diff between two Cachegrind output files. The command to use cg_annotate is:
```
cg_annotate cachegrind.out.pid
```
- Interpret the Output: The output from cg_annotate includes various sections such as metadata, summary, and detailed file counts. The metadata section provides information about the invocation, events recorded, and other relevant details. The summary section gives an overview of the program's performance, and the file counts section provides detailed information about the performance of each function in the program.
- Analyze Cache Usage: If you enabled cache simulation, cg_annotate will also provide information about the cache usage, including cache misses and hits. This can help you identify areas of your code that may be inefficient in terms of cache utilization.
- Use Additional Tools: You can also use tools like KCachegrind, which is a KDE profiling frontend, to visualize and analyze the Cachegrind data. KCachegrind provides a graphical interface to explore the profiling data in more detail.

The valgrind output will track:
- D1 (L1 data cache) reads and misses (Dr, D1mr)
- D1 writes and misses (Dw, D1mw)
- LL (Last-level cache) reads and misses (DLmr)
- LL writes and misses (DLmw)
- I1 (instruction cache) reads and misses (Ir, I1mr)

The output of cg_annotate will show:
- I refs (instruction references)
- I1 misses (L1 instruction cache misses)
- D refs (data references - reads + writes)
- D1 misses (L1 data cache misses)
- LL misses (Last-level cache misses)
- Miss rates (as percentages)

A typical line in the output would show something like:
```
Ir  I1mr  ILmr  Dr  D1mr  DLmr  Dw  D1mw  DLmw
```

Where:
- Ir = instruction reads
- I1mr = L1 instruction cache misses
- ILmr = LL instruction cache misses
- Dr = data reads
- D1mr = L1 data cache misses
- DLmr = LL data cache misses
- Dw = data writes
- D1mw = L1 data cache write misses
- DLmw = LL data cache write misses

\*Note: hits aren't specifically mentioned, but you can caculate this easy since you have the misses and miss rates.


##  Heap Profile:
```bash
valgrind --tool=massif ./test-allocator
massif-visualizer massif.out.<pid>
```

##  Memory Errors:
```bash
valgrind --tool=memcheck --leak-check=full ./test-allocator
```

## Low-level Performance:
```bash
perf stat -e cache-misses,cache-references,cycles ./test-allocator
```
Note: perf is usually limited to root level privelages so you have two options:

1. Use sudo (quickest):
```bash
sudo perf stat -e cache-misses,cache-references,cycles ./test-allocator
```

2. Or adjust kernel permissions (more precise, permanent):
```bash
sudo sysctl -w kernel.perf_event_paranoid=-1
```
Then run perf normally without sudo.

Option 1 is simplest for quick tests. Option 2 is better for development environments where you'll use perf regularly.


# To measure fragmentation:
- Track total heap size vs actually used memory
- Implement a fragmentation calculator that walks the heap
- Track average free block size and distribution
