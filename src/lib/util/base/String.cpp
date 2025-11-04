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
#include "WideChar.h"
#include "lib/interface.h"
#include "util/base/Address.h"
#include "util/base/Panic.h"
#include "util/collection/Array.h"
#include "util/collection/ArrayList.h"
#include "util/io/stream/ByteArrayOutputStream.h"
#include "util/io/stream/PrintStream.h"
#include "util/math/Math.h"

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
    if (begin >= end || begin >= len) {
        return "";
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

    String string = String::format(format, args);

    va_end(args);
    return string;
}

String String::format(const char *format, va_list args) {
    Io::ByteArrayOutputStream stream;

    const auto written = String::format(format, args, stream);
    if (written < 0) {
        Panic::fire(Panic::INVALID_ARGUMENT, "String: Invalid format string!");
    }

    return stream.getContent();
}

int32_t String::format(const char *format, va_list args, Io::OutputStream &target) {
    Io::PrintStream printStream(target);

    for (size_t i = 0; format[i] != '\0'; i++) {
        // Skip until a format specifier (starting with '%') is found
        while (format[i] != '%' && format[i] != '\0') {
            printStream.write(format[i]);
            i++;
        }

        if (format[i] == '\0') {
            break;
        }

        if (format[i++] == '%') {
            enum ParameterLength { SHORT, INT, LONG };

            ParameterLength parameterLength = INT;
            bool alternateConversion = false;
            bool zeroPadding = false;
            int32_t padding = -1;
            int32_t precision = -1;

            printStream.setBase(10);
            printStream.setNumberPadding(0);
            printStream.setIntegerPrecision(0);
            printStream.setDecimalPrecision(-1);
            printStream.setNumberJustification(false);
            printStream.setPositiveSign('\0');
            printStream.setNegativeSign('-');
            printStream.setIntegerPrefix("");
            printStream.setAlwaysPrintDecimalPoint(false);

            // Parse flags, width and precision modifiers
            while (true) {
                const auto c = format[i];
                if (c == '%') {
                    break;
                }

                if (c == 'h') {
                    parameterLength = SHORT;
                } else if (c == 'l' || c == 'L') {
                    parameterLength = LONG;
                } else if (c == '-') {
                    printStream.setNumberJustification(true);
                } else if (c == '+') {
                    printStream.setPositiveSign('+');
                } else if (c == ' ') {
                    printStream.setPositiveSign(' ');
                } else if (c == '#') {
                    alternateConversion = true;
                } else if (c == '0') {
                    zeroPadding = true;
                } else if (CharacterTypes::isDigit(c)) {
                    padding = parseNumber<int32_t>(format + i);
                    while (CharacterTypes::isDigit(format[i])) {
                        i++;
                    }
                    i--;
                } else if (c == '*') {
                    padding = va_arg(args, int32_t);
                    i++;
                } else if (c == '.') {
                    i++;
                    if (format[i] == '*') {
                        precision = va_arg(args, int32_t);
                    } else if (CharacterTypes::isDigit(format[i])) {
                        precision = parseNumber<int32_t>(format + i);
                        while (CharacterTypes::isDigit(format[i])) {
                            i++;
                        }
                        i--;
                    }
                } else {
                    break;
                }

                i++;
            }

            // Execute format specifier
            const auto c = format[i];
            switch (c) {
                case '%':
                    target.write('%');
                    break;
                case 'n': {
                    void *ptr = va_arg(args, void*);
                    switch (parameterLength) {
                        case SHORT:
                            *static_cast<short*>(ptr) = static_cast<short>(printStream.getBytesWritten());
                            break;
                        case LONG:
                            *static_cast<long*>(ptr) = static_cast<long>(printStream.getBytesWritten());
                            break;
                        default:
                            *static_cast<int*>(ptr) = static_cast<int>(printStream.getBytesWritten());
                    }
                    break;
                }
                case 'B': {
                    const auto value = static_cast<bool>(va_arg(args, size_t));
                    printStream.print(value ? "true" : "false", precision);
                    break;
                }
                case 'c':
                    if (parameterLength == LONG) {
                        char mbBuffer[4];
                        const auto mbLen = WideChar::wcharToUtf8(mbBuffer, static_cast<wchar_t>(va_arg(args, int)));
                        target.write(reinterpret_cast<const uint8_t*>(mbBuffer), 0, mbLen);
                    } else {
                        target.write(va_arg(args, int));
                    }
                    break;
                case 's':
                    if (parameterLength == LONG) {
                        wchar_t *wideString = va_arg(args, wchar_t*);

                        while (true) {
                            char mbBuffer[4];
                            const auto mbLen = Util::WideChar::wcharToUtf8(mbBuffer, *wideString++);
                            if (mbBuffer[0] == 0) {
                                break;
                            }

                            target.write(reinterpret_cast<const uint8_t*>(mbBuffer), 0, mbLen);
                        }
                    } else {
                        printStream.print(va_arg(args, char*), precision);
                    }
                    break;
                case 'd':
                case 'i':
                case 'u':
                case 'o':
                case 'x':
                case 'X':
                case 'p': {
                    if (c == 'p') {
                        precision = 8;
                    }

                    if (zeroPadding) {
                        precision = precision < 0 ? padding : precision + padding;
                        padding = 0;
                    }

                    if (padding >= 0) {
                        printStream.setNumberPadding(padding);
                    }

                    if (precision >= 0) {
                        printStream.setIntegerPrecision(precision);
                    }

                    if (c == 'o') {
                        printStream.setBase(8);
                        if (alternateConversion) {
                            printStream.setIntegerPrefix("0");
                        }
                    } else if (c == 'X') {
                        printStream.setBase(16);
                        printStream.setAlphaNumericBase('A');
                        if (alternateConversion) {
                            printStream.setIntegerPrefix("0X");
                        }
                    } else if (c == 'x' || c == 'p') {
                        printStream.setBase(16);
                        printStream.setAlphaNumericBase('a');
                        if (alternateConversion) {
                            printStream.setIntegerPrefix("0x");
                        }
                    } else {
                        printStream.setBase(10);
                    }

                    auto value = va_arg(args, size_t);
                    if (parameterLength == SHORT) {
                        value &= 0xffff;
                    }

                    if (precision == 0 && value == 0) {
                        if (c == 'o' && alternateConversion) {
                            target.write('0');
                        }

                        break; // Do not print anything, if precision is 0 and value is 0
                    }

                    if (c == 'd' || c == 'i') {
                        printStream.print(static_cast<int32_t>(value));
                    } else {
                        printStream.print(value);
                    }

                    break;
                }
                case 'f':
                case 'F':
                case 'e':
                case 'E':
                case 'g':
                case 'G': {
                    if (padding >= 0) {
                        printStream.setNumberPadding(padding);
                    }

                    if (precision >= 0) {
                        printStream.setDecimalPrecision(precision);
                    } else {
                        printStream.setDecimalPrecision(6);
                    }

                    if (alternateConversion) {
                        printStream.setAlwaysPrintDecimalPoint(true);
                    }

                    const auto value = va_arg(args, double);

                    if (Math::isNan(value)) {
                        printStream.print(CharacterTypes::isUpper(c) ? "NAN" : "nan");
                        break;
                    }

                    if (Math::isInfinity(value)) {
                        if (value > 0) {
                            printStream.print(CharacterTypes::isUpper(c) ? "INF" : "inf");
                        } else {
                            printStream.print(CharacterTypes::isUpper(c) ? "-INF" : "-inf");
                        }
                        break;
                    }

                    const auto exp = value == 0 ? 0 : static_cast<int32_t>(Math::floor(Math::log10(Math::absolute(value))));

                    bool scientificNotation = false;
                    if (c == 'e' || c == 'E') {
                        scientificNotation = true;
                    }

                    if (c == 'g' || c == 'G') {
                        if (precision == -1) {
                            precision = 6;
                        }
                        if (precision == 0) {
                            precision = 1;
                        }

                        if (precision > exp && exp >= -4) {
                            printStream.setDecimalPrecision(precision - exp - 1);
                            scientificNotation = false;
                        } else {
                            scientificNotation = true;
                            printStream.setDecimalPrecision(precision - 1);
                        }
                    }

                    if (!scientificNotation) {
                        printStream.print(value);
                    } else {
                        printStream.print(value / Math::pow(10.0, exp));
                        target.write(CharacterTypes::isLower(c) ? 'e' : 'E');
                        printStream.print(exp);
                    }

                    break;
                }
                default:
                    return -1;
            }
        }
    }

    return printStream.getBytesWritten();
}

