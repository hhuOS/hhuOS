/*
 * Copyright (C) 2018-2023 Heinrich-Heine-Universitaet Duesseldorf,
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

#ifndef HHUOS_PRINTSTREAM_H
#define HHUOS_PRINTSTREAM_H

#include <cstdint>

#include "OutputStream.h"
#include "lib/util/base/String.h"

namespace Util::Io {

class PrintStream : public OutputStream {

public:

    explicit PrintStream(OutputStream &stream, bool autoFlush = false);

    PrintStream(const PrintStream &copy) = delete;

    PrintStream &operator=(const PrintStream &copy) = delete;

    ~PrintStream() override = default;

    void write(uint8_t c) override;

    void write(const uint8_t *sourceBuffer, uint32_t offset, uint32_t length) override;

    void flush() override;

    void setBase(uint8_t newBase);

    void setNumberPadding(uint8_t padding);

    void print(const Util::String &string);

    void print(const char *string);

    void print(bool boolean);

    void print(int32_t number);

    void print(uint32_t number);

    void print(int16_t number);

    void print(uint16_t number);

    void print(int8_t number);

    void print(uint8_t number);

    void print(void *pointer);

    void println();

    void println(const Util::String &string);

    void println(const char *sourceBuffer);

    void println(bool boolean);

    void println(int32_t number);

    void println(uint32_t number);

    void println(int16_t number);

    void println(uint16_t number);

    void println(int8_t number);

    void println(uint8_t number);

    void println(void *pointer);

    PrintStream& operator<<(char c);

    PrintStream& operator<<(const char *string);

    PrintStream& operator<<(const String &string);

    PrintStream& operator<<(bool boolean);

    PrintStream& operator<<(int16_t value);

    PrintStream& operator<<(uint16_t value);

    PrintStream& operator<<(int32_t value);

    PrintStream& operator<<(uint32_t value);

    PrintStream& operator<<(void *ptr);

    PrintStream& operator<<(PrintStream& (*f)(PrintStream&));

    static PrintStream& flush(PrintStream& stream);

    static PrintStream& endl(PrintStream& stream);

    static PrintStream& bin(PrintStream& stream);

    static PrintStream& oct(PrintStream& stream);

    static PrintStream& dec(PrintStream& stream);

    static PrintStream& hex(PrintStream& stream);

private:

    OutputStream &stream;
    bool autoFlush;
    uint8_t base = 10;
    uint8_t numberPadding = 0;

};

}

#endif
