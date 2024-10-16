/*
 * Copyright (C) 2018-2024 Heinrich-Heine-Universitaet Duesseldorf,
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

#include <stdarg.h>

#include "lib/interface.h"
#include "lib/util/io/stream/ByteArrayOutputStream.h"
#include "lib/util/io/stream/PrintStream.h"
#include "lib/util/collection/ArrayList.h"
#include "lib/util/base/Address.h"
#include "lib/util/base/Exception.h"
#include "lib/util/base/String.h"
#include "lib/util/collection/Array.h"

namespace Util {

String::String() noexcept {
    len = 0;
    buffer = new char[1];
    buffer[0] = '\0';
}

String::String(char character) noexcept {
    len = 1;
    buffer = new char[2];
    buffer[0] = character;
    buffer[1] = '\0';
}

String::String(const char *string) noexcept {
    auto address = Address<uint32_t>(string);

    if (string == nullptr) {
        len = 0;
    } else {
        len = address.stringLength();
    }

    buffer = new char[len + 1];
    Address<uint32_t>(buffer).copyRange(address, len);
    buffer[len] = '\0';
}

String::String(const uint8_t *data, uint32_t length) noexcept {
    auto address = Address<uint32_t>(data);

    len = length;
    buffer = new char[len + 1];
    Address<uint32_t>(buffer).copyRange(address, len);
    buffer[len] = '\0';
}

String::String(const String &other) noexcept {
    len = other.len;
    buffer = new char[len + 1];
    Address<uint32_t>(buffer).copyRange(Address<uint32_t>(other.buffer), len);
    buffer[len] = '\0';
}

String::~String() {
    delete[] buffer;
}

uint32_t String::hashCode() const {
    uint32_t hash = 0;

    for (uint32_t i = 0; i < len; i++) {
        hash += buffer[i];
    }

    return hash;
}

String String::substring(uint32_t begin) const {
    return substring(begin, length());
}

String String::substring(uint32_t begin, uint32_t end) const {
    if (begin > end || begin >= len) {
        return "";
    }

    if (end > len) {
        end = len;
    }

    uint32_t length = end - begin;
    char *newBuffer = new char[length + 1];
    Address<uint32_t>(newBuffer).copyRange(Address<uint32_t>(this->buffer + begin), length);
    newBuffer[length] = '\0';

    String ret(newBuffer);
    delete[] newBuffer;

    return ret;
}

Util::Array<String> String::split(const String &delimiter, uint32_t limit) const {
    Util::ArrayList<String> result;

    if (len == 0) {
        return result.toArray();
    }

    uint32_t start = 0;
    uint32_t end = indexOf(delimiter);
    String element;

    while (end != UINT32_MAX && result.size() + 1 != limit) {
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

bool String::isEmpty() const {
    return len == 0;
}

uint32_t String::indexOf(char character, uint32_t start) const {
    for (uint32_t i = start; i < len; i++) {
        if (buffer[i] == character) {
            return i;
        }
    }

    return UINT32_MAX;
}

uint32_t String::indexOf(const String &other, uint32_t start) const {
    uint32_t i;
    uint32_t j = 0;

    for (i = start; i < len; i++) {
        if (buffer[i] == other.buffer[0]) {
            for (j = 1; j < other.len; j++) {
                if (i + j >= len) {
                    return UINT32_MAX;
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

    return UINT32_MAX;

}


String String::remove(const String &string) const {
    uint32_t index = indexOf(string);

    if (index == UINT32_MAX) {
        return String(buffer);
    }

    return substring(0, index) + substring(index + string.len, len);
}

String String::removeAll(const String &string) const {
    Util::Array<String> tokens = split(string);
    String tmp;

    for (const String &token : tokens) {
        tmp += token;
    }

    return tmp;
}

bool String::beginsWith(const String &string) const {
    return substring(0, string.len) == string;
}

bool String::endsWith(const String &string) const {
    return substring(len - string.len, len) == string;
}


uint32_t String::length() const {
    return len;
}

bool String::operator==(const String &other) const {
    return Address<uint32_t>(buffer).compareString(Address<uint32_t>(other.buffer)) == 0;
}

bool String::operator!=(const String &other) const {
    return Address<uint32_t>(buffer).compareString(Address<uint32_t>(other.buffer)) != 0;
}

String &String::operator=(const String &other) {
    if (&other == this) {
        return *this;
    }

    delete buffer;
    buffer = nullptr;
    len = other.len;
    buffer = new char[len + 1];

    if (other.buffer != nullptr) {
        Address<uint32_t>(buffer).copyRange(Address<uint32_t>(other.buffer), len + 1);
    }

    return *this;
}

String &String::operator+=(const String &other) {
    buffer = static_cast<char*>(reallocateMemory(buffer, len + other.len + 1));
    Address<uint32_t>(buffer + len).copyRange(Address<uint32_t>(other.buffer), other.len + 1);
    len += other.len;

    return *this;
}

String operator+(const String &first, const String &second) {
    String tmp = first;
    tmp += second;

    return tmp;
}

String operator+(const String &first, char second) {
    String tmp = first;
    tmp += String(second);

    return tmp;
}

String operator+(const String &first, const char *second) {
    String tmp = first;
    tmp += String(second);

    return tmp;
}

String operator+(char first, const String &second) {
    String tmp(first);
    tmp += second;

    return tmp;
}

String operator+(const char *first, const String &second) {
    String tmp(first);
    tmp += second;

    return tmp;
}

String::operator char *() const {
    return buffer;
}

String::operator const char *() const {
    return buffer;
}

String::operator uint8_t *() const {
    return reinterpret_cast<uint8_t*>(buffer);
}

String::operator const uint8_t *() const {
    return reinterpret_cast<const uint8_t*>(buffer);
}

char String::operator[](uint32_t index) const {
    return buffer[index];
}

char &String::operator[](uint32_t index) {
    return buffer[index];
}

String::operator uint32_t() const {
    return hashCode();
}

String String::join(const String &separator, const Util::Array<String> &elements) {
    String tmp = "";
    uint32_t size = elements.length();

    for (uint32_t i = 0; i < size - 1; i++) {
        tmp += elements[i] + separator;
    }

    tmp += elements[size - 1];
    return tmp;
}

String String::toUpperCase() const {
    String tmp = *this;
    char c;

    for (uint32_t i = 0; i < len; i++) {
        c = tmp[i];

        if (isAlpha(tmp[i])) {
            if (c <= 'Z') {
                continue;
            }

            tmp[i] = c - CASE_OFFSET;
        }
    }

    return tmp;
}

String String::toLowerCase() const {
    String tmp = *this;
    char c;

    for (uint32_t i = 0; i < len; i++) {
        c = tmp[i];

        if (isAlpha(tmp[i])) {
            if (c >= 'a') {
                continue;
            }

            tmp[i] = c + CASE_OFFSET;
        }
    }

    return tmp;
}

bool String::isAlpha(const char c) {
    return (c >= 'A' && c <= 'Z') || (c >= 'a' && c <= 'z');
}

bool String::isNumeric(const char c) {
    return (c >= '0' && c <= '9');
}

bool String::contains(char c) const {
    return indexOf(c) < UINT32_MAX;
}

String String::strip() const {
    uint32_t startIndex;
    char element;

    for (startIndex = 0; startIndex < len; startIndex++) {
        element = buffer[startIndex];

        if (element != '\t' && element != '\n' && element != ' ') {
            if (startIndex == len - 1) {
                return buffer[startIndex];
            }
            break;
        }
    }

    uint32_t endIndex;
    for (endIndex = len - 1; endIndex > startIndex; endIndex--) {
        element = buffer[endIndex];

        if (element != '\t' && element != '\n' && element != ' ') {
            break;
        }
    }

    if (startIndex == endIndex && len > 1) {
        return "";
    }

    return substring(startIndex, endIndex + 1);
}

String String::format(const char *format, ...) {
    va_list args;
    va_start(args, format);

    String tmp = vformat(format, args);

    va_end(args);
    return tmp;
}

String String::vformat(const char *format, va_list args) {
    auto stream = Io::ByteArrayOutputStream();
    auto printStream = Io::PrintStream(stream);

    for (uint32_t i = 0; format[i] != '\0'; i++) {
        uint32_t j;
        for (j = 0; format[i + j] != '%' && format[i + j] != '\0'; j++) {}

        printStream.write(reinterpret_cast<const uint8_t*>(format), i, j);

        if (format[i + j] == '%') {
			printStream.setIntegerPrecision(0);
            if (format[i + j + 1] == '0') {
                uint8_t padding = format[i + j + 2] - '0';
                if (padding < 1 || padding > 9) {
                    Util::Exception::throwException(Util::Exception::INVALID_ARGUMENT, "String: Format padding must be between 01 and 09");
                }

                printStream.setIntegerPrecision(padding);
                j += 2;
            }

            char specifier = format[i + ++j];
            switch (specifier) {
                case 'c' :
                    printStream << static_cast<char>(va_arg(args, int32_t));
                    break;
                case 'd' :
                    printStream << Io::PrintStream::dec << va_arg(args, int32_t);
                    break;
                case 'u' :
                    printStream << Io::PrintStream::dec << va_arg(args, uint32_t);
                    break;
                case 'o':
                    printStream << Io::PrintStream::oct << va_arg(args, uint32_t);
                    break;
                case 's':
                    printStream << va_arg(args, char*);
                    break;
                case 'x':
                    printStream << Io::PrintStream::hex << va_arg(args, uint32_t);
                    break;
                case 'b':
                    printStream << Io::PrintStream::bin << va_arg(args, uint32_t);
                    break;
                case 'B':
                    printStream << static_cast<bool>(va_arg(args, uint32_t));
                    break;
                default:
                    Exception::throwException(Exception::INVALID_ARGUMENT, "String: Invalid format specifier!");
            }
        } else {
            break;
        }

        printStream.setNumberPadding(0);
        i += j;
    }

    return stream.getContent();
}

int32_t String::parseInt(const char *string) {
    int32_t length;
    for (length = 0; string[length] != '\0'; length ++) {}

    int32_t limit = 0;
    int32_t modifier = 1;
    int32_t result = 0;

    if (string[0] == '-') {
        limit = 1;
        modifier = -1;
    }

    int32_t j = 1;
    for(int32_t i = length - 1; i >= limit; i--) {
        if (isNumeric(string[i])) {
            result += (string[i] - '0') * j;
            j *= 10;
        }
    }

    return result * modifier;
}

int32_t String::parseInt(const String &string) {
    return parseInt(static_cast<const char*>(string));
}

int32_t String::parseHexInt(const char *string) {
    int32_t length;
    for (length = 0; string[length] != '\0'; length ++) {}

    int32_t result = 0;
    int32_t j = 1;
    for(int32_t i = length - 1; i >= 0; i--) {
        if (isNumeric(string[i])) {
            result += (string[i] - '0') * j;
            j *= 16;
        } else if (string[i] >= 'A' && string[i] <= 'F') {
            result += (10 + string[i] - 'A') * j;
            j *= 16;
        } else if (string[i] >= 'a' && string[i] <= 'f') {
            result += (10 + string[i] - 'a') * j;
            j *= 16;
        }
    }

    return result;
}

int32_t String::parseHexInt(const String &string) {
    return parseHexInt(static_cast<const char*>(string));
}

double String::parseDouble(const char *string) {
    return parseDouble(String(string));
}

double String::parseDouble(const String &string) {
    auto parts = string.split(".");
    int32_t firstNumber = parseInt(parts[0]);
    int32_t secondNumber = parts.length() > 1 ? parseInt(parts[1]) : 0;

    double exp = 1;
    if (parts.length() > 1) {
        for (uint32_t i = 0; i < parts[1].length(); i++) {
            exp *= 10;
        }
    }

    if (firstNumber < 0 || (firstNumber < 1 && string[0] == '-')) {
        return firstNumber - (secondNumber / exp);
    } else {
        return firstNumber + (secondNumber / exp);
    }
}

}