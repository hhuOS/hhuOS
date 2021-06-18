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

#ifndef HHUOS_PRINTWRITER_H
#define HHUOS_PRINTWRITER_H

#include "Writer.h"
#include "OutputStream.h"

namespace Util::Stream {

class PrintWriter : public Writer {

public:

    explicit PrintWriter(Writer &writer, bool autoFlush = false);

    explicit PrintWriter(OutputStream &outputStream, bool autoFlush = false);

    PrintWriter(const PrintWriter &copy) = delete;

    PrintWriter &operator=(const PrintWriter &copy) = delete;

    ~PrintWriter() override;

    void flush() override;

    void write(const char *sourceBuffer, uint32_t offset, uint32_t length) override;

    void write(const char *sourceBuffer, uint32_t length) override;

    void write(char c) override;

    void write(const Util::Memory::String &string) override;

    void write(const Util::Memory::String &string, uint32_t offset, uint32_t length) override;

    void setBase(uint8_t newBase);

    void print(const Util::Memory::String &string);

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

    void println(const Util::Memory::String &string);

    void println(const char *sourceBuffer);

    void println(bool boolean);

    void println(int32_t number);

    void println(uint32_t number);

    void println(int16_t number);

    void println(uint16_t number);

    void println(int8_t number);

    void println(uint8_t number);

    void println(void *pointer);

    PrintWriter& operator<<(char c);

    PrintWriter& operator<<(const char *string);

    PrintWriter& operator<<(const Memory::String &string);

    PrintWriter& operator<<(bool boolean);

    PrintWriter& operator<<(int16_t value);

    PrintWriter& operator<<(uint16_t value);

    PrintWriter& operator<<(int32_t value);

    PrintWriter& operator<<(uint32_t value);

    PrintWriter& operator<<(void *ptr);

    PrintWriter& operator<<(PrintWriter& (*f)(PrintWriter&));

    static PrintWriter& flush(PrintWriter& writer);

    static PrintWriter& endl(PrintWriter& writer);

    static PrintWriter& bin(PrintWriter& writer);

    static PrintWriter& oct(PrintWriter& writer);

    static PrintWriter& dec(PrintWriter& writer);

    static PrintWriter& hex(PrintWriter& writer);

private:

    Writer &writer;
    bool autoFlush;
    bool deleteWriter;
    uint8_t base = 10;
};

}

#endif
