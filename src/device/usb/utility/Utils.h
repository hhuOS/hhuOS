#ifndef UTILS
#define UTILS

#include "stdint.h"

#define PAGE_SIZE 4096

typedef uint32_t size_t;
typedef int32_t ssize_t;

#define SIZE_OF(obj) (char*)(&obj+1) - (char*)(&obj)

#define offsetof(TYPE, MEMBER) ((size_t) &((TYPE *)0)->MEMBER)
#define container_of(ptr, type, member) ({         \
    const typeof( ((type *)0)->member ) *__mptr = (ptr); \
    (type *)( (char *)__mptr - offsetof(type,member) );})

struct list_head{
    struct list_element* l_e;
};

struct list_element{
    struct list_element* l_e;
};

typedef struct list_head list_head;
typedef struct list_element list_element;

static inline void mem_set(uint8_t* p, unsigned int size, uint8_t val){
    for(unsigned int i = 0; i < size; i++){
        *(p + i) = val;
    }
}

static inline uint16_t floor_address(uint16_t interval){
    // get highest bit
    int pos = 0;
    while(interval != 0){
        pos++;
        interval >>= 1;
    }
    if(pos == 0) return 0;

    pos -= 1;

    uint16_t v = 1;

    while(pos > 0){
        v <<= 1;
        pos--;
    }

    return v;
}

#endif