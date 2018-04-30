/*
 * Copyright (C) 2018 Burak Akguel, Christian Gesse, Fabian Ruhland, Filip Krakowski, Michael Schoettner
 * Heinrich-Heine University
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
 */

#ifndef __stdlib_include__
#define __stdlib_include__

#include <stdint.h>

#define EXIT_FAILURE    1
#define EXIT_SUCCESS    0

typedef unsigned int size_t;

/**
 * Allocates a chunk of memory of a given size.
 *
 * @param size The size in bytes.
 *
 * @return A pointer to the allocated chunk of memory.
 */
extern void *malloc(size_t size);

/**
 * Allocates a chunk of memory of a given size.
 *
 * @param size The size in bytes.
 *
 * @return A pointer to the allocated chunk of memory.
 */
extern void *aligned_alloc(uint32_t alignment, size_t size);

/**
 * Deallocates memory, that has been allocates using 'malloc(size_t)'.
 *
 * @param ptr A pointer to the chunk of memory, that shall be deallocated.
 */
extern void free(void *ptr);

/**
 * Changes the size of a chunk of memory, that has been allocated using 'malloc(size_t)'.
 * The contents of the chunk will be unchanged up to the lesser of the new and old sizes.
 * If 'ptr' is a null-pointer, this method behaves the same as 'malloc(size_t)'.
 *
 * @param ptr Points the chunk of memory whose size shall be changed.
 * @param size The desired size in bytes.
 *
 * @return A pointer to reallocated chunk of memory.
 */
extern void *realloc(void *ptr, size_t size);

#endif
