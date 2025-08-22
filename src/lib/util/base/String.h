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

#ifndef HHUOS_LIB_UTIL_STRING_H
#define HHUOS_LIB_UTIL_STRING_H

#include <stddef.h>
#include <stdarg.h>

#include "base/Address.h"
#include "base/CharacterTypes.h"
#include "collection/Array.h"
#include "io/stream/OutputStream.h"
#include "time/Date.h"

namespace Util {

/// This class wraps a string on the heap and provides methods to manipulate it.
/// For example, string concatenation, substring extraction, and string splitting.
/// If required, the heap memory is automatically reallocated.
///
/// ## Example
/// ```
/// // Create a string and print it.
/// const auto string = Util::String("Hello, World!");
/// Util::System::out << string << Util::Io::PrintStream::endl << Util::Io::PrintStream::flush;
/// ```
class String {

public:
    /// Create a new empty string, consisting only of the null terminator.
    String();

    /// Create a new string containing only the given character.
    String(char c);

    /// Create a new string from the given C-style string.
    /// The given string is copied to the heap.
    String(const char *string);

    /// Create a new string from the given byte array.
    /// The given byte array is copied to the heap.
    String(const uint8_t *data, size_t length);

    /// Create a new string from an existing string.
    String(const String &other);

    /// Delete the string and free the heap memory.
    ~String();

    /// Assign the given string to this string, overwriting the existing string.
    ///
    /// ### Example
    /// ```c++
    /// const auto string1 = Util::String("Hello");
    /// const auto string2 = Util::String("World");
    ///
    /// string1 = string2; // string1 = "World"
    /// ```
    String& operator=(const String &other);

    /// Append the given string to this string.
    ///
    /// ### Example
    /// ```c++
    /// const auto string1 = Util::String("Hello");
    /// const auto string2 = Util::String("World");
    ///
    /// const auto string3 = string1 + string2; // string3 = "HelloWorld"
    /// ```
    String& operator+=(const String &other);

    /// Compare the string with another string for equality.
    ///
    /// ### Example
    /// ```c++
    /// const auto string1 = Util::String("Hello");
    /// const auto string2 = Util::String("Hello");
    /// const auto string3 = Util::String("World");
    ///
    /// const auto equal1 = string1 == string2; // equal1 = true
    /// const auto equal2 = string1 == string3; // equal2 = false
    /// ```
    bool operator==(const String &other) const;

    /// Compare the string with another string for inequality.
    ///
    /// ### Example
    /// ```c++
    /// const auto string1 = Util::String("Hello");
    /// const auto string2 = Util::String("Hello");
    /// const auto string3 = Util::String("World");
    ///
    /// const auto equal1 = string1 != string2; // equal1 = false
    /// const auto equal2 = string1 != string3; // equal2 = true
    /// ```
    bool operator!=(const String &other) const;

    /// Create a new string by concatenating two strings.
    ///
    /// ### Example
    /// ```c++
    /// const auto string1 = Util::String("Hello");
    /// const auto string2 = Util::String("World");
    ///
    /// const auto string3 = string1 + string2; // string3 = "HelloWorld"
    /// ```
    friend String operator+(const String &first, const String &second);

    /// Create a new string by concatenating a string with a character.
    ///
    /// ### Example
    /// ```c++
    /// const auto string1 = Util::String("Hello");
    /// const auto character = '!';
    ///
    /// const auto string2 = string1 + character; // string2 = "Hello!"
    /// ```
    friend String operator+(const String &first, char second);

    /// Create a new string by concatenating a string with a C-style string.
    ///
    /// ### Example
    /// ```c++
    /// const auto string1 = Util::String("Hello");
    /// const char *string2 = "World";
    ///
    /// const auto string3 = string1 + string2; // string3 = "HelloWorld"
    /// ```
    friend String operator+(const String &first, const char *second);

    /// Create a new string by concatenating a character with a string.
    ///
    /// ### Example
    /// ```c++
    /// const auto character = 'H';
    /// const auto string1 = Util::String("ello");
    ///
    /// const auto string2 = character + string1; // string2 = "Hello"
    /// ```
    friend String operator+(char first, const String &string);

    /// Create a new string by concatenating a C-style string with a string.
    ///
    /// ### Example
    /// ```c++
    /// const char *string1 = "Hello";
    /// const auto string2 = Util::String("World");
    ///
    /// const auto string3 = string1 + string2; // string3 = "HelloWorld"
    /// ```
    friend String operator+(const char *first, const String &second);

