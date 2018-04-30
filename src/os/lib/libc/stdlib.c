#include "stdlib.h"
#include "string.h"
#include "system_interface.h"

#include <stddef.h>

void *malloc(size_t size) {
    void *ret = sys_alloc_mem(size, 0);
    if((int) ret == -1) {
        return NULL;
    }
    
    return ret;
}

void *aligned_alloc(unsigned int alignment, size_t size) {
    void *ret = sys_alloc_mem(size, alignment);
    if((int) ret == -1) {
        return NULL;
    }
    
    return ret;
}

void free(void *ptr) {
    sys_free_mem(ptr);
}

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
