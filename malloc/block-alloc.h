#ifndef BLOCK_ALLOC_H
#define BLOCK_ALLOC_H

#include <stddef.h>
#include <stdint.h>

typedef struct block_header {
    size_t size;      // Size of the block including header
    uint8_t is_free;  // 1 if block is free, 0 if allocated
    struct block_header *next; // Next block in memory
} block_header_t;

// Minimum block size that can be allocated (including header)
#define MIN_BLOCK_SIZE (sizeof(block_header_t) + 16)

// Public API
void *block_malloc(size_t size);
void block_free(void *ptr);

#endif // BLOCK_ALLOC_H