    /// Get the character at the given index.
    ///
    /// ### Example
    /// ```c++
    /// const auto string = Util::String("Hello, World!");
    /// const auto character = string[0]; // character = 'H'
    /// ```
    char operator[](size_t index) const;

    /// Get a reference to the character at the given index.
    /// This allows modifying the character at the given index.
    ///
    /// ### Example
    /// ```c++
    /// auto string = Util::String("Hello, World!");
    /// string[1] = 'A'; // string = "HAllo, World!"
    /// ```
    char& operator[](size_t index);

    /// Convert the string to a C-style string.
    /// This is done by returning a pointer to the internal buffer.
    ///
    /// ### Example
    /// ```c++
    /// const auto string = Util::String("Hello, World!");
    /// printf("String: '%s'", static_cast<const char*>(string)); // prints "String: 'Hello, World!'"
    /// ```
    explicit operator const char*() const;

    /// Convert the string to a byte array.
    /// This is done by returning a pointer to the internal buffer.
    explicit operator const uint8_t*() const;

    /// Get the hash code of the string (see `hashCode()`).
    explicit operator size_t() const;

    /// Get the length of the string (excluding the null terminator).
    ///
    /// ### Example
    /// ```c++
    /// const auto string = Util::String("Hello, World!");
    /// const auto length = string.length(); // length = 13
    /// ```
    [[nodiscard]] size_t length() const;

    /// Check if the string is empty (length == 0).
    ///
    /// ### Example
    /// ```c++
    /// const auto string1 = Util::String("Hello, World");
    /// const auto string2 = Util::String("");
    ///
    /// const auto empty1 = string1.isEmpty(); // empty1 = false
    /// const auto empty2 = string2.isEmpty(); // empty2 = true
    /// ```
    [[nodiscard]] bool isEmpty() const;

    /// Calculate a simple hash sum of the string.
    /// This is used to implement the `size_t()` operator, which is for example used by `Util::HashMap`.
    [[nodiscard]] size_t hashCode() const;

    /// Get the index of the first occurrence of the given character in the string.
    ///
    /// ### Example
    /// ```c++
    /// const auto string = Util::String("Hello, World!");
    /// const auto index = string.indexOf('o'); // index = 4
    /// ```
    [[nodiscard]] size_t indexOf(char character, size_t start = 0) const;

    /// Get the index of the first occurrence of the given other string in the string.
    ///
    /// ### Example
    /// ```c++
    /// const auto string = Util::String("Hello, World!");
    /// const auto index = string.indexOf("World"); // index = 7
    /// ```
    [[nodiscard]] size_t indexOf(const String &other, size_t start = 0) const;

    /// Get a substring of the string, beginning at the given index and ending at the end of the string.
    /// The character at the given index is included in the substring.
    /// If the given index is greater than the length of the string, an empty string is returned.
    ///
    /// ### Example
    /// ```c++
    /// const auto string = Util::String("Hello, World!");
    /// const auto substring = string.substring(7); // substring = "World!"
    /// ```
    [[nodiscard]] String substring(size_t begin) const;


    /// Get a substring of the string, beginning and ending at the given indices.
    /// The substring is inclusive at the beginning and exclusive at the end.
    /// If the `begin` index is greater than the end index, an empty string is returned.
    /// If the `begin` index is greater than the length of the string, an empty string is returned.
    /// If the `end` index is greater than the length of the string, it is set to the length of the string.
    ///
    /// ### Example
    /// ```c++
    /// const auto string = Util::String("Hello, World!");
    /// const auto substring1 = string.substring(7, 12); // substring1 = "World"
    /// const auto substring2 = string.substring(7, 15); // substring2 = "World"
    /// const auto substring3 = string.substring(2, 9); // substring3 = "llo, Wo"
    /// ```
    [[nodiscard]] String substring(size_t begin, size_t end) const;

    /// Create a copy of the string with leading and trailing whitespaces ('\\t', '\\n', ' ') removed.
    ///
    /// ### Example
    /// ```c++
    /// const auto string = Util::String("   Hello, World!   ");
    /// const auto stripped = string.strip(); // stripped = "Hello, World!"
    /// ```
    [[nodiscard]] String strip() const;

    /// Split the string into an array of strings using the given delimiter.
    /// The delimiter itself is not included in the resulting strings.
    /// If the delimiter is not found, the array only contains the original string.
    /// If the string is empty, an empty array is returned.
    ///
    /// ### Example
    /// ```c++
    /// const auto string = Util::String("abc,def,ghi");
    /// const auto tokens = string.split(","); // tokens = { "abc", "def", "ghi" }
    /// ```
    [[nodiscard]] Array<String> split(const String &delimiter) const;

