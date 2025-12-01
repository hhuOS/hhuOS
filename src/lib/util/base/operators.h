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
 */

#ifndef HHUOS_LIB_UTIL_BASE_OPERATORS_H
#define HHUOS_LIB_UTIL_BASE_OPERATORS_H

#include <stddef.h>

/// Basic `new` operator.
void* operator new(size_t size);

/// Basic `new` operator for arrays.
void* operator new[](size_t size);

/// Placement `new` operator.
void* operator new(size_t size, void *pointer);

/// Placement `new` operator for arrays.
void* operator new[](size_t size, void *pointer);

/// Basic `delete` operator.
void operator delete(void*);

/// Basic `delete` operator for arrays.
void operator delete[](void*);

/// Placement `delete` operator.
void operator delete(void *pointer, size_t size);

/// Placement `delete` operator for arrays.
void operator delete[](void *pointer, size_t size);

#endif
