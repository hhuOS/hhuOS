/*
 * Copyright (C) 2018 Burak Akguel, Christian Gesse, Fabian Ruhland, Filip Krakowski, Michael Schoettner
 * Heinrich-Heine University; Olaf Spinczyk, TU Dortmund
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

#ifndef __OutputStream_include__
#define __OutputStream_include__

#include "lib/string/StringBuffer.h"

/**
 * Contains definitions for the C++ output-stream operator.
 * An implementation only needs to implement flush() for the operator to work.
 *
 * @author Olaf Spinczyk, TU Dortmund; Michael Schoettner, Fabian Ruhland, HHU
 * @date 2017
 */
class OutputStream : public StringBuffer {

public:

    int base;

public:

    /**
     * Constructor.
     */
    OutputStream ();

    /**
     * Copy-constructor.
     */
    OutputStream(const OutputStream &copy) = delete;

    /**
     * Destructor.
     */
    ~OutputStream() override = default;

    /**
     * Flushes the output buffer.
     * An implementation needs to write StringBuffer::buffer to its destination.
     */
    void flush() override = 0;

    /**
     * Write binary data to the output buffer.
     * This method won't write until it encounters a \0, as the stream operators do. Instead it writes as much bytes
     * as are specified by len, regardless of their value.
     *
     * @param data The data to be written.
     * @param len The amount of bytes to be written.
     */
    void writeBytes(char *data, uint64_t len);

    /**
     * Write a char.
     *
     * @param c The char.
     */
    OutputStream& operator << (char c);

    /**
     * Write a char.
     *
     * @param c The char.
     */
    OutputStream& operator << (unsigned char c);

    /**
     * Write a null-terminated string.
     * @param string The string.
     */
    OutputStream& operator << (char* string);

    /**
     * Write a null-terminated string.
     * @param string The string.
     */
    OutputStream& operator << (const char* string);

    /**
     * Write a short integer and display it according to the current base.
     *
     * @param ival The integer.
     */
    OutputStream& operator << (short ival);

    /**
     * Write an unsigned short integer and display it according to the current base.
     *
     * @param ival The integer.
     */
    OutputStream& operator << (unsigned short ival);

    /**
     * Write an float and display it according to the current base.
     *
     * @param ival The floating point number.
     */
    // OutputStream& operator << (double ival);

    /**
     * Write an integer and display it according to the current base.
     *
     * @param ival The integer.
     */
    OutputStream& operator << (int ival);

    /**
     * Write an unsigned integer and display it according to the current base.
     *
     * @param ival The integer.
     */
    OutputStream& operator << (unsigned int ival);

    /**
     * Write a long integer and display it according to the current base.
     *
     * @param ival The integer.
     */
    OutputStream& operator << (long ival);

    /**
     * Write an unsigned long integer and display it according to the current base.
     *
     * @param ival The integer.
     */
    OutputStream& operator << (unsigned long ival);

    /**
     * Write a pointer as hexadecimal nuber.
     *
     * @param ptr The pointer
     */
    OutputStream& operator << (void* ptr);

    /**
     * Manipulate the stream using the manipulator functions.
     */
    OutputStream& operator << (OutputStream& (*f) (OutputStream&));

};

/**
 * Line break.
 */
OutputStream& endl (OutputStream& os);

/**
 * Change the OutputStream's base to 2.
 * All numbers will then be displayed in binary notation.
 */
OutputStream& bin (OutputStream& os);

/**
 * Change the OutputStream's base to 8.
 * All numbers will then be displayed in octal notation.
 */
OutputStream& oct (OutputStream& os);

/**
 * Change the OutputStream's base to 10.
 * All numbers will then be displayed in decimal notation.
 */
OutputStream& dec (OutputStream& os);

/**
 * Change the OutputStream's base to 16.
 * All numbers will then be displayed in hexadecimal notation.
 */
OutputStream& hex (OutputStream& os);

#endif
