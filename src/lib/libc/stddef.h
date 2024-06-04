#ifndef LIBC_STDDEF
#define LIBC_STDDEF

#define NULL ((void*)0)
#define offsetof(TYPE, MEMBER) __builtin_offsetof(TYPE, MEMBER) //GCC specific

typedef unsigned long int size_t;

#ifndef __cplusplus
typedef unsigned long int wchar_t;
#endif

typedef unsigned long int ptrdiff_t;


#endif