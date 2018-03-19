/*****************************************************************************
 *                                                                           *
 *                              S T R I N G                                  *
 *                                                                           *
 *---------------------------------------------------------------------------*
 * Description:     Provides several functions, that are useful for          *
 *                  manipulating strings and other objects, that can be      *
 *                  treated as character-arrays.                             *
 *                                                                           *
 *                                                                           *
 * Author:          Fabian Ruhland, HHU, 16.10.2017                          *
 *****************************************************************************/

#ifndef __string_include__
#define __string_include__

typedef unsigned int    size_t;

void *memcpy(void *dest, const void *src, size_t n);
void *memmove(void *dest, const void *src, size_t n);
char *strcpy(char *dest, const char *src);
char *strcpy(char *dest, const char *src);
char *strncpy(char *dest, const char *src, size_t n);
char *strcat(char *s1, const char *s2);
char *strncat(char *s1, const char *s2, size_t n);
int memcmp(const void *s1, const void *s2, size_t n);
int strcmp(const char *s1, const char *s2);
int strncmp(const char *s1, const char *s2, size_t n);
void *memchr(const void *s, int c, size_t n);
char *strchr(const char *s, int c);
size_t strcspn(const char *s1, const char *s2);
char *strpbrk(const char *s1, const char *s2);
char *strrchr(const char *s, int c);
size_t strspn(const char *s1, const char *s2);
char *strstr(const char *s1, const char *s2);
char *strtok(char *s1, const char *s2);
void *memset(void *s, int c, size_t n);
char *strerror(int errnum);
int  strtoint(const char* s);
size_t strlen(const char *str);

#endif
