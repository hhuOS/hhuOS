#ifndef _LIBC_STDLIB
#define _LIBC_STDLIB

#include "lib/libc/stddef.h"

#define MB_LEN_MAX 4
#define MB_CUR_MAX 4

#ifdef __cplusplus
extern "C" {
#endif

//memory management
void *malloc(size_t size);
void *calloc(size_t num, size_t size);
void *realloc(void *ptr, size_t new_size);
void free(void *ptr);



//program execution
#define EXIT_SUCCESS 0
#define EXIT_FAILURE 1
void abort();
void exit(int exit_code);
int atexit(void (*func)(void));

//random
#define RAND_MAX 2147483647
int rand();
void srand( unsigned int seed );

//algorithms
void* bsearch(const void* key, const void* ptr, size_t count, size_t size, int (*comp)(const void*, const void*));
void qsort(void* ptr, size_t count, size_t size, int (*comp)(const void*, const void*));

//maths
typedef struct {int quot; int rem;} div_t; 
typedef struct {long quot; long rem;} ldiv_t;
int abs(int n);
int labs(long n);
div_t div( int x, int y);
ldiv_t ldiv ( long x, long y);

//string conversion
long strtol(const char* str, char **str_end, int base);
unsigned long strtoul(const char* str, char **str_end, int base);
double strtod(const char* str, char **str_end);

double atof (const char* str);
int atoi (const char *str);
long atol (const char *str);

//multibyte strings 
int mblen(const char* s, size_t n);
int mbtowc(wchar_t * pwc, const char* s, size_t n);
int wctomb(char * s, wchar_t wc);
size_t mbstowcs(wchar_t * dst, const char * s, size_t len);
size_t wcstombs(char* dst, const wchar_t * src, size_t len);

//program utility
int system(const char* command);
char * getenv(const char* name);


#ifdef __cplusplus
}
#endif

#endif