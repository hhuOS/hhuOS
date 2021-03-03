/*
 * Copyright (C) 2018-2021 Heinrich-Heine-Universitaet Duesseldorf,
 * Institute of Computer Science, Department Operating Systems
 * Burak Akguel, Christian Gesse, Fabian Ruhland, Filip Krakowski, Michael Schoettner
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

#ifndef HHUOS_STRINGFORMATOUTPUTSTREAM_H
#define HHUOS_STRINGFORMATOUTPUTSTREAM_H

#include <util/async/Atomic.h>
#include "FilterOutputStream.h"

namespace Util::Stream {

/**
 * Output stream, that provides operator overloads for several data types.
 * Values given to those operators will be converted to string representations before writing.
 *
 * The operator implementations are based on work by Olaf Spinczyk, TU Dortmund (2017).
 */
class StringFormatOutputStream : public FilterOutputStream {

public:

    explicit StringFormatOutputStream(OutputStream &stream);

    StringFormatOutputStream(const StringFormatOutputStream &copy) = delete;

    StringFormatOutputStream &operator=(const StringFormatOutputStream &copy) = delete;

    ~StringFormatOutputStream() override = default;

    void write(uint8_t c) override;

    void write(const uint8_t *source, uint32_t offset, uint32_t length) override;

    /**
     * Write the string representation of a character.
     *
     * @param c The char.
     */
    StringFormatOutputStream& operator<<(char c);

    /**
     * Write the string representation of a null-terminated string.
     *
     * @param string The string.
     */
    StringFormatOutputStream& operator<<(const char *string);

    /**
     * Write the string representation of a short integer.
     *
     * @param value The integer.
     */
    StringFormatOutputStream& operator<<(int16_t value);

    /**
     * Write the string representation of an unsigned short integer.
     *
     * @param value The integer.
     */
    StringFormatOutputStream& operator<<(uint16_t value);

    /**
     * Write the string representation of an integer.
     *
     * @param value The integer.
     */
    StringFormatOutputStream& operator<<(int32_t value);

    /**
     * Write the string representation of an unsigned integer.
     *
     * @param value The integer.
     */
    StringFormatOutputStream& operator<<(uint32_t value);

    /**
     * Write the string representation of a pointer (same as uint32_t).
     *
     * @param ptr The pointer
     */
    StringFormatOutputStream& operator<<(void *ptr);

    /**
     * Manipulate the stream using the manipulator functions.
     */
    StringFormatOutputStream& operator<<(StringFormatOutputStream& (*f)(StringFormatOutputStream&));

    /**
     * Line break.
     */
    static StringFormatOutputStream& endl(StringFormatOutputStream& stream);

    /**
     * Change the OutputStream's base to 2.
     * All numbers will then be converted to binary notation.
     */
    static StringFormatOutputStream& bin(StringFormatOutputStream& stream);

    /**
     * Change the OutputStream's base to 8.
     * All numbers will then be converted to octal notation.
     */
    static StringFormatOutputStream& oct(StringFormatOutputStream& stream);

    /**
     * Change the OutputStream's base to 10.
     * All numbers will then be converted to decimal notation.
     */
    static StringFormatOutputStream& dec(StringFormatOutputStream& stream);

    /**
     * Change the OutputStream's base to 16.
     * All numbers will then be converted to hexadecimal notation.
     */
    static StringFormatOutputStream& hex(StringFormatOutputStream& stream);

private:

    uint8_t base = 10;
};

}


#endif
