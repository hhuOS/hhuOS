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

#ifndef HHUOS_LIB_LIBC_LIMITS
#define HHUOS_LIB_LIBC_LIMITS

#define CHAR_BIT 8
#define CHAR_MIN -128
#define CHAR_MAX 127
#define SCHAR_MIN -128
#define SCHAR_MAX 127
#define UCHAR_MAX 255

#define SHRT_MIN -32768
#define SHRT_MAX 32767
#define USHRT_MAX 65535

#define INT_MIN -2147483648
#define INT_MAX 2147483647
#define UINT_MAX 4294967295

#define LONG_MIN -2147483648
#define LONG_MAX 2147483647
#define ULONG_MAX 4294967295

#define LLONG_MIN -9223372036854775808
#define LLONG_MAX 9223372036854775807
#define ULLONG_MAX 18446744073709551615

#endif