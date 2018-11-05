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

#ifndef __lib_libc_printf_include__
#define __lib_libc_printf_include__

/// @todo first step to implement stdio or something like that

/// @todo did wie have to implement stdarg.h or cstdarg or float/double ?
#include <stdarg.h>
#include "lib/OutputStream.h"
#include "lib/lock/Spinlock.h"

#define SPECIFIER_LENGTH    2
#define PADDING_LENGTH      2

#ifdef __cplusplus
extern "C" {
#endif

extern Spinlock *printLock;
extern OutputStream *stdout;

void initPrintf();

/**
 * Write formatted data on kout or OutputStream.
 * After the format parameter, the function expects at least as many
 * additional arguments as needed for format.
 * 
 * @param format    C string that contains a format string that follows the same specifications as format in printf.
 * @param ...       Depending on the format string, the function may expect a sequence of additional arguments.
 * 
 * @return On success, the total number of characters written is returned. This count does not include the additional null-character automatically appended at the end of the string.
 * 
 */
void printf ( const char * format, ... );

#ifdef __cplusplus
}
#endif

#endif
