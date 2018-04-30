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

#include "system_interface.h"

/**
 * Allocates a chunk of memory.
 * 
 * @param size The chunk's desired size in bytes.
 * 
 * @return A pointer to the allocated chunk.
 */
void *sys_alloc_mem(unsigned int size, unsigned int alignment) {
    // TODO
    //  Add alignment

    return new char[size];
}

/**
 * Frees a formerly allocated chunk of memory.
 * 
 * @param ptr A pointer to the chunk.
 */
void sys_free_mem(void *ptr) {
    delete (char *)ptr;
}
