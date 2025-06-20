#include <unistd.h>
#include <string.h>
#include "block-alloc.h"

// Split block if remaining size would be at least MIN_BLOCK_SIZE
void split_block(block_header_t *block, size_t new_size) {
  if (block->size >= new_size + MIN_BLOCK_SIZE) {
      block_header_t *new_block = (block_header_t *)((uint8_t *)block + new_size);
      new_block->size = block->size - new_size;
      new_block->is_free = 1;
      new_block->next = block->next;

      block->size = new_size;
      block->next = new_block;
  }
}

// Try to merge block with next block if both are free
void coalesce_block(block_header_t *block) {
  while (block->next != NULL &&
         block->is_free &&
         block->next->is_free &&
         (uint8_t *)block + block->size == (uint8_t *)block->next) {

      block->size += block->next->size;
      block->next = block->next->next;
  }
}

// Head of the memory block list
block_header_t *heap_start = NULL;

// Initialize heap with initial block
block_header_t *init_heap(size_t initial_size) {
  block_header_t *block = (block_header_t *)sbrk(initial_size);
  if (block == (void*)-1) return NULL;

  block->size = initial_size;
  block->is_free = 1;
  block->next = NULL;

  heap_start = block;
  return block;
}

// Find first free block with sufficient size
block_header_t *find_free_block(size_t size) {
  if (!heap_start) {
      // Round up to page size for efficiency
      size_t heap_size = (size + 4095) & ~4095;
      return init_heap(heap_size);
  }

  // Cycle through existing blocks until we find one that's free and has enough room for the size that was passed in
  block_header_t *current = heap_start;
  while (current) {
      if (current->is_free && current->size >= size) {
          return current;
      }
      current = current->next;
  }

  // No suitable block found, extend heap
  size_t heap_size = (size + 4095) & ~4095; // Round to page size
  block_header_t *last = heap_start;
  while (last->next) last = last->next; // Get to the next block

  block_header_t *new_block = (block_header_t *)sbrk(heap_size);
  if (new_block == (void*)-1) return NULL;

  new_block->size = heap_size;
  new_block->is_free = 1;
  new_block->next = NULL;
  last->next = new_block;

  return new_block;
}

// Public API; allocate memory of given size
void *block_malloc(size_t size) {
  if (size == 0) return NULL;
	if (size > SIZE_MAX - sizeof(block_header_t)) return NULL;  // Check for overflow

  // Account for header size and alignment
  size_t total_size = size + sizeof(block_header_t); // adding number of bytes needed for the block_heater_t; i.e. padding for metadata
  // If total size requested is larger than min, only allocate up to min
  if (total_size < MIN_BLOCK_SIZE) {
      total_size = MIN_BLOCK_SIZE;
  }

  // Find a free block
  block_header_t *block = find_free_block(total_size);
  if (!block) return NULL;

  // Split block if it's too large
  split_block(block, total_size);

  // Mark block as allocated
  block->is_free = 0;

  // Return pointer to usable memory (after header)
  return (void *)(block + 1);
}

// Free allocated memory
void block_free(void *ptr) {
  if (!ptr) return;

  // Get block header
  block_header_t *block = ((block_header_t *)ptr) - 1;

  // Calculate user data size
  size_t user_size = block->size - sizeof(block_header_t);

  // Zero out all user data for security
  // using memset() since it's memory optimized
  memset(ptr, 0, user_size);

  block->is_free = 1;
  coalesce_block(block); // Try to merge with subsequent blocks
}
