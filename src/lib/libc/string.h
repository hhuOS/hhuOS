/*
 * Copyright (C) 2017-2025 Heinrich Heine University Düsseldorf,
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

#ifndef HHUOS_LIB_LIBC_STRING
#define HHUOS_LIB_LIBC_STRING

#include <stddef.h>

#ifdef __cplusplus
extern "C" {
#endif

// String manipulation
char* strcpy(char *dest, const char *src);
char* strncpy(char *dest, const char *src, size_t count);
char* strcat(char *dest, const char *src);
char* strncat(char *dest, const char *src, size_t count);
size_t strxfrm(char *dest, const char *src, size_t count);
char* strdup(const char *str1);

// String examination
size_t strlen(const char *str);
int strcmp(const char *lhs, const char *rhs);
int strncmp(const char *lhs, const char *rhs, size_t count);
int strcasecmp(const char *lhs, const char *rhs);
int strncasecmp(const char *lhs, const char *rhs, size_t count);
int strcoll(const char *lhs, const char *rhs);
char* strchr(const char *str, int ch);
char* strrchr(const char *str, int ch);
size_t strspn(const char *dest, const char *src);
size_t strcspn(const char *dest, const char *src);
char* strpbrk(const char *dest, const char *breakset);
char* strstr(const char *str, const char *substr);
char* strtok(char *str, const char *delim);

// Memory manipulation
void* memchr(const void *ptr, int ch, size_t count);
int memcmp(const void *lhs, const void *rhs, size_t count);
void* memset(void *dest, int ch, size_t count);
void* memcpy(void *dest, const void *src, size_t count);
void* memmove(void *dest, const void *src, size_t count);

// Error handling
char* strerror(int errnum);

#ifdef __cplusplus
}
#endif

#endif