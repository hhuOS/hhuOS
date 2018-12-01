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

#ifndef __system_interface_include__
#define __system_interface_include__

#include "lib/libc/stdlib.h"

#ifdef __cplusplus
extern "C" {
#include "kernel/memory/manager/MemoryManager.h"
#endif


/**
 * Gets called by the system, after the necessary components (e.g. Memory management, Filesystem, etc.) have
 * been initialized.
 */
void sys_init_libc();

/**
 * Allocates a chunk of memory.
 *
 * @param size The chunk's desired size in bytes.
 *
 * @return A pointer to the allocated chunk.
 */
void *sys_alloc_mem(size_t size, uint32_t alignment);

/**
 * Frees a formerly allocated chunk of memory.
 *
 * @param ptr A pointer to the chunk.
 */
void sys_free_mem(void *);

void *sys_realloc_mem(void *ptr, size_t size, size_t alignment);

#ifdef __cplusplus
}
#endif 

#endif
