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

#include <lib/util/List.h>
#include <lib/util/ArrayList.h>
#include <lib/libc/sprintf.h>
#include <lib/libc/printf.h>
#include "String.h"

extern "C" {
    #include "lib/libc/string.h"
    #include "lib/libc/stdlib.h"
}

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

    if(string == nullptr) {

        len = 0;

    } else {

        len = strlen(string);
    }
    
    buffer = new char[len + 1];

    memcpy(buffer, string, len);

    buffer[len] = '\0';
}

String::String(const String &other) noexcept {

    len = other.len;

    buffer = new char[len + 1];

    memcpy(buffer, other.buffer, len);

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

String String::substring(uint32_t begin, uint32_t end) const {

    if (begin > end || begin >= len) {
        return "";
    }

    if (end > len) {
        end = len;
    }

    uint32_t length = end - begin;

    char *buffer = new char[length + 1];

    memcpy(buffer, this->buffer + begin, length);

    String tmp = "";

    tmp.buffer = buffer;

    tmp.len = length;

    return tmp;
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

        if (!element.isEmpty()){
            result.add(element);
        }

        start = end + delimiter.len;

        end = indexOf(delimiter, start);
    }

    element = substring(start, len);

    if (!element.isEmpty()){
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

    uint32_t i = 0;
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
        return buffer;
    }

    return substring(0, index) + substring(index + string.len, len);
}

String String::removeAll(const String &string) const{

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


uint32_t String::length() const{
    return len;
}

bool String::operator==(const String &other) const {
    return strcmp(buffer, other.buffer) == 0;
}

bool String::operator!=(const String &other) const {
    return strcmp(buffer, other.buffer) != 0;
}

String &String::operator=(const String &other) {

    if (&other == this) {
        return *this;
    }

    delete buffer;

    buffer = nullptr;

    len = other.len;

    buffer = new char[len + 1];

    if(other.buffer != nullptr) {
        memcpy(buffer, other.buffer, len + 1);
    }

    return *this;
}

String &String::operator+=(const String &other) {

    buffer = (char*) realloc(buffer, len + other.len + 1);

    memcpy(buffer + len, other.buffer, other.len + 1);

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

    String tmp = first;

    tmp += second;

    return tmp;
}

String operator+(const char *first, const String &second) {

    String tmp = first;

    tmp += second;

    return tmp;
}

String::operator char*() const {
    return buffer;
}

OutputStream &operator<<(OutputStream &outStream, const String &string) {
    return outStream << string.buffer;
}

char String::operator[](uint32_t index) const {
    return buffer[index];
}

char &String::operator[](uint32_t index) {
    return buffer[index];
}

String::operator uint32_t() const {
    uint32_t hash = 0;

    for (uint32_t i = 0; i < len; i++) {
        hash += buffer[i];
    }

    return hash;
}

String String::join(const String &separator, const Util::Array<String> &elements) {

    String tmp = String();

    uint32_t size = elements.length();

    for (uint32_t i = 0; i < size - 1; i++) {
        tmp += elements[i] + separator;
    }

    tmp += elements[size - 1];

    return tmp;
}

String String::valueOf(bool value) {
    return value ? String("true") : String("false");
}

String String::valueOf(int32_t value, uint8_t radix) {

    char result[32];

    memset(result, 0, 32);

    switch (radix) {
        case 2:
            sprintf(result, "%b", value);
            break;
        case 8:
            sprintf(result, "%o", value);
            break;
        case 10:
            sprintf(result, "%d", value);
            break;
        case 16:
            sprintf(result, "%x", value);
            break;
        default:
            break;
    }

    return String(result);
}

String String::valueOf(uint32_t value, uint8_t radix, uint8_t padding) {

    char result[32];

    memset(result, 0, 32);

    char format[5];

    char *writer = &format[0];

    *writer++ = '%';

    if (padding > 0) {

        *writer++ = '0';

        *writer++ = padding + '0';
    }

    switch (radix) {
        case 2:
            *writer++ = 'b';
            break;
        case 8:
            *writer++ = 'o';
            break;
        case 10:
            *writer++ = 'u';
            break;
        case 16:
            *writer++ = 'x';
            break;
        default:
            break;
    }

    *writer++ = '\0';

    sprintf(result, format, value);

    return String(result);
}

String String::toUpperCase() {

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

String String::toLowerCase() {

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

String String::format(const char *fmt, ...) {

    va_list args;
    va_start(args, fmt);

    String tmp = vformat(fmt, args);

    va_end(args);

    return tmp;
}

String String::vformat(const char *fmt, va_list args) {

    Util::ArrayList<char> buffer;

    char numberBuffer[64];

    char specifier[SPECIFIER_LENGTH + PADDING_LENGTH + 1];

    const char* traverse = fmt;

    const char* string;

    uint32_t stringLength;

    while (*traverse != '\0') {

        while( *traverse != '%' ) {

            if (*traverse == '\0') {

                buffer.add('\0');

                Util::Array<char> output = buffer.toArray();

                return output.begin();
            }

            buffer.add(*traverse);

            traverse++;
        }

        memcpy(specifier, traverse, SPECIFIER_LENGTH + PADDING_LENGTH);

        traverse++;

        if (*traverse == '0') {

            traverse += 2;

            specifier[SPECIFIER_LENGTH + PADDING_LENGTH] = '\0';

        } else {

            specifier[SPECIFIER_LENGTH] = '\0';
        }

        traverse++;

        if (specifier[1] == 's') {

            string = va_arg(args, char *);

            stringLength = strlen(string);

            for (uint32_t i = 0; i < stringLength; i++) {

                buffer.add(string[i]);
            }

            continue;
        }

        sprintf(numberBuffer, specifier, args);

        stringLength = strlen(numberBuffer);

        for (uint32_t i = 0; i < stringLength; i++) {

            buffer.add(numberBuffer[i]);
        }
    }

    buffer.add('\0');

    return buffer.getArray();
}

String String::strip() {

    uint32_t startIndex = 0;

    char element;

    for (startIndex = 0; startIndex < len; startIndex++) {

        element = buffer[startIndex];

        if (element != '\t' && element != ' ') {

            break;
        }
    }

    uint32_t endIndex;

    for (endIndex = len - 1; endIndex > startIndex; endIndex--) {

        element = buffer[endIndex];

        if (element != '\t' && element != ' ') {

            break;
        }
    }

    if (startIndex == endIndex) {

        return "";
    }

    return substring(startIndex, endIndex + 1);
}