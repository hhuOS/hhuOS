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

#include "stdlib.h"
#include "string.h"
#include "system_interface.h"

#include <stddef.h>

void *malloc(size_t size) {
    void *ret = sys_alloc_mem(size, 0);
    if((int) ret == -1) {
        return NULL;
    }
    
    return ret;
}

void *aligned_alloc(unsigned int alignment, size_t size) {
    void *ret = sys_alloc_mem(size, alignment);
    if((int) ret == -1) {
        return NULL;
    }
    
    return ret;
}

void free(void *ptr) {
    sys_free_mem(ptr);
}

void *realloc(void *ptr, size_t size) {
    return sys_realloc_mem(ptr, size, 0);
}

void *aligned_realloc(void *ptr, size_t size, size_t alignment) {
    return sys_realloc_mem(ptr, size, alignment);
}
