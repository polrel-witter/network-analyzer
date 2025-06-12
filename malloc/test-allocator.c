#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include "block-alloc.h"

// Simple metrics
static size_t total_allocs = 0;
static size_t total_frees = 0;

// Helper to print metrics after each test
void print_test_metrics(const char* test_name) {
    printf("\n=== %s ===\n", test_name);
    printf("Total allocations: %zu\n", total_allocs);
    printf("Total frees: %zu\n", total_frees);
}

// Test 1: Simple repeated alloc/free
void test_simple_pattern() {
    printf("\nRunning simple alloc/free pattern...\n");

    void* ptrs[100];
    // Allocate and free immediately
    for (int i = 0; i < 100; i++) {
        ptrs[i] = block_malloc(16);
        total_allocs++;
        block_free(ptrs[i]);
        total_frees++;
    }

    print_test_metrics("Simple Pattern");
}

// Test 2: Growing allocations
void test_growing_pattern() {
    printf("\nRunning growing allocation pattern...\n");

    void* ptrs[10];
    // Allocate increasingly larger blocks
    for (int i = 0; i < 10; i++) {
        size_t size = 16 * (i + 1);  // 16, 32, 48, ...
        ptrs[i] = block_malloc(size);
        total_allocs++;
    }

    // Free in reverse order to test coalescing
    for (int i = 9; i >= 0; i--) {
        block_free(ptrs[i]);
        total_frees++;
    }

    print_test_metrics("Growing Pattern");
}

// Test 3: Fragmentation inducing pattern
void test_fragmentation() {
    printf("\nRunning fragmentation test...\n");

    void* ptrs[6];

    // Allocate 6 blocks
    for (int i = 0; i < 6; i++) {
        ptrs[i] = block_malloc(128);
        total_allocs++;
    }

    // Free alternate blocks to create gaps
    for (int i = 0; i < 6; i += 2) {
        block_free(ptrs[i]);
        total_frees++;
    }

    // Try to allocate a large block that should fail due to fragmentation
    void* large = block_malloc(384);
    if (large == NULL) {
        printf("Expected fragmentation observed: large allocation failed\n");
    }

    // Cleanup remaining blocks
    for (int i = 1; i < 6; i += 2) {
        block_free(ptrs[i]);
        total_frees++;
    }

    print_test_metrics("Fragmentation Test");
}

// Test 4: Edge cases
void test_edge_cases() {
    printf("\nTesting edge cases...\n");

    // Test zero size
    void* p1 = block_malloc(0);
    if (p1 == NULL) printf("Zero size allocation correctly returned NULL\n");

    // Test very small size
    void* p2 = block_malloc(1);
    total_allocs++;
    printf("Small allocation (1 byte) returned: %p\n", p2);

    // Test large size
    void* p3 = block_malloc(1024 * 1024);  // 1MB
    total_allocs++;
    printf("Large allocation (1MB) returned: %p\n", p3);

    // Cleanup
    block_free(p2);
    block_free(p3);
    total_frees += 2;

    print_test_metrics("Edge Cases");
}

// Test 5: Stress test
void test_stress() {
  void* ptrs[1000];
  for(int j = 0; j < 100; j++) {
      // Allocate
      for(int i = 0; i < 1000; i++) {
          ptrs[i] = block_malloc(64);  // Small consistent size
      }
      // Free
      for(int i = 0; i < 1000; i++) {
          block_free(ptrs[i]);
      }
  }
}

int main() {
    printf("Starting allocator tests...\n");

    test_simple_pattern();
    test_growing_pattern();
    test_fragmentation();
    test_edge_cases();
    test_stress();

    printf("\nFinal metrics:\n");
    printf("Total allocations: %zu\n", total_allocs);
    printf("Total frees: %zu\n", total_frees);
    printf("Leaked allocations: %zu\n", total_allocs - total_frees);

    return 0;
}
