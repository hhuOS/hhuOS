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

#ifndef __String_include__
#define __String_include__

#include <cstdint>
#include <cstdarg>
#include "lib/util/collection/Array.h"

namespace Util {

/**
 * @author Filip Krakowski
 */
class String {

public:

    String() noexcept;

    String(char character) noexcept;

    String(const char string[]) noexcept;

    String(const uint8_t *data, uint32_t length) noexcept;

    String(const String &other) noexcept;

    ~String();

    [[nodiscard]] uint32_t hashCode() const;

    [[nodiscard]] uint32_t length() const;

    [[nodiscard]] uint32_t indexOf(char character, uint32_t start = 0) const;

    [[nodiscard]] uint32_t indexOf(const String &other, uint32_t start = 0) const;

    [[nodiscard]] bool isEmpty() const;

    [[nodiscard]] String substring(uint32_t begin) const;

    [[nodiscard]] String substring(uint32_t begin, uint32_t end) const;

    [[nodiscard]] String strip() const;

    [[nodiscard]] Array<String> split(const String &delimiter, uint32_t limit = 0) const;

    [[nodiscard]] String remove(const String &string) const;

    [[nodiscard]] String removeAll(const String &string) const;

    [[nodiscard]] bool beginsWith(const String &string) const;

    [[nodiscard]] bool endsWith(const String &string) const;

    [[nodiscard]] bool contains(char c) const;

    [[nodiscard]] static String join(const String &separator, const Util::Array<String> &elements);

    [[nodiscard]] static String format(const char *format, ...);

    [[nodiscard]] static String vformat(const char *format, va_list args);

    [[nodiscard]] static bool isAlpha(char c);

    [[nodiscard]] static bool isNumeric(char c);

    [[nodiscard]] static int32_t parseInt(const char *string);

    [[nodiscard]] static int32_t parseInt(const String &string);

    [[nodiscard]] static int32_t parseHexInt(const char *string);

    [[nodiscard]] static int32_t parseHexInt(const String &string);

    [[nodiscard]] String toUpperCase() const;

    [[nodiscard]] String toLowerCase() const;

    bool operator==(const String &other) const;

    bool operator!=(const String &other) const;

    String &operator=(const String &other);

    String &operator+=(const String &other);

    friend String operator+(const String &first, const String &second);

    friend String operator+(const String &first, char second);

    friend String operator+(const String &first, const char *second);

    friend String operator+(char first, const String &string);

    friend String operator+(const char *first, const String &second);

    char operator[](uint32_t index) const;

    char &operator[](uint32_t index);

    explicit operator char *() const;

    explicit operator const char *() const;

    explicit operator uint8_t *() const;

    explicit operator const uint8_t *() const;

    explicit operator uint32_t() const;

private:

    char *buffer;
    uint32_t len;

    static const constexpr uint8_t CASE_OFFSET = 32;
};

}

#endif