String String::formatDate(const Time::Date &date, const char *format) {
    Io::ByteArrayOutputStream stream;

    const auto written = formatDate(date, stream, format);
    if (written < 0) {
        Panic::fire(Panic::INVALID_ARGUMENT, "String: Invalid format string!");
    }

    return stream.getContent();
}

int32_t String::formatDate(const Time::Date &date, Io::OutputStream &target, const char *format) {
    Io::PrintStream printStream(target);

    for (uint32_t i = 0; format[i] != 0; i++) {
        // Skip until a format specifier (starting with '%') is found
        size_t j = 0;
        while (format[i + j] != '%' && format[i + j] != '\0') {
            j++;
        }

        // Write the skipped part to the print stream
        printStream.write(reinterpret_cast<const uint8_t*>(format), i, j);

        i += j;
        if (format[i + j] == '\0') {
            break;
        }

        const auto c = format[++i];
        printStream.setIntegerPrecision(0);

        switch (c) {
            case '%':
                printStream.print('%');
                break;
            case 'Y':
                printStream.print(date.getYear());
                break;
            case 'y':
                printStream.print(date.getYear() % 100);
                break;
            case 'b':
                printStream.print(MONTH_ABBREVIATIONS[date.getMonth()]);
                break;
            case 'B':
                printStream.print(MONTH_NAMES[date.getMonth()]);
                break;
            case 'm':
                printStream.setIntegerPrecision(2);
                printStream.print(date.getMonth());
                break;
            case 'U':
                printStream.setIntegerPrecision(2);
                printStream.print(date.getWeekOfYearSunday());
                break;
            case 'W':
                printStream.setIntegerPrecision(2);
                printStream.print(date.getWeekOfYear());
                break;
            case 'j':
                printStream.setIntegerPrecision(3);
                printStream.print(date.getDayOfYear());
                break;
            case 'd':
                printStream.setIntegerPrecision(2);
                printStream.print(date.getDayOfMonth());
                break;
            case 'a':
                printStream.print(WEEKDAY_ABBREVIATIONS[date.getWeekday()]);
                break;
            case 'A':
                printStream.print(WEEKDAY_NAMES[date.getWeekday()]);
                break;
            case 'w':
                printStream.print((date.getWeekday() + 1 % 7));
                break;
            case 'H':
                printStream.setIntegerPrecision(2);
                printStream.print(date.getHours());
                break;
            case 'I':
                printStream.setIntegerPrecision(2);
                printStream.print((date.getHours() - 1) % 12 + 1);
                break;
            case 'M':
                printStream.setIntegerPrecision(2);
                printStream.print(date.getMinutes());
                break;
            case 'c':
                printStream.print(WEEKDAY_ABBREVIATIONS[date.getWeekday()]);
                printStream.print(' ');
                printStream.print(MONTH_ABBREVIATIONS[date.getMonth()]);
                printStream.print(' ');
                printStream.setIntegerPrecision(2);
                printStream.print(date.getDayOfMonth());
                printStream.print(' ');
                printStream.print(date.getHours());
                printStream.print(':');
                printStream.print(date.getMinutes());
                printStream.print(':');
                printStream.print(date.getSeconds());
                printStream.print(' ');
                printStream.setIntegerPrecision(4);
                printStream.print(date.getYear());
                break;
            case 'S':
                printStream.setIntegerPrecision(2);
                printStream.print(date.getSeconds());
                break;
            case 'x':
                printStream.setIntegerPrecision(2);
                printStream.print(date.getDayOfMonth());
                printStream.print('.');
                printStream.print(date.getMonth());
                printStream.print('.');
                printStream.print(date.getYear());
                break;
            case 'X':
                printStream.setIntegerPrecision(2);
                printStream.print(date.getHours());
                printStream.print(':');
                printStream.print(date.getMinutes());
                printStream.print(':');
                printStream.print(date.getSeconds());
                break;
            case 'p':
                printStream.print(date.getHours() > 12 ? "p.m." : "a.m.");
                break;
            case 'Z':
                printStream.print("UTC+0");
                break;
            default:
                return -1;
        }
    }

    return static_cast<int32_t>(printStream.getBytesWritten());
}

}
