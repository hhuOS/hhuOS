#ifndef _LIBC_STDLIB
#define _LIBC_STDLIB

#include <stddef.h>

#define MB_LEN_MAX 4
#define MB_CUR_MAX 4


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
extern "C" unsigned long strtoul(const char* str, char **str_end, int base);
extern "C" double strtod(const char* str, char **str_end);

extern "C" double atof (const char* str);
extern "C" int atoi (const char *str);
extern "C" long atol (const char *str);

//multibyte strings 
extern "C" int mblen(const char* s, size_t n);
extern "C" int mbtowc(wchar_t * pwc, const char* s, size_t n);
extern "C" int wctomb(char * s, wchar_t wc);
extern "C" size_t mbstowcs(wchar_t * dst, const char * s, size_t len);
extern "C" size_t wcstombs(char* dst, const wchar_t * src, size_t len);

//program utility
extern "C" int system(const char* command);
extern "C" char * getenv(const char* name);

#endif