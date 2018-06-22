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

#ifndef __InputStream_include__
#define __InputStream_include__

#include "OutputStream.h"

/**
 * Contains definitions for the C++ input-stream operator.
 *
 * @author Fabian Ruhland, HHU
 * @date 2018
 */
class InputStream {

public:

    /**
     * Constructor.
     */
    InputStream() = default;

    /**
     * Copy-constructor.
     */
    InputStream(const InputStream &copy) = delete;

    /**
     * Destructor.
     */
    ~InputStream() = default;

    /**
     * Read an unsigned char from the stream.
     *
     * @param c Reference to the byte, in which the read char shall be stored
     */
    InputStream& operator >> (unsigned char &c);

    /**
     * Read a char from the stream.
     *
     * @param c Reference to the byte, in which the read char shall be stored
     */
    virtual InputStream& operator >> (char &c) = 0;

    /**
     * Read a string from the stream.
     *
     * The given char-pointer must not be allocated.
     * The implementation should allocate the memory itself.
     *
     * @param string Reference to the pointer, which shall point to the read string.
     */
    virtual InputStream& operator >> (char *&string) = 0;

    /**
     * Read data from the stream and directly write it to an output stream.
     *
     * @param outStream The output stream
     */
    virtual InputStream& operator >> (OutputStream &outStream) = 0;
};

#endif