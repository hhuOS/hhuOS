#include "system_interface.h"

/**
 * Allocates a chunk of memory.
 * 
 * @param size The chunk's desired size in bytes.
 * 
 * @return A pointer to the allocated chunk.
 */
void *sys_alloc_mem(unsigned int size, unsigned int alignment) {
    // TODO
    //  Add alignment

    return new char[size];
}

/**
 * Frees a formerly allocated chunk of memory.
 * 
 * @param ptr A pointer to the chunk.
 */
void sys_free_mem(void *ptr) {
    delete (char *)ptr;
}
