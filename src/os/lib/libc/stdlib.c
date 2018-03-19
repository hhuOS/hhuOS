#include "stdlib.h"
#include "string.h"
#include "system_interface.h"

#include <stddef.h>

/**
 * Allocates a chunk of memory of a given size.
 * 
 * @param size The size in bytes.
 * 
 * @return A pointer to the allocated chunk of memory.
 */
void *malloc(size_t size) {
    void *ret = sys_alloc_mem(size, 0);
    if((int) ret == -1) {
        return NULL;
    }
    
    return ret;
}

/**
 * Allocates a chunk of memory of a given size.
 * 
 * @param size The size in bytes.
 * 
 * @return A pointer to the allocated chunk of memory.
 */
void *aligned_alloc(unsigned int alignment, size_t size) {
    void *ret = sys_alloc_mem(size, alignment);
    if((int) ret == -1) {
        return NULL;
    }
    
    return ret;
}

/**
 * Deallocates memory, that has been allocates using 'malloc(size_t)'.
 * 
 * @param ptr A pointer to the chunk of memory, that shall be deallocated.
 */
void free(void *ptr) {
    sys_free_mem(ptr);
}

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
void *realloc(void *ptr, size_t size) {
    void *ret = sys_alloc_mem(size, 0);
    if((int) ret == -1) {
        return NULL;
    }
    
    if(ptr == NULL) {
        return ret;
    }
    
    if(size >= ((unsigned int *) ptr)[-1]) {
        memcpy(ret, ptr, ((unsigned int *) ptr)[-1]);
    } else {
        memcpy(ret, ptr, size);
    }
    
    sys_free_mem(ptr);
    return ret;
} 
