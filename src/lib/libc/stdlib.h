#ifndef _LIBC_STDLIB
#define _LIBC_STDLIB

#include <stddef.h>

//memory management
extern "C" void *malloc(size_t size);
extern "C" void *calloc(size_t num, size_t size);
extern "C" void *realloc(void *ptr, size_t new_size);
extern "C" void free(void *ptr);



//program execution
#define EXIT_SUCCESS 0
#define EXIT_FAILURE 1
extern "C" void abort();
extern "C" void exit(int exit_code);
extern "C" int atexit(void (*func)(void));

//random
#define RAND_MAX 2147483647
extern "C" int rand();
extern "C" void srand( unsigned int seed );

//algorithms
extern "C" void* bsearch(const void* key, const void* ptr, size_t count, size_t size, int (*comp)(const void*, const void*));
extern "C" void qsort(void* ptr, size_t count, size_t size, int (*comp)(const void*, const void*));

//maths
typedef struct {int quot; int rem;} div_t; 
typedef struct {long quot; long rem;} ldiv_t;
extern "C" int abs(int n);
extern "C" int labs(long n);
extern "C" div_t div( int x, int y);
extern "C" ldiv_t ldiv ( long x, long y);

//string conversion
extern "C" long strtol(const char* str, char **str_end, int base);
extern "C" long stroul(const char* str, char **str_end, int base);
extern "C" long strtod(const char* str, char **str_end);

extern "C" double atof (const char* str);
extern "C" int atoi (const char *str);
extern "C" long atol (const char *str);



#endif