/*
 * Copyright (C) 2017-2025 Heinrich Heine University Düsseldorf,
 * Institute of Computer Science, Department Operating Systems
 * Main developers: Christian Gesse <christian.gesse@hhu.de>, Fabian Ruhland <ruhland@hhu.de>
 * Original development team: Burak Akguel, Christian Gesse, Fabian Ruhland, Filip Krakowski, Michael Schöttner
 * This project has been supported by several students.
 * A full list of integrated student theses can be found here: https://github.com/hhuOS/hhuOS/wiki/Student-theses
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

#include "String.h"

#include "CharacterTypes.h"
#include "lib/interface.h"
#include "base/Address.h"
#include "base/Panic.h"
#include "collection/Array.h"
#include "collection/ArrayList.h"
#include "io/stream/ByteArrayOutputStream.h"
#include "io/stream/PrintStream.h"

namespace Util {

String::String() {
    len = 0;
    buffer = new char[1];

    buffer[0] = '\0';
}

String::String(const char c) {
    len = 1;
    buffer = new char[2];

    buffer[0] = c;
    buffer[1] = '\0';
}

String::String(const char *string) {
    const auto address = Address(string);

    len = string == nullptr ? 0 : address.stringLength();
    buffer = new char[len + 1];

    Address(buffer).copyRange(address, len);
    buffer[len] = '\0';
}

String::String(const uint8_t *data, const size_t length) {
    const auto address = Address(data);

    len = length;
    buffer = new char[len + 1];

    Address(buffer).copyRange(address, len);
    buffer[len] = '\0';
}

String::String(const String &other) {
    len = other.len;
    buffer = new char[len + 1];

    Address(buffer).copyRange(Address(other.buffer), len);
    buffer[len] = '\0';
}

String::~String() {
    delete[] buffer;
}

bool String::operator==(const String &other) const {
    return Address(buffer).compareString(Address(other.buffer)) == 0;
}

bool String::operator!=(const String &other) const {
    return Address(buffer).compareString(Address(other.buffer)) != 0;
}

String &String::operator=(const String &other) {
    if (&other == this) {
        return *this;
    }

    delete buffer;
    len = other.len;
    buffer = new char[len + 1];

    Address(buffer).copyRange(Address(other.buffer), len + 1);

    return *this;
}

String& String::operator+=(const String &other) {
    buffer = static_cast<char*>(reallocateMemory(buffer, len + other.len + 1));
    Address(buffer + len).copyRange(Address(other.buffer), other.len + 1);
    len += other.len;

    return *this;
}

String operator+(const String &first, const String &second) {
    String ret = first;
    ret += second;

    return ret;
}

String operator+(const String &first, const char second) {
    String ret = first;
    ret += String(second);

    return ret;
}

String operator+(const String &first, const char *second) {
    String ret = first;
    ret += String(second);

    return ret;
}

String operator+(const char first, const String &string) {
    String ret(first);
    ret += string;

    return ret;
}

String operator+(const char *first, const String &second) {
    String ret(first);
    ret += second;

    return ret;
}

String::operator const char*() const {
    return buffer;
}

String::operator const uint8_t*() const {
    return reinterpret_cast<const uint8_t*>(buffer);
}

char String::operator[](const size_t index) const {
    return buffer[index];
}

char &String::operator[](const size_t index) {
    return buffer[index];
}

String::operator size_t() const {
    return hashCode();
}

size_t String::length() const {
    return len;
}

bool String::isEmpty() const {
    return len == 0;
}

size_t String::hashCode() const {
    size_t hash = 0;

    for (size_t i = 0; i < len; i++) {
        hash += buffer[i];
    }

    return hash;
}

size_t String::indexOf(const char character, const size_t start) const {
    for (size_t i = start; i < len; i++) {
        if (buffer[i] == character) {
            return i;
        }
    }

    return SIZE_MAX;
}

size_t String::indexOf(const String &other, const size_t start) const {
    size_t j = 0;

    for (size_t i = start; i < len; i++) {
        if (buffer[i] == other.buffer[0]) {
            for (j = 1; j < other.len; j++) {
                if (i + j >= len) {
                    return SIZE_MAX;
                }

                if (buffer[i + j] != other.buffer[j]) {
                    break;
                }
            }
        }

        if (j == other.len) {
            return i;
        }
    }

    return SIZE_MAX;
}

String String::substring(const size_t begin) const {
    return substring(begin, length());
}

String String::substring(const size_t begin, size_t end) const {
    if (begin > end || begin >= len) {
        return {};
    }

    if (end > len) {
        end = len;
    }

    const size_t length = end - begin;
    auto *newBuffer = new char[length + 1];
    Address(newBuffer).copyRange(Address(buffer + begin), length);
    newBuffer[length] = '\0';

    const auto ret = String(newBuffer);
    delete[] newBuffer;

    return ret;
}

String String::strip() const {
    size_t startIndex;
    char element;

    for (startIndex = 0; startIndex < len; startIndex++) {
        element = buffer[startIndex];

        if (element != '\t' && element != '\n' && element != ' ') {
            if (startIndex == len - 1) {
                return {buffer[startIndex]};
            }

            break;
        }
    }

    size_t endIndex;
    for (endIndex = len - 1; endIndex > startIndex; endIndex--) {
        element = buffer[endIndex];

        if (element != '\t' && element != '\n' && element != ' ') {
            break;
        }
    }

    if (startIndex == endIndex && len > 1) {
        return {};
    }

    return substring(startIndex, endIndex + 1);
}

Array<String> String::split(const String &delimiter) const {
    if (len == 0) {
        return Array<String>(0);
    }

    ArrayList<String> result;
    size_t start = 0;
    size_t end = indexOf(delimiter);
    String element;

    while (end != SIZE_MAX) {
        element = substring(start, end);

        if (!element.isEmpty()) {
            result.add(element);
        }

        start = end + delimiter.len;
        end = indexOf(delimiter, start);
    }

    element = substring(start, len);

    if (!element.isEmpty()) {
        result.add(element);
    }

    return result.toArray();
}


String String::remove(const String &string) const {
    const auto index = indexOf(string);

    if (index == SIZE_MAX) {
        return {buffer};
    }

    return substring(0, index) + substring(index + string.len, len);
}

String String::removeAll(const String &string) const {
    return join("", split(string));
}

bool String::beginsWith(const String &string) const {
    if (string.len > len) {
        return false;
    }

    for (size_t i = 0; i < string.len; i++) {
        if (buffer[i] != string.buffer[i]) {
            return false;
        }
    }

    return true;
}

bool String::endsWith(const String &string) const {
    if (string.len > len) {
        return false;
    }

    for (size_t i = 0; i < string.len; i++) {
        if (buffer[len - string.len + i] != string.buffer[i]) {
            return false;
        }
    }

    return true;
}

bool String::contains(const char c) const {
    return indexOf(c) < SIZE_MAX;
}

String String::join(const String &separator, const Array<String> &elements) {
    String ret{};
    const size_t size = elements.length();

    for (size_t i = 0; i < size - 1; i++) {
        ret += elements[i] + separator;
    }

    ret += elements[size - 1];
    return ret;
}

String String::toUpperCase() const {
    String ret = *this;

    for (size_t i = 0; i < len; i++) {
        const char c = ret[i];

        if (CharacterTypes::isAlphabet(c)) {
            if (c <= 'Z') {
                continue;
            }

            ret[i] = static_cast<char>(c - CASE_OFFSET);
        }
    }

    return ret;
}

String String::toLowerCase() const {
    String tmp = *this;

    for (size_t i = 0; i < len; i++) {
        const char c = tmp[i];

        if (CharacterTypes::isAlphabet(tmp[i])) {
            if (c >= 'a') {
                continue;
            }

            tmp[i] = static_cast<char>(c + CASE_OFFSET);
        }
    }

    return tmp;
}

String String::format(const char *format, ...) {
    va_list args;
    va_start(args, format);

    String tmp = vformat(format, args);

    va_end(args);
    return tmp;
}

String String::vformat(const char *format, va_list args) {
    Io::ByteArrayOutputStream stream{};
    Io::PrintStream printStream(stream);

    for (size_t i = 0; format[i] != '\0'; i++) {
        size_t j;
        for (j = 0; format[i + j] != '%' && format[i + j] != '\0'; j++) {}

        printStream.write(reinterpret_cast<const uint8_t*>(format), i, j);

        if (format[i + j] == '%') {
			printStream.setIntegerPrecision(0);
            if (format[i + j + 1] == '0') {
                const uint8_t padding = format[i + j + 2] - '0';
                if (padding < 1 || padding > 9) {
                    Util::Panic::fire(Panic::INVALID_ARGUMENT,
                        "String: Format padding must be between 01 and 09");
                }

                printStream.setIntegerPrecision(padding);
                j += 2;
            }

            const char specifier = format[i + ++j];
            switch (specifier) {
                case 'c' :
                    printStream << static_cast<char>(va_arg(args, long));
                    break;
                case 'd' :
                    printStream << Io::PrintStream::dec << va_arg(args, long);
                    break;
                case 'u' :
                    printStream << Io::PrintStream::dec << va_arg(args, size_t);
                    break;
                case 'o':
                    printStream << Io::PrintStream::oct << va_arg(args, size_t);
                    break;
                case 's':
                    printStream << va_arg(args, char*);
                    break;
                case 'x':
                    printStream << Io::PrintStream::hex << va_arg(args, size_t);
                    break;
                case 'b':
                    printStream << Io::PrintStream::bin << va_arg(args, size_t);
                    break;
                case 'B':
                    printStream << static_cast<bool>(va_arg(args, size_t));
                    break;
                default:
                    Panic::fire(Panic::INVALID_ARGUMENT, "String: Invalid format specifier!");
            }
        } else {
            break;
        }

        printStream.setNumberPadding(0);
        i += j;
    }

    return stream.getContent();
}

}