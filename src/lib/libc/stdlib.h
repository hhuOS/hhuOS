/*
 * Copyright (C) 2017-2026 Heinrich Heine University Düsseldorf,
 * Institute of Computer Science, Department Operating Systems
 * Main developers: Christian Gesse <christian.gesse@hhu.de>, Fabian Ruhland <ruhland@hhu.de>
 * Original development team: Burak Akguel, Christian Gesse, Fabian Ruhland, Filip Krakowski, Michael Schöttner
 * This project has been supported by several students.
 * A full list of integrated student theses can be found here: https://github.com/hhuOS/hhuOS/wiki/Student-theses
 *
 * This program is free software: you can redistribute it and/or modify it under the terms of the GNU General Public
 * License as published by the Free Software Foundation, either version 3 of the License, or (at your option) any
 * later version.
 *
 * This program is distributed in the hope that it will be useful, but WITHOUT ANY WARRANTY; without even the implied
 * warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU General Public License for more
 * details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program.  If not, see <http://www.gnu.org/licenses/>
 *
 * The stanard C library is based on a master's thesis, written by Tobias Fabian Oehme.
 * The original source code can be found here: https://github.com/ToboterXP/hhuOS/tree/thesis
 */

#ifndef HHUOS_LIB_LIBC_STDLIB
#define HHUOS_LIB_LIBC_STDLIB

#include <stddef.h>

#define MB_LEN_MAX 4
#define MB_CUR_MAX 4

#ifdef __cplusplus
extern "C" {
#endif

// Memory management
void* malloc(size_t size);
void* calloc(size_t num, size_t size);
void* realloc(void *ptr, size_t new_size);
void free(void *ptr);

// Program execution
#define EXIT_SUCCESS 0
#define EXIT_FAILURE 1

void abort();
void exit(int exit_code);
int atexit(void (*func)());

// Random
#define RAND_MAX 2147483647
int rand();
void srand(unsigned int seed);

// Algorithms
void* bsearch(const void *key, const void *ptr, size_t count, size_t size, int (*comp)(const void*, const void*));
void qsort(void *ptr, size_t count, size_t size, int (*comp)(const void*, const void*));

// Maths
typedef struct {
    int quot;
    int rem;
} div_t;

typedef struct {
    long quot;
    long rem;
} ldiv_t;

int abs(int n);
int labs(long n);
div_t div(int x, int y);
ldiv_t ldiv(long x, long y);

// String conversion
long strtol(const char *str, char **str_end, int base);
unsigned long strtoul(const char *str, char **str_end, int base);
double strtod(const char *str, char **str_end);

double atof (const char *str);
int atoi(const char *str);
long atol(const char *str);

// Multibyte strings
int mblen(const char *s, size_t n);
int mbtowc(wchar_t *pwc, const char *s, size_t n);
int wctomb(char *s, wchar_t wc);
size_t mbstowcs(wchar_t *dst, const char *s, size_t len);
size_t wcstombs(char *dst, const wchar_t *src, size_t len);

// Program utility
int system(const char *command);
char* getenv(const char *name);

#ifdef __cplusplus
}
#endif

#endif