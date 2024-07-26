#ifndef LIBC_STRING
#define LIBC_STRING 

#include <stddef.h>

#ifdef __cplusplus
extern "C" {
#endif

//string manipulation
char * strcpy(char * dest, const char* src);
char * strncpy(char * dest, const char* src, size_t count);
char * strcat(char * dest, const char * src);
char * strncat(char * dest, const char * src, size_t count);
size_t strxfrm(char * dest, const char *src, size_t count);
char * strdup(const char * str1);

//string examination
size_t strlen(const char* str);
int strcmp(const char* lhs, const char* rhs);
int strncmp(const char* lhs, const char* rhs, size_t count);
int strcasecmp(const char* lhs, const char* rhs);
int strncasecmp(const char* lhs, const char* rhs, size_t count);
int strcoll(const char* lhs, const char* rhs);
char * strchr(const char * str, int ch);
char * strrchr(const char * str, int ch);
size_t strspn(const char* dest, const char* src);
size_t strcspn(const char *dest, const char * src);
char * strpbrk(const char * dest, const char *breakset);
char * strstr(const char * str, const char *substr);
char * strtok(char * str, const char * delim);

//memory manipulation 
void * memchr(const void* ptr, int ch, size_t count);
int memcmp(const void * lhs, const void* rhs, size_t count);
void * memset(void * dest, int ch, size_t count);
void * memcpy(void * dest, const void * src, size_t count);
void * memmove(void * dest, const void * src, size_t count);

//error handling
char * strerror(int errnum);


#ifdef __cplusplus
}
#endif

#endif