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

#include "PrintWriter.h"
#include "OutputStreamWriter.h"

namespace Util::Stream {

PrintWriter::PrintWriter(Writer &writer, bool autoFlush) : writer(writer), autoFlush(autoFlush), deleteWriter(false) {

}

PrintWriter::PrintWriter(OutputStream &outputStream, bool autoFlush) : writer(*new OutputStreamWriter(outputStream)), autoFlush(autoFlush), deleteWriter(true)  {

}

PrintWriter::~PrintWriter() {
    if (deleteWriter) {
        delete &writer;
    }
}

void PrintWriter::close() {
    writer.close();
}

void PrintWriter::flush() {
    writer.flush();
}

void PrintWriter::write(char c) {
    writer.write(c);
}

void PrintWriter::write(const char *sourceBuffer, uint32_t offset, uint32_t length) {
    writer.write(sourceBuffer, offset, length);
}

void PrintWriter::setBase(uint8_t newBase) {
    base = newBase;
}

void PrintWriter::print(const char *string) {
    for (uint32_t i = 0; string[i] != 0; i++) {
        write(string[i]);
    }
}

void PrintWriter::print(const Memory::String &string) {
    writer.write(string);
}

void PrintWriter::print(bool boolean) {
    writer.write(boolean ? "true" : "false");
}

void PrintWriter::print(int32_t number) {
    if (number < 0) {
        write('-');
        number = -number;
    }

    print(static_cast<uint32_t>(number));
}

void PrintWriter::print(uint32_t number) {
    uint32_t div;
    char digit;
    uint8_t currentBase = base;

    if (currentBase == 8)
        write('0');
    else if (currentBase == 16) {
        write('0');
        write('x');
    }

    for (div = 1; number / div >= currentBase; div *= currentBase);

    for (; div > 0; div /= currentBase) {
        digit = static_cast<char>(number / div);

        if (digit < 10) {
            write(static_cast<char>('0' + digit));
        }
        else {
            write(static_cast<char>('A' + digit - 10));
        }

        number %= div;
    }
}

void PrintWriter::print(int16_t number) {
    print(static_cast<int32_t>(number));
}

void PrintWriter::print(uint16_t number) {
    print(static_cast<uint32_t>(number));
}

void PrintWriter::print(int8_t number) {
    print(static_cast<uint32_t>(number));
}

void PrintWriter::print(uint8_t number) {
    print(static_cast<uint32_t>(number));
}

void PrintWriter::print(void *pointer) {
    print(reinterpret_cast<uint32_t>(pointer));
}

void PrintWriter::println() {
    write('\n');
    if (autoFlush) {
        writer.flush();
    }
}

void PrintWriter::println(const char *string) {
    print(string);
    println();
}

void PrintWriter::println(const Memory::String &string) {
    print(string);
    println();
}

void PrintWriter::println(bool boolean) {
    print(boolean);
    println();
}

void PrintWriter::println(int32_t number) {
    print(number);
    println();
}

void PrintWriter::println(uint32_t number) {
    print(number);
    println();
}

void PrintWriter::println(int16_t number) {
    print(number);
    println();
}

void PrintWriter::println(uint16_t number) {
    print(number);
    println();
}

void PrintWriter::println(int8_t number) {
    print(number);
    println();
}

void PrintWriter::println(uint8_t number) {
    print(number);
    println();
}

void PrintWriter::println(void *pointer) {
    print(pointer);
    println();
}

PrintWriter &PrintWriter::operator<<(char c) {
    write(c);
    return *this;
}

PrintWriter &PrintWriter::operator<<(const char *string) {
    print(string);
    return *this;
}

PrintWriter &PrintWriter::operator<<(const Memory::String &string) {
    print(string);
    return *this;
}

PrintWriter &PrintWriter::operator<<(int16_t number) {
    print(number);
    return *this;
}

PrintWriter &PrintWriter::operator<<(uint16_t number) {
    print(number);
    return *this;
}

PrintWriter &PrintWriter::operator<<(int32_t number) {
    print(number);
    return *this;
}

PrintWriter &PrintWriter::operator<<(uint32_t number) {
    print(number);
    return *this;
}

PrintWriter &PrintWriter::operator<<(void *pointer) {
    print(pointer);
    return *this;
}

PrintWriter &PrintWriter::operator<<(PrintWriter &(*f)(PrintWriter &)) {
    return f(*this);
}

PrintWriter &PrintWriter::endl(PrintWriter &writer) {
    writer.println();
    return writer;
}

PrintWriter &PrintWriter::bin(PrintWriter &writer) {
    writer.base = 2;
    return writer;
}

PrintWriter &PrintWriter::oct(PrintWriter &writer) {
    writer.base = 8;
    return writer;
}

PrintWriter &PrintWriter::dec(PrintWriter &writer) {
    writer.base = 10;
    return writer;
}

PrintWriter &PrintWriter::hex(PrintWriter &writer) {
    writer.base = 16;
    return writer;
}

}