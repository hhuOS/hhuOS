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

static inline void __mem_set(uint8_t* p, unsigned int size, uint8_t val){
    for(unsigned int i = 0; i < size; i++){
        *(p + i) = val;
    }
}

static inline void __mem_cpy(uint8_t* source, uint8_t* target, size_t size){
    for(unsigned int i = 0; i < size; i++){
        target[i] = source[i];
    }
}

static inline uint16_t __floor_address(uint16_t interval){
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

#define __STRUCT_INIT__(name, f1, f2, ...) \
    name->f1 = &f2; \
    name->f1(name, ## __VA_ARGS__)

#define __LIST_FOR_EACH__(name) \
    while (name != (void*)0)

#define __LIST_FOR_EACH_FINAL__(name) \
    while (name->l_e != (void*)0)

#define __LIST_TRAVERSE__(name) \
    __LIST_FOR_EACH_FINAL__(name) { \
        name = name->l_e; \
    } 

#define __FOR_RANGE__(name, dtype, start, end) \
    for(dtype name = start; name < end; name++)

#define __FOR_RANGE_INC__(name, dtype, start, end, inc) \
    for(dtype name = start; name < end; name+=inc)

#define __FOR_RANGE_DEC__(name, dtype, start, end, dec) \
    for(dtype name = start; name >= end; name-=dec)

#define __FOR_RANGE_COND__(name, dtype, start, condition, update) \
    for(dtype name = start; condition; update)

#define __NEW__(mem_service, type, size, name, fptr, func, ...) \
    struct type* name = __ALLOC_KERNEL_MEM__(mem_service, type, size); \
    __STRUCT_INIT__(name, fptr, func, ## __VA_ARGS__)

#define __NEW_ALIGN__(mem_service, type, size, align, name, fptr, func, ...) \
    struct type* name = __ALLOC_KERNEL_MEM__(mem_service, type, size, align); \
    __STRUCT_INIT__(name, fptr, funct, ## __VA_ARGS__)

#define __SUPER__(name, field) \
    name->super.field

#define __ENTRY__(name, field) \
    name->field

#define __CALL_SUPER__(super, super_function, ...) \
    super.super_function(&super, ## __VA_ARGS__)

#define __DECLARE_STRUCT__(type, name) \
    struct type name

#define __CONCAT__(A,B) \
    A ## B

#define __STRINGIZE__(A) #A

#define __ARR_ENTRY__(arr, pos, value) \
    arr[pos] = value

#define __CAST__(cast, type) \
    ((cast)type)

#define __BYTE__ uint8_t
#define __WORD__ uint16_t
#define __D_WORD__ uint32_t

#define __UHC__ UsbController

#define __STRUCT_CALL__(str, function, ...) \
    str->function(str, ## __VA_ARGS__)

#define __PTR__ uintptr_t

#define __PTR_TYPE__(type, addr) \
    (type)(__PTR__)(addr)

#define __GET_FROM_SUPER__(super, field) \
    super->field

#define __SET_IN_SUPER__(super, field, cmd) \
    super->field = cmd
    
#define __FUNC_CALL__(proto, ...) \
    proto(__VA_ARGS__)

#define __DECLARE_GET__(type, f_name, member, container_type, keyword) \
    __FUNC_CALL__(keyword type get_ ## f_name, container_type c)  {     \
        return c->member;                                     \
    } 

#define __DECLARE_SET__(f_name, member, container_type, member_type, keyword) \
    __FUNC_CALL__(keyword void set_ ## f_name, container_type c, member_type m) { \
        c->member = m;                                                     \
    }

#define __DECLARE_STRUCT_GET__(f_name, container_type, type) \
    type (*get_ ## f_name)(container_type c)
#define __DECLARE_STRUCT_SET__(f_name, container_type, member_type) \
    void (*set_ ## f_name)(container_type c, member_type m)

#define __RET_N__  0    
#define __RET_E__ -1
#define __RET_S__  1

#define __IF_EXT__(condition, expr1, expr2) \
    condition ? expr1 : expr2

#define __IF_CONTINUE__(condition) \
    if(condition) {continue;}

#define __LIST_FIRST_ENTRY__(list_h) \
    list_h.l_e

#define __LIST_NEXT_ELEMENT__(list_e) \
    list_e->l_e

#define __LIST_NEXT_HEAD__(list_h) \
    list_h.l_e->l_e

#define __LIST_ADD_FIRST_ENTRY__(list_h, list_e) \
    __LIST_FIRST_ENTRY__(list_h) = list_e

#define __LIST_NEXT_ENTRY__(list_e, list_e_next) \
    __LIST_NEXT_ELEMENT__(list_e) = list_e_next

#define __IF_LIST_ENTRY_NULL(list_e) \
    if (list_e == (void*)0)

#define __IF_LIST_HEAD_NULL(list_h) \
    if(__LIST_FIRST_ENTRY__(list_h) == (void*)0)

#define __UPDATE_LIST_ENTRY__(list_e) \
    list_e = __LIST_NEXT_ELEMENT__(list_e)

#endif