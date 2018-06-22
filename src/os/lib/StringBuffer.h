/*
 * Copyright (C) 2018 Burak Akguel, Christian Gesse, Fabian Ruhland, Filip Krakowski, Michael Schoettner, Olaf Spinczyk
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

#ifndef __StringBuffer_include__
#define __StringBuffer_include__

#include <cstdint>

/**
 * Implementation of a string buffer, that is automatically flushed to an output stream, once it is full.
 *
 * @author Olaf Spinczyk, TU Dortmund; Michael Schoettner, Fabian Ruhland, HHU
 * @date 2016
 */
class StringBuffer {

protected:
    char buffer[80];
    uint32_t pos;

public:
    /**
     * Constructor.
     */
    StringBuffer();

    /**
     * Copy-constructor.
     */
    StringBuffer(const StringBuffer &copy) = delete;

    /**
     * Destructor.
     */
    virtual ~StringBuffer() = default;

    /**
     * Append a char to the buffer.
     *
     * @param c The char
     */
    void put (char c);

    /**
     * Flush the buffer.
     */
    virtual void flush () = 0;
};

#endif
