#ifndef __stdlib_include__
#define __stdlib_include__

#include <stdint.h>

#define EXIT_FAILURE    1
#define EXIT_SUCCESS    0

typedef unsigned int size_t;

/**
 * Allocates a chunk of memory of a given size.
 *
 * @param size The size in bytes.
 *
 * @return A pointer to the allocated chunk of memory.
 */
extern void *malloc(size_t size);

/**
 * Allocates a chunk of memory of a given size.
 *
 * @param size The size in bytes.
 *
 * @return A pointer to the allocated chunk of memory.
 */
extern void *aligned_alloc(uint32_t alignment, size_t size);

/**
 * Deallocates memory, that has been allocates using 'malloc(size_t)'.
 *
 * @param ptr A pointer to the chunk of memory, that shall be deallocated.
 */
extern void free(void *ptr);

/**
 * Changes the size of a chunk of memory, that has been allocated using 'malloc(size_t)'.
 * The contents of the chunk will be unchanged up to the lesser of the new and old sizes.
 * If 'ptr' is a null-pointer, this method behaves the same as 'malloc(size_t)'.
 *
 * @param ptr Points the chunk of memory whose size shall be changed.
 * @param size The desired size in bytes.
 *
 * @return A pointer to reallocated chunk of memory.
 */
extern void *realloc(void *ptr, size_t size);

#endif
