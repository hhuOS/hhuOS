#ifndef __system_interface_include__
#define __system_interface_include__

#include "lib/libc/stdlib.h"

#ifdef __cplusplus
extern "C" {
    #include "kernel/memory/manager/HeapMemoryManager.h"
#endif


/**
 * Allocates a chunk of memory.
 *
 * @param size The chunk's desired size in bytes.
 *
 * @return A pointer to the allocated chunk.
 */
void *sys_alloc_mem(size_t size, uint32_t alignment);

/**
 * Frees a formerly allocated chunk of memory.
 *
 * @param ptr A pointer to the chunk.
 */
void sys_free_mem(void *);

#ifdef __cplusplus
}
#endif 

#endif
