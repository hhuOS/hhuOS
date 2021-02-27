/*
 * Copyright (C) 2021 Heinrich-Heine-Universitaet Duesseldorf,
 * Institute of Computer Science, Department Operating Systems
 *
 * This program is free software: you can redistribute it and/or modify it
 * under the terms of the GNU General Public License as published by the
 * Free Software Foundation, either version 3 of the License,
 * or (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.
 * See the GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program.  If not, see <http://www.gnu.org/licenses/>
 */

#ifndef HHUOS_OUTPUTSTREAM_H
#define HHUOS_OUTPUTSTREAM_H

#include <cstdint>

namespace Util::Stream {

/**
 * Interface for classes, that write output as a stream of bytes.
 * The C++ output stream operator can be used, to write strings. Numbers will be converted to their string representation before writing.
 *
 * The C++ output stream operator is based on work by Olaf Spinczyk, TU Dortmund (2017).
 */
class OutputStream {

public:

    OutputStream() = default;

    OutputStream(const OutputStream &copy) = delete;

    OutputStream &operator=(const OutputStream &copy) = delete;

    virtual ~OutputStream() = default;

    virtual void write(uint8_t c) = 0;

    virtual void write(const uint8_t *source, uint32_t offset, uint32_t length);

    virtual void flush();

    virtual void close();

    /**
     * Write the string representation of a character.
     *
     * @param c The char.
     */
    OutputStream& operator<<(char c);

    /**
     * Write the string representation of a null-terminated string.
     *
     * @param string The string.
     */
    OutputStream& operator<<(const char *string);

    /**
     * Covert a short integer.
     *
     * @param value The integer.
     */
    OutputStream& operator<<(int16_t value);

    /**
     * Write the string representation of an unsigned short integer.
     *
     * @param value The integer.
     */
    OutputStream& operator<<(uint16_t value);

    /**
     * Write the string representation of an integer.
     *
     * @param value The integer.
     */
    OutputStream& operator<<(int32_t value);

    /**
     * Write the string representation of an unsigned integer.
     *
     * @param value The integer.
     */
    OutputStream& operator<<(uint32_t value);

    /**
     * Write the string representation of a pointer (same as uint32_t).
     *
     * @param ptr The pointer
     */
    OutputStream& operator<<(void *ptr);

    /**
     * Manipulate the stream using the manipulator functions.
     */
    OutputStream& operator<<(OutputStream& (*f) (OutputStream&));

    /**
     * Line break.
     */
    static OutputStream& endl(OutputStream& os);

    /**
     * Change the OutputStream's base to 2.
     * All numbers will then be converted to binary notation.
     */
    static OutputStream& bin(OutputStream& os);

    /**
     * Change the OutputStream's base to 8.
     * All numbers will then be converted to octal notation.
     */
    static OutputStream& oct(OutputStream& os);

    /**
     * Change the OutputStream's base to 10.
     * All numbers will then be converted to decimal notation.
     */
    static OutputStream& dec(OutputStream& os);

    /**
     * Change the OutputStream's base to 16.
     * All numbers will then be converted to hexadecimal notation.
     */
    static OutputStream& hex(OutputStream& os);

private:

    Async::Atomic<uint8_t> base = Async::Atomic<uint8_t>(10);
};

}

#endif
