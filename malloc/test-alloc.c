#include <stdio.h>
#include "block-alloc.h"

int main() {
    // Allocate some memory
    int *numbers = (int*)block_malloc(5 * sizeof(int));
    if (numbers) {
        // Use the allocated memory
        for (int i = 0; i < 5; i++) {
            numbers[i] = i + 1;
        }

        // Print the values
        for (int i = 0; i < 5; i++) {
            printf("%d ", numbers[i]);
        }
        printf("\n");

        // Free the memory when done
        block_free(numbers);
    }

    // Allocate a string
    char *str = (char*)block_malloc(20);
    if (str) {
        sprintf(str, "Hello, allocator!");
        printf("%s\n", str);
        block_free(str);
    }

    return 0;
}
