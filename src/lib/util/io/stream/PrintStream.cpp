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

#include "PrintStream.h"

#include "ByteArrayOutputStream.h"
#include "lib/util/io/stream/OutputStream.h"
#include "lib/util/base/Address.h"

namespace Util::Io {

PrintStream::PrintStream(OutputStream &stream, bool autoFlush) : stream(stream), autoFlush(autoFlush) {}

void PrintStream::flush() {
    stream.flush();
}

void PrintStream::write(const uint8_t *sourceBuffer, uint32_t offset, uint32_t length) {
    stream.write(sourceBuffer, offset, length);
}

void PrintStream::write(uint8_t c) {
    write(&c, 0, 1);
}

void PrintStream::setBase(uint8_t newBase) {
    base = newBase;
}

void PrintStream::setNumberPadding(uint8_t padding) {
    numberPadding = padding;
}

void PrintStream::print(const char *string) {
    write(reinterpret_cast<const uint8_t*>(string), 0, Address<uint32_t>(string).stringLength());
}

void PrintStream::print(const String &string) {
    write(static_cast<const uint8_t*>(string), 0, string.length());
}

void PrintStream::print(bool boolean) {
    print(boolean ? "true" : "false");
}

void PrintStream::print(int32_t number) {
    if (number < 0) {
        write('-');
        number = -number;
    }

    print(static_cast<uint32_t>(number));
}

void PrintStream::print(uint32_t number) {
    uint32_t div;
    char digit;
    uint8_t currentBase = base;

    auto numberStream = Io::ByteArrayOutputStream();
    auto numberstream = Io::PrintStream(numberStream);

    for (div = 1; number / div >= currentBase; div *= currentBase);

    for (; div > 0; div /= currentBase) {
        digit = static_cast<char>(number / div);

        if (digit < 10) {
            numberstream << static_cast<char>('0' + digit);
        }
        else {
            numberstream << static_cast<char>('A' + digit - 10);
        }

        number %= div;
    }

    for (uint32_t i = numberStream.getLength(); i < numberPadding; i++) {
        write('0');
    }

    print(numberStream.getContent());
}

void PrintStream::print(int16_t number) {
    print(static_cast<int32_t>(number));
}

void PrintStream::print(uint16_t number) {
    print(static_cast<uint32_t>(number));
}

void PrintStream::print(int8_t number) {
    print(static_cast<uint32_t>(number));
}

void PrintStream::print(uint8_t number) {
    print(static_cast<uint32_t>(number));
}

void PrintStream::print(void *pointer) {
    print(reinterpret_cast<uint32_t>(pointer));
}

void PrintStream::println() {
    write('\n');
    if (autoFlush) {
        stream.flush();
    }
}

void PrintStream::println(const char *string) {
    print(string);
    println();
}

void PrintStream::println(const String &string) {
    print(string);
    println();
}

void PrintStream::println(bool boolean) {
    print(boolean);
    println();
}

void PrintStream::println(int32_t number) {
    print(number);
    println();
}

void PrintStream::println(uint32_t number) {
    print(number);
    println();
}

void PrintStream::println(int16_t number) {
    print(number);
    println();
}

void PrintStream::println(uint16_t number) {
    print(number);
    println();
}

void PrintStream::println(int8_t number) {
    print(number);
    println();
}

void PrintStream::println(uint8_t number) {
    print(number);
    println();
}

void PrintStream::println(void *pointer) {
    print(pointer);
    println();
}

PrintStream& PrintStream::operator<<(char c) {
    write(c);
    return *this;
}

PrintStream& PrintStream::operator<<(const char *string) {
    print(string);
    return *this;
}

PrintStream& PrintStream::operator<<(const String &string) {
    print(string);
    return *this;
}

PrintStream& PrintStream::operator<<(bool boolean) {
    print(boolean);
    return *this;
}

PrintStream& PrintStream::operator<<(int16_t number) {
    print(number);
    return *this;
}

PrintStream& PrintStream::operator<<(uint16_t number) {
    print(number);
    return *this;
}

PrintStream& PrintStream::operator<<(int32_t number) {
    print(number);
    return *this;
}

PrintStream& PrintStream::operator<<(uint32_t number) {
    print(number);
    return *this;
}

PrintStream& PrintStream::operator<<(void *pointer) {
    print(pointer);
    return *this;
}

PrintStream& PrintStream::operator<<(PrintStream &(*f)(PrintStream &)) {
    return f(*this);
}

PrintStream& PrintStream::flush(PrintStream &stream) {
    stream.flush();
    return stream;
}

PrintStream& PrintStream::endl(PrintStream &stream) {
    stream.println();
    return stream;
}

PrintStream& PrintStream::bin(PrintStream &stream) {
    stream.setBase(2);
    return stream;
}

PrintStream& PrintStream::oct(PrintStream &stream) {
    stream.setBase(8);
    return stream;
}

PrintStream& PrintStream::dec(PrintStream &stream) {
    stream.setBase(10);
    return stream;
}

PrintStream& PrintStream::hex(PrintStream &stream) {
    stream.setBase(16);
    return stream;
}

}