    /// Create a copy of the string with the first occurrence of the given string removed.
    /// If the given string is not found, a copy the original string is returned.
    ///
    /// ### Example
    /// ```c++
    /// const auto string = Util::String("Hello, World!");
    /// const auto removed = string.remove(", World"); // removed = "Hello!"
    /// ```
    [[nodiscard]] String remove(const String &string) const;

    /// Create a copy of the string with all occurrences of the given string removed.
    /// This is effectively the same as calling `split()` with the given string as the delimiter
    /// and then concatenating the resulting strings.
    /// If the given string is not found, a copy of the original string is returned.
    ///
    /// ### Example
    /// ```c++
    /// const auto string = Util::String("TestHello, TestWorldTest!");
    /// const auto removed = string.removeAll("Test"); // removed = "Hello, World!"
    /// ```
    [[nodiscard]] String removeAll(const String &string) const;

    /// Check if the string begins with the given string.
    ///
    /// ### Example
    /// ```c++
    /// const auto string = Util::String("Hello, World!");
    /// const auto beginsWith1 = string.beginsWith("Hello"); // beginsWith1 = true
    /// const auto beginsWith2 = string.beginsWith("hello"); // beginsWith2 = false
    /// ```
    [[nodiscard]] bool beginsWith(const String &string) const;

    /// Check if the string ends with the given string.
    ///
    /// ### Example
    /// ```c++
    /// const auto string = Util::String("Hello, World!");
    /// const auto endsWith1 = string.endsWith("World"); // endsWith1 = true
    /// const auto endsWith2 = string.endsWith("world"); // endsWith2 = false
    /// ```
    [[nodiscard]] bool endsWith(const String &string) const;

    /// Check if the string contains the given character.
    ///
    /// ### Example
    /// ```c++
    /// const auto string = Util::String("Hello, World!");
    /// const auto contains1 = string.contains('o'); // contains1 = true
    /// const auto contains2 = string.contains('x'); // contains2 = false
    /// ```
    [[nodiscard]] bool contains(char c) const;

    /// Create a copy of the string with all characters converted to uppercase.
    ///
    /// ### Example
    /// ```c++
    /// const auto string = Util::String("Hello, World!");
    /// const auto upper = string.toUpperCase(); // upper = "HELLO, WORLD!"
    /// ```
    [[nodiscard]] String toUpperCase() const;

    /// Create a copy of the string with all characters converted to lowercase.
    ///
    /// ### Example
    /// ```c++
    /// const auto string = Util::String("Hello, World!");
    /// const auto lower = string.toLowerCase(); // lower = "hello, world!"
    /// ```
    [[nodiscard]] String toLowerCase() const;

    /// Concatenate a given array of strings with the given separator between them.
    /// The separator is not included at the beginning or end of the resulting string.
    /// The separator may be empty, in which case the strings are concatenated without any separator.
    ///
    /// ### Example
    /// ```c++
    /// const auto strings = Util::Array<String>({ "Hello", "World" });
    /// const auto joined = Util::String::join(", ", strings); // joined = "Hello, World"
    /// ```
    ///
    /// ### Example
    /// ```c++
    /// const auto strings = Util::Array<String>({ "Hello, ", "World", "!" });
    /// const auto joined = Util::String::join("", strings); // joined = "Hello, World!"
    /// ```
    [[nodiscard]] static String join(const String &separator, const Array<String> &elements);

    /// Format a string using the given format string and arguments.
    /// Supports all format specifiers defined by `printf()` in the C89 standard plus '%B' for boolean values.
    ///
    /// ### Example
    /// ```c++
    /// // string = "Hello, World! You are 42 years old."
    /// const auto string = Util::String::format("Hello, %s! You are %u years old.", "World", 42);
    /// ```
    [[nodiscard]] static String format(const char *format, ...);

    /// Format a string using the given format string and arguments.
    /// Supports all format specifiers defined by `printf()` in the C89 standard plus '%B' for boolean values.
    /// The arguments are passed as a `va_list`, which is useful if the calling function itself is variadic
    /// and wants to pass the arguments to this function. For example, `printf()` is implemented using this function.
    [[nodiscard]] static String format(const char *format, va_list args);

    /// Format a string using the given format string and arguments.
    /// Supports all format specifiers defined by `printf()` in the C89 standard plus '%B' for boolean values.
    /// Instead of returning a new string, the formatted output is written to the given `OutputStream`.
    /// The return value is the number of bytes written to the `OutputStream` or -1 if an error occurred.
    /// In case of an error, data may or may not have been written to the `OutputStream` (undefined behavior).
    static int32_t format(const char *format, va_list args, Io::OutputStream &target);

    /// Format a date using the given format string.
    /// Supports all format specifiers defined by `strftime()` in the C89 standard.
    /// The default format is "%c", which is the same as `asctime()`.
    ///
    /// ### Example
    /// `c++
    /// const auto date = Util::Time::Date(2025, 08, 25, 10, 47, 21);
    ///
    /// const auto str1 = Util::String::formatDate(date); // str1 = "Mon Aug 25 10:47:21 2025"
    /// const auto str2 = Util::String::formatDate(date, "%Y-%m-%d %H:%M:%S"); // str2 = "2025-08-25 10:47:21"
    /// const auto str3 = Util::String::formatDate(date, "%B %d, %Y"); // str3 = "August 25, 2025"
    /// ```
    [[nodiscard]] static String formatDate(const Time::Date &date, const char *format = "%c");

    /// Format a date using the given format string.
    /// Supports all format specifiers defined by `strftime()` in the C89 standard.
    /// Instead of returning a new string, the formatted output is written to the given `OutputStream`.
    /// The return value is the number of bytes written to the `OutputStream` or -1 if an error occurred.
    /// In case of an error, data may or may not have been written to the `OutputStream` (undefined behavior).
    static int32_t formatDate(const Time::Date &date, Io::OutputStream &target, const char *format = "%c");

    /// Parse a signed number from the given string.
    /// If the string is not a valid number, a `Panic` is fired.
    ///
    /// ### Example
    /// ```c++
    /// const auto number1 = Util::String::parseNumber<int32_t>("42"); // number1 = 42
    /// const auto number2 = Util::String::parseNumber<int32_t>("-42"); // number2 = -42
    /// const auto number3 = Util::String::parseNumber<int32_t>("Hello, World!"); // Panic fired
    /// ```
    template<typename T>
    [[nodiscard]] static T parseNumber(const char *string);

    /// Parse a signed number from the given string.
    /// If the string is not a valid number, a `Panic` is fired.
    ///
    /// ### Example
    /// ```c++
    /// const auto string1 = Util::String("42");
    /// const auto string2 = Util::String("-42");
    /// const auto string3 = Util::String("Hello, World!");
    ///
    /// const auto number1 = Util::String::parseNumber<int32_t>(string1); // number1 = 42
    /// const auto number2 = Util::String::parseNumber<int32_t>(string2); // number2 = -42
    /// const auto number3 = Util::String::parseNumber<int32_t>(string3); // Panic fired
    /// ```
    template<typename T>
    [[nodiscard]] static T parseNumber(const String &string);

    /// Parse an unsigned hexadecimal number from the given string.
    /// If the string is not a valid hexadecimal number, a `Panic` is fired.
    ///
    /// ### Example
    /// ```c++
    /// const auto number1 = Util::String::parseHexNumber<uint32_t>("0x42"); // number1 = 66
    /// const auto number2 = Util::String::parseHexNumber<uint32_t>("Hello, World"); // Panic fired
    /// ```
    template<typename T>
    [[nodiscard]] static T parseHexNumber(const char *string);

    /// Parse an unsigned hexadecimal number from the given string.
    /// If the string is not a valid hexadecimal number, a `Panic` is fired.
    ///
    /// ### Example
    /// ```c++
    /// const auto string1 = Util::String("0x42");
    /// const auto string2 = Util::String("Hello, World");
    ///
    /// const auto number1 = Util::String::parseHexNumber<uint32_t>(string1); // number1 = 66
    /// const auto number2 = Util::String::parseHexNumber<uint32_t>(string2); // Panic fired
    /// ```
    template<typename T>
    [[nodiscard]] static T parseHexNumber(const String &string);

    /// Parse a signed floating point number from the given string.
    /// If the string is not a valid floating point number, a `Panic` is fired.
    ///
    /// ### Example
    /// ```c++
    /// const auto number1 = Util::String::parseFloat<double>("42.0"); // number1 = 42.0
    /// const auto number2 = Util::String::parseFloat<double>("-42.0"); // number2 = -42.0
    /// const auto number3 = Util::String::parseFloat<double>("Hello, World!"); // Panic fired
    /// ```
    template<typename T>
    [[nodiscard]] static T parseFloat(const char *string);

    /// Parse a signed floating point number from the given string.
    /// If the string is not a valid floating point number, a `Panic` is fired.
    ///
    /// ### Example
    /// ```c++
    /// const auto string1 = Util::String("42.0");
    /// const auto string2 = Util::String("-42.0");
    /// const auto string3 = Util::String("Hello, World!");
    ///
    /// const auto number1 = Util::String::parseFloat<double>(string1); // number1 = 42.0
    /// const auto number2 = Util::String::parseFloat<double>(string2); // number2 = -42.0
    /// const auto number3 = Util::String::parseFloat<double>(string3); // Panic fired
    /// ```
    template<typename T>
    [[nodiscard]] static T parseFloat(const String &string);

private:

    template<typename T>
    [[nodiscard]] static T parseNumber(const char *string, size_t length);

    template<typename T>
    [[nodiscard]] static T parseHexNumber(const char *string, size_t length);

    char *buffer;
    size_t len;

    static constexpr const char *WEEKDAY_NAMES[7] = {
        "Monday", "Tuesday", "Wednesday", "Thursday", "Friday", "Saturday", "Sunday"
    };

    static constexpr const char *WEEKDAY_ABBREVIATIONS[7] = {
        "Mon", "Tue", "Wed", "Thu", "Fri", "Sat", "Sun"
    };

    static constexpr const char *MONTH_NAMES[13] = {
        "", "January", "February", "March", "April", "May", "June", "July",
        "August", "September", "October", "November", "December"
    };

    static constexpr const char *MONTH_ABBREVIATIONS[13] = {
        "", "Jan", "Feb", "Mar", "Apr", "May", "Jun", "Jul", "Aug", "Sep", "Oct", "Nov", "Dec"
    };

    static constexpr uint8_t CASE_OFFSET = 32;
};

template<typename T>
T String::parseNumber(const char *string, const size_t length) {
    if (length == 0) {
        return 0;
    }

    const size_t limit = string[0] == '-' ? 1 : 0;
    const auto sign = string[0] == '-' ? -1 : 1;

    T result = 0;
    T decimalPower = 1;

    for (size_t i = length - 1; i > limit; i--) {
        if (!CharacterTypes::isDigit(string[i])) {
            Panic::fire(Panic::INVALID_ARGUMENT, "String: Invalid number format!");
        }

        result += (string[i] - '0') * decimalPower;
        decimalPower *= 10;
    }

    if (!CharacterTypes::isDigit(string[limit])) {
        Panic::fire(Panic::INVALID_ARGUMENT, "String: Invalid number format!");
    }
    result += (string[limit] - '0') * decimalPower;

    return result * sign;
}

template<typename T>
T String::parseNumber(const char *string) {
    size_t len = 0;
    while (CharacterTypes::isDigit(string[len]) && string[len] != '\0') {
        len++;
    }

    return parseNumber<T>(string, len);
}

template<typename T>
T String::parseNumber(const String &string) {
    return parseNumber<T>(static_cast<const char*>(string));
}

template<typename T>
T String::parseHexNumber(const char *string, const size_t length) {
    long result = 0;
    long hexPower = 1;

    for (long i = static_cast<long>(length - 1); i >= 0; i--) {
        if (CharacterTypes::isDigit(string[i])) {
            result += (string[i] - '0') * hexPower;
        } else if (string[i] >= 'A' && string[i] <= 'F') {
            result += (10 + string[i] - 'A') * hexPower;
        } else if (string[i] >= 'a' && string[i] <= 'f') {
            result += (10 + string[i] - 'a') * hexPower;
        } else {
            Panic::fire(Panic::INVALID_ARGUMENT, "String: Invalid hex number format!");
        }

        hexPower *= 16;
    }

    return result;
}

template<typename T>
T String::parseHexNumber(const char *string) {
    return parseHexNumber<T>(string, Address(string).stringLength());
}

template<typename T>
T String::parseHexNumber(const String &string) {
    return parseHexNumber<T>(static_cast<const char*>(string), string.len);
}

template<typename T>
T String::parseFloat(const char *string) {
    return parseFloat<T>(String(string));
}

template<typename T>
T String::parseFloat(const String &string) {
    auto parts = string.split(".");
    int64_t firstNumber, secondNumber;

    if (string.beginsWith(".")) {
        firstNumber = 0;
        secondNumber = parseNumber<int64_t>(parts[0]);
    } else {
        firstNumber = parseNumber<int64_t>(parts[0]);
        secondNumber = parts.length() > 1 ? parseNumber<int64_t>(parts[1]) : 0;
    }

    T exp = 1;
    if (parts.length() > 1) {
        for (size_t i = 0; i < parts[1].length(); i++) {
            exp *= 10;
        }
    }

    if (string[0] == '-') {
        return firstNumber - secondNumber / exp;
    }

    return firstNumber + secondNumber / exp;
}

}

#endif
