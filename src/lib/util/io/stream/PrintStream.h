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

#ifndef HHUOS_LIB_UTIL_IO_PRINTSTREAM_H
#define HHUOS_LIB_UTIL_IO_PRINTSTREAM_H

#include <stddef.h>
#include <stdint.h>

#include "base/String.h"
#include "io/stream/FilterOutputStream.h"

namespace Util::Io {

/// An output stream that provides convenient methods for printing various data types as text.
/// It supports strings, characters, integers (in various bases), booleans, pointers, and floating-point numbers.
/// Printing can be done by calling print/println methods or using the stream insertion operator (<<).
/// The stream can be configured with various formatting options, such as base (binary, octal, decimal, hexadecimal),
/// number padding, justification, sign characters, precision for floating-point numbers, and prefixes for integers.
/// It can also be set to flush automatically on new lines.
/// The standard output stream (Util::System::out) is an instance of PrintStream,
/// which allows convenient formatted printing to the console.
class PrintStream final : public FilterOutputStream {

public:
	/// Create a print stream instance that writes to the given output stream.
    explicit PrintStream(OutputStream &stream, bool flushOnNewLine = false);

	/// Write a single byte directly to the underlying output stream with no formatting applied.
    bool write(uint8_t byte) override;

	/// Write a buffer of bytes directly to the underlying output stream with no formatting applied.
    size_t write(const uint8_t *sourceBuffer, size_t offset, size_t length) override;

	/// Flush the underlying output stream.
    size_t flush() override;

	/// Get the total number of bytes written to the underlying output stream so far.
	[[nodiscard]] size_t getBytesWritten() const;

	/// Set the numeric base for integer printing (e.g. 2 for binary, 8 for octal, 10 for decimal, 16 for hexadecimal).
	/// The default base is 10 (decimal).
	///
	/// ### Example
	/// ```c++
	/// Util::System::out.println(255); // prints "255" in decimal
	///
	/// Util::System::out.setBase(16);
	/// Util::System::out.println(255); // prints "FF" in hexadecimal
	///
	/// Util::System::out.setBase(2);
	/// Util::System::out.println(255); // prints "11111111" in binary
	///
	/// Util::System::out.flush();
	/// ```
    void setBase(uint8_t newBase);

	/// Set the minimum number of characters to use for printing numbers.
	/// If the number has fewer digits, it will be padded with spaces.
	/// The default value is 0 (no padding).
	///
	/// ### Example
	/// ```c++
	/// Util::System::out.setNumberPadding(5);
	/// Util::System::out.println(42); // prints "   42" (5 characters wide, right-justified with spaces)
	///
	/// Util::System::out.flush();
	/// ```
    void setNumberPadding(int32_t padding);

	/// Set whether numbers should be left-justified (true) or right-justified (false) within the padding.
	/// The default is right-justified (false).
	///
	/// ### Example
	/// ```c++
	/// Util::System::out.setNumberPadding(5);
	/// Util::System::out.println(42); // prints "   42" (5 characters wide, right-justified with spaces)
	///
	/// Util::System::out.setNumberJustification(true);
	/// Util::System::out.println(42); // prints "42   " (5 characters wide, left-justified with spaces)
	///
	/// Util::System::out.flush();
	/// ```
	void setNumberJustification(bool leftJustified);

	/// Set the sign character to use for positive numbers.
	/// The default is no sign character ('\0').
	///
	/// ### Example
	/// ```c++
	/// Util::System::out.println(42); // prints "42"
	///
	/// Util::System::out.setPositiveSign('+');
	/// Util::System::out.println(42); // prints "+42"
	///
	/// Util::System::out.setPositiveSign('@');
	/// Util::System::out.println(42); // prints "@42"
	///
	/// Util::System::out.setPositiveSign('\0');
	/// Util::System::out.println(42); // prints "42"
	///
	/// Util::System::out.flush();
	/// ```
	void setPositiveSign(char sign);

	/// Set the sign character to use for negative numbers.
	/// The default is '-' (minus sign).
	///
	/// ### Example
	/// ```c++
	/// Util::System::out.println(-42); // prints "-42"
	///
	/// Util::System::out.setNegativeSign('~');
	/// Util::System::out.println(-42); // prints "~42"
	///
	/// Util::System::out.setNegativeSign('!');
	/// Util::System::out.println(-42); // prints "!42"
	///
	/// Util::System::out.setNegativeSign('\0');
	/// Util::System::out.println(-42); // prints "42"
	///
	/// Util::System::out.setNegativeSign('-');
	/// Util::System::out.println(-42); // prints "-42"
	///
	/// Util::System::out.flush();
	/// ```
	void setNegativeSign(char sign);

	/// Set the minimum number of digits to print for integer numbers.
	/// If the number to print has fewer digits, it will be padded with leading zeros.
	///
	/// ### Example
	/// ```c++
	/// Util::System::out.println(42); // prints "42"
	///
	/// Util::System::out.setIntegerPrecision(5);
	/// Util::System::out.println(42); // prints "00042"
	///
	/// Util::System::out.setIntegerPrecision(3);
	/// Util::System::out.println(42); // prints "042"
	///
	/// Util:System::out.println(12345); // prints "12345" (no truncation)
	///
	/// Util::System::out.flush();
	/// ```
	void setIntegerPrecision(int32_t precision);

	/// Set the number of digits to print after the decimal point for floating-point numbers.
	/// If the number has more decimal places, it will be rounded.
	/// The default value is -1, which means to print all available decimal places.
	///
	/// ### Example
	/// ```c++
	/// Util::System::out.println(3.141592653589793); // prints "3.141592653589793"
	///
	/// Util::System::out.setDecimalPrecision(2);
	/// Util::System::out.println(3.141592653589793); // prints "3.14"
	///
	/// Util::System::out.setDecimalPrecision(5);
	/// Util::System::out.println(3.141592653589793); // prints "3.14159"
	///
	/// Util::System::out.setDecimalPrecision(0);
	/// Util::System::out.println(3.141592653589793); // prints "3"
	///
	/// Util::System::out.flush();
	/// ```
	void setDecimalPrecision(int32_t precision);

	/// Set a prefix string to print before integer numbers (e.g. "0x" for hexadecimal).
	/// The default is an empty string (no prefix).
	///
	/// ### Example
	/// ```c++
	/// Util::System::out.println(255); // prints "255"
	/// Util::System::out.setBase(16);
	/// Util::System::out.println(255); // prints "FF"
	///
	/// Util::System::out.setIntegerPrefix("0x");
	///
	/// Util::System::out.setBase(10);
	/// Util::System::out.println(255); // prints "0x255"
	///
	/// Util::System::out.setBase(16);
	/// Util::System::out.println(255); // prints "0xFF"
	///
	/// Util::System::out.flush();
	/// ```
	void setIntegerPrefix(const String &prefix);

	/// Set the base character to use for digits above 9 when printing in bases greater than 10 (e.g. hexadecimal).
	/// This method accepts any character, although the only sensible choices are 'A' and 'a'.
	/// The default is 'A' (uppercase).
	///
	/// ### Example
	/// ```c++
	/// Util::System::out.setBase(16);
	/// Util::System::out.setIntegerPrefix("0x");
	///
	/// Util::System::out.println(255); // prints "0xFF"
	///
	/// Util::System::out.setAlphaNumericBase('a');
	/// Util::System::out.println(255); // prints "0xff"
	///
	/// Util::System::out.setAlphaNumericBase('Q');
	/// Util::System::out.println(255); // prints "0xQQ"
	///
	/// Util::System::out.flush();
	/// ```
	void setAlphaNumericBase(char hexBase);

	/// Set whether to always print a decimal point for floating-point numbers,
	/// even if there are no decimal digits to print (e.g. "3." instead of "3").
	/// The default is false (do not always print the decimal point).
	///
	/// ### Example
	/// ```c++
	/// Util::System::out.println(3.0); // prints "3"
	///
	/// Util::System::out.setAlwaysPrintDecimalPoint(true);
	/// Util::System::out.println(3.0); // prints "3."
	///
	/// Util::System::out.flush();
	/// ```
	void setAlwaysPrintDecimalPoint(bool value);

	/// Print a single character.
	///
	/// ### Example
	/// ```c++
	/// Util::System::out.print('A'); // prints "A"
	/// Util::System::out.print(' '); // prints " "
	/// Util::System::out.println('a'); // prints "a"
	///
	/// Util::System::out.flush();
	/// ```
	void print(char c);

	/// Print a null-terminated C string.
	/// If `maxBytes` is specified and non-negative, at most `maxBytes` characters will be printed.
	/// If the string is shorter than `maxBytes`, printing stops at the null terminator.
	///
	/// ### Example
	/// ```c++
	/// Util::System::out.print("Hello, World!"); // prints "Hello, World!"
	/// Util::System::out.print("Hello, World!", 5); // prints "Hello"
	/// Util::System::out.print("Hello, World!", 20); // prints "Hello, World!"
	///
	/// Util::System::out.flush();
	/// ```
	void print(const char *string, int32_t maxBytes = -1);

	/// Print a string.
	///
	/// ### Example
	/// ```c++
	/// const auto string = Util::String("Hello, World!");
	/// Util::System::out.print(string); // prints "Hello, World!"
	///
	/// Util::System::out.flush();
	/// ```
    void print(const String &string);

	/// Print a boolean value as "true" or "false".
	///
	/// ### Example
	/// ```c++
	/// Util::System::out.print(true); // prints "true"
	/// Util::System::out.println(); // new line
	///
	/// Util::System::out.print(false); // prints "false"
	/// Util::System::out.println(); // new line
	///
	///	Util::System::out.print(2 > 1); // prints "true"
	/// Util::System::out.println(); // new line
	///
	///	Util::System::out.print(1 == 2); // prints "false"
	/// Util::System::out.println(); // new line
	///
	/// Util::System::out.flush();
	/// ```
    void print(bool boolean);

	/// Print a signed 8-bit integer.
	/// This method is overloaded to support signed and unsigned 8-bit, 16-bit, 32-bit, and 64-bit integers.
	/// Formatting is affected by the current base, number padding, justification,
	/// sign characters, integer precision, and prefix settings.
	///
	/// ### Example
	/// ```c++
	/// Util::System::out.print(42); // prints "42"
	/// Util::System::out.println(); // new line
	///
	/// Util::System::out.print(-42); // prints "-42"
	/// Util::System::out.println(); // new line
	///
	/// Util::System::out.setIntegerPrecision(5);
	/// Util::System::out.print(42); // prints "00042"
	/// Util::System::out.println(); // new line
	///
	/// Util::System::out.setPositiveSign('+');
	/// Util::System::out.print(42); // prints "+00042"
	/// Util::System::out.println(); // new line
	///
	/// Util::System::out.setNumberPadding(8);
	/// Util::System::out.print(42); // prints "  +00042"
	/// Util::System::out.println(); // new line
	///
	/// Util::System::out.setIntegerPrefix("0x");
	/// Util::System::out.setBase(16);
	/// Util::System::out.print(255); // prints "+0x000FF"
	/// Util::System::out.println(); // new line
	///
	/// Util::System::out.flush();
	/// ```
    void print(int8_t number);

	/// Print an unsigned 8-bit integer.
	/// This method is overloaded to support signed and unsigned 8-bit, 16-bit, 32-bit, and 64-bit integers.
	/// Formatting is affected by the current base, number padding, justification,
	/// sign characters, integer precision, and prefix settings.
	///
	/// ### Example
	/// ```c++
	/// Util::System::out.print(42); // prints "42"
	/// Util::System::out.println(); // new line
	///
	/// Util::System::out.setIntegerPrecision(5);
	/// Util::System::out.print(42); // prints "00042"
	/// Util::System::out.println(); // new line
	///
	/// Util::System::out.setPositiveSign('+');
	/// Util::System::out.print(42); // prints "+00042"
	/// Util::System::out.println(); // new line
	///
	/// Util::System::out.setNumberPadding(8);
	/// Util::System::out.print(42); // prints "  +00042"
	/// Util::System::out.println(); // new line
	///
	/// Util::System::out.setIntegerPrefix("0x");
	/// Util::System::out.setBase(16);
	/// Util::System::out.print(255); // prints "+0x000FF"
	/// Util::System::out.println(); // new line
	///
	/// Util::System::out.flush();
	/// ```
    void print(uint8_t number);

	/// Print a signed 16-bit integer.
	/// This method is overloaded to support signed and unsigned 8-bit, 16-bit, 32-bit, and 64-bit integers.
	/// Formatting is affected by the current base, number padding, justification,
	/// sign characters, integer precision, and prefix settings.
	///
	/// ### Example
	/// ```c++
	/// Util::System::out.print(42); // prints "42"
	/// Util::System::out.println(); // new line
	///
	/// Util::System::out.print(-42); // prints "-42"
	/// Util::System::out.println(); // new line
	///
	/// Util::System::out.setIntegerPrecision(5);
	/// Util::System::out.print(42); // prints "00042"
	/// Util::System::out.println(); // new line
	///
	/// Util::System::out.setPositiveSign('+');
	/// Util::System::out.print(42); // prints "+00042"
	/// Util::System::out.println(); // new line
	///
	/// Util::System::out.setNumberPadding(8);
	/// Util::System::out.print(42); // prints "  +00042"
	/// Util::System::out.println(); // new line
	///
	/// Util::System::out.setIntegerPrefix("0x");
	/// Util::System::out.setBase(16);
	/// Util::System::out.print(255); // prints "+0x000FF"
	/// Util::System::out.println(); // new line
	///
	/// Util::System::out.flush();
	/// ```
    void print(int16_t number);

	/// Print an unsigned 16-bit integer.
	/// This method is overloaded to support signed and unsigned 8-bit, 16-bit, 32-bit, and 64-bit integers.
	/// Formatting is affected by the current base, number padding, justification,
	/// sign characters, integer precision, and prefix settings.
	///
	/// ### Example
	/// ```c++
	/// Util::System::out.print(42); // prints "42"
	/// Util::System::out.println(); // new line
	///
	/// Util::System::out.setIntegerPrecision(5);
	/// Util::System::out.print(42); // prints "00042"
	/// Util::System::out.println(); // new line
	///
	/// Util::System::out.setPositiveSign('+');
	/// Util::System::out.print(42); // prints "+00042"
	/// Util::System::out.println(); // new line
	///
	/// Util::System::out.setNumberPadding(8);
	/// Util::System::out.print(42); // prints "  +00042"
	/// Util::System::out.println(); // new line
	///
	/// Util::System::out.setIntegerPrefix("0x");
	/// Util::System::out.setBase(16);
	/// Util::System::out.print(255); // prints "+0x000FF"
	/// Util::System::out.println(); // new line
	///
	/// Util::System::out.flush();
	/// ```
    void print(uint16_t number);

	/// Print a signed 32-bit integer.
	/// This method is overloaded to support signed and unsigned 8-bit, 16-bit, 32-bit, and 64-bit integers.
	/// Formatting is affected by the current base, number padding, justification,
	/// sign characters, integer precision, and prefix settings.
	///
	/// ### Example
	/// ```c++
	/// Util::System::out.print(42); // prints "42"
	/// Util::System::out.println(); // new line
	///
	/// Util::System::out.print(-42); // prints "-42"
	/// Util::System::out.println(); // new line
	///
	/// Util::System::out.setIntegerPrecision(5);
	/// Util::System::out.print(42); // prints "00042"
	/// Util::System::out.println(); // new line
	///
	/// Util::System::out.setPositiveSign('+');
	/// Util::System::out.print(42); // prints "+00042"
	/// Util::System::out.println(); // new line
	///
	/// Util::System::out.setNumberPadding(8);
	/// Util::System::out.print(42); // prints "  +00042"
	/// Util::System::out.println(); // new line
	///
	/// Util::System::out.setIntegerPrefix("0x");
	/// Util::System::out.setBase(16);
	/// Util::System::out.print(255); // prints "+0x000FF"
	/// Util::System::out.println(); // new line
	///
	/// Util::System::out.flush();
	/// ```
    void print(int32_t number);

	/// Print an unsigned 32-bit integer.
	/// This method is overloaded to support signed and unsigned 8-bit, 16-bit, 32-bit, and 64-bit integers.
	/// Formatting is affected by the current base, number padding, justification,
	/// sign characters, integer precision, and prefix settings.
	///
	/// ### Example
	/// ```c++
	/// Util::System::out.print(42); // prints "42"
	/// Util::System::out.println(); // new line
	///
	/// Util::System::out.setIntegerPrecision(5);
	/// Util::System::out.print(42); // prints "00042"
	/// Util::System::out.println(); // new line
	///
	/// Util::System::out.setPositiveSign('+');
	/// Util::System::out.print(42); // prints "+00042"
	/// Util::System::out.println(); // new line
	///
	/// Util::System::out.setNumberPadding(8);
	/// Util::System::out.print(42); // prints "  +00042"
	/// Util::System::out.println(); // new line
	///
	/// Util::System::out.setIntegerPrefix("0x");
	/// Util::System::out.setBase(16);
	/// Util::System::out.print(255); // prints "+0x000FF"
	/// Util::System::out.println(); // new line
	///
	/// Util::System::out.flush();
	/// ```
    void print(uint32_t number);

	/// Print a signed 64-bit integer.
	/// This method is overloaded to support signed and unsigned 8-bit, 16-bit, 32-bit, and 64-bit integers.
	/// Formatting is affected by the current base, number padding, justification,
	/// sign characters, integer precision, and prefix settings.
	///
	/// ### Example
	/// ```c++
	/// Util::System::out.print(42); // prints "42"
	/// Util::System::out.println(); // new line
	///
	/// Util::System::out.print(-42); // prints "-42"
	/// Util::System::out.println(); // new line
	///
	/// Util::System::out.setIntegerPrecision(5);
	/// Util::System::out.print(42); // prints "00042"
	/// Util::System::out.println(); // new line
	///
	/// Util::System::out.setPositiveSign('+');
	/// Util::System::out.print(42); // prints "+00042"
	/// Util::System::out.println(); // new line
	///
	/// Util::System::out.setNumberPadding(8);
	/// Util::System::out.print(42); // prints "  +00042"
	/// Util::System::out.println(); // new line
	///
	/// Util::System::out.setIntegerPrefix("0x");
	/// Util::System::out.setBase(16);
	/// Util::System::out.print(255); // prints "+0x000FF"
	/// Util::System::out.println(); // new line
	///
	/// Util::System::out.flush();
	/// ```
    void print(int64_t number);

	/// Print an unsigned 64-bit integer.
	/// This method is overloaded to support signed and unsigned 8-bit, 16-bit, 32-bit, and 64-bit integers.
	/// Formatting is affected by the current base, number padding, justification,
	/// sign characters, integer precision, and prefix settings.
	///
	/// ### Example
	/// ```c++
	/// Util::System::out.print(42); // prints "42"
	/// Util::System::out.println(); // new line
	///
	/// Util::System::out.setIntegerPrecision(5);
	/// Util::System::out.print(42); // prints "00042"
	/// Util::System::out.println(); // new line
	///
	/// Util::System::out.setPositiveSign('+');
	/// Util::System::out.print(42); // prints "+00042"
	/// Util::System::out.println(); // new line
	///
	/// Util::System::out.setNumberPadding(8);
	/// Util::System::out.print(42); // prints "  +00042"
	/// Util::System::out.println(); // new line
	///
	/// Util::System::out.setIntegerPrefix("0x");
	/// Util::System::out.setBase(16);
	/// Util::System::out.print(255); // prints "+0x000FF"
	/// Util::System::out.println(); // new line
	///
	/// Util::System::out.flush();
	/// ```
    void print(uint64_t number, char sign = '\0');

	/// Print a pointer (memory address) as an unsigned integer.
	/// The address is cast to `uintptr_t` and printed according to the current base and formatting settings.
	///
	/// ### Example
	/// ```c++
	/// int variable = 42;
	///
	/// // Print the address of `variable` as a decimal value
	/// Util::System::out.print(&variable);
	/// Util::System::out.println(); // new line
	///
	/// // Print the address of `variable` as a hexadecimal value, padded to 8 digits with "0x" prefix
	/// Util::System::out.setBase(16);
	/// Util::System::out.setIntegerPrefix("0x");
	/// Util::System::out.setIntegerPrecision(8);
	/// Util::System::out.print(&variable);
	/// Util::System::out.println(); // new line
	///
	/// Util::System::out.flush();
	/// ```
    void print(void *pointer);

	/// Print a floating-point number.
	/// The number of decimal places printed is controlled by the current decimal precision setting.
	/// Rounding is applied if the number has more decimal places than specified.
	///
	/// ### Example
	/// ```c++
	/// Util::System::out.print(3.141592653589793); // prints "3.141592653589793"
	/// Util::System::out.println(); // new line
	///
	/// Util::System::out.setDecimalPrecision(2);
	/// Util::System::out.print(3.141592653589793); // prints "3.14"
	/// Util::System::out.println(); // new line
	///
	/// Util::System::out.setDecimalPrecision(0);
	/// Util::System::out.print(3.141592653589793); // prints "3"
	/// Util::System::out.println(); // new line
	///
	/// Util::System::out.setAlwaysPrintDecimalPoint(true);
	/// Util::System::out.print(3.0); // prints "3."
	/// Util::System::out.println(); // new line
	///
	/// Util::System::out.flush();
	/// ```
	void print(double number);

	/// Print a new line character ('\n').
	/// If the stream is configured to flush on new lines, it will also flush the underlying output stream.
	///
	/// ### Example
	/// ```c++
	/// /// Print "Hello" and "World!" on separate lines
	/// Util::System::out.print("Hello");
	/// Util::System::out.println(); // prints a new line
	/// Util::System::out.print("World!");
	/// Util::System::out.println(); // prints a new line
	///
	/// Util::System::out.flush();
	/// ```
    void println();

	/// Print a single character followed by a new line.
	/// If the stream is configured to flush on new lines, it will also flush the underlying output stream.
	///
	/// ### Example
	/// ```c++
	/// Util::System::out.println('A'); // prints "A\n"
	/// Util::System::out.println('a'); // prints "a\n"
	///
	/// Util::System::out.flush();
	/// ```
	void println(char c);

	/// Print a null-terminated C string followed by a new line.
	/// If the stream is configured to flush on new lines, it will also flush the underlying output stream.
	/// If `maxBytes` is specified and non-negative, at most `maxBytes` characters will be printed.
	/// If the string is shorter than `maxBytes`, printing stops at the null terminator.
	///
	/// ### Example
	/// ```c++
	/// Util::System::out.println("Hello, World!"); // prints "Hello, World!\n"
	/// Util::System::out.print("Hello, World!", 5); // prints "Hello\n"
	/// Util::System::out.print("Hello, World!", 20); // prints "Hello, World!\n"
	///
	/// Util::System::out.flush();
	/// ```
    void println(const char *string, int32_t maxBytes = -1);

	/// Print a string followed by a new line.
	/// If the stream is configured to flush on new lines, it will also flush the underlying output stream.
	///
	///	/// ### Example
	/// ```c++
	/// const auto string = Util::String("Hello, World!");
	/// Util::System::out.println(string); // prints "Hello, World!\n"
	///
	/// Util::System::out.flush();
	/// ```
    void println(const String &string);

	/// Print a boolean value as "true" or "false" followed by a new line.
	/// If the stream is configured to flush on new lines, it will also flush the underlying output stream.
	///
	/// ### Example
	/// ```c++
	/// Util::System::out.println(true); // prints "true\n"
	/// Util::System::out.println(false); // prints "false\n"
	///	Util::System::out.println(2 > 1); // prints "true\n"
	///	Util::System::out.println(1 == 2); // prints "false\n"
	///
	/// Util::System::out.flush();
	/// ```
    void println(bool boolean);

	/// Print a signed 8-bit integer followed by a new line.
	/// If the stream is configured to flush on new lines, it will also flush the underlying output stream.
	/// This method is overloaded to support signed and unsigned 8-bit, 16-bit, 32-bit, and 64-bit integers.
	/// Formatting is affected by the current base, number padding, justification,
	/// sign characters, integer precision, and prefix settings.
	///
	/// ### Example
	/// ```c++
	/// Util::System::out.println(42); // prints "42\n"
	/// Util::System::out.println(-42); // prints "-42\n"
	///
	/// Util::System::out.setIntegerPrecision(5);
	/// Util::System::out.println(42); // prints "00042\n"
	///
	/// Util::System::out.setPositiveSign('+');
	/// Util::System::out.println(42); // prints "+00042\n"
	///
	/// Util::System::out.setNumberPadding(8);
	/// Util::System::out.println(42); // prints "  +00042\n"
	///
	/// Util::System::out.setIntegerPrefix("0x");
	/// Util::System::out.setBase(16);
	/// Util::System::out.println(255); // prints "+0x000FF\n"
	///
	/// Util::System::out.flush();
	/// ```
    void println(int8_t number);

	/// Print an unsigned 8-bit integer followed by a new line.
	/// If the stream is configured to flush on new lines, it will also flush the underlying output stream.
	/// This method is overloaded to support signed and unsigned 8-bit, 16-bit, 32-bit, and 64-bit integers.
	/// Formatting is affected by the current base, number padding, justification,
	/// sign characters, integer precision, and prefix settings.
	///
	/// ### Example
	/// ```c++
	/// Util::System::out.println(42); // prints "42\n"
	///
	/// Util::System::out.setIntegerPrecision(5);
	/// Util::System::out.println(42); // prints "00042\n"
	///
	/// Util::System::out.setPositiveSign('+');
	/// Util::System::out.println(42); // prints "+00042\n"
	///
	/// Util::System::out.setNumberPadding(8);
	/// Util::System::out.println(42); // prints "  +00042\n"
	///
	/// Util::System::out.setIntegerPrefix("0x");
	/// Util::System::out.setBase(16);
	/// Util::System::out.println(255); // prints "+0x000FF\n"
	///
	/// Util::System::out.flush();
	/// ```
    void println(uint8_t number);

	/// Print a signed 16-bit integer followed by a new line.
	/// If the stream is configured to flush on new lines, it will also flush the underlying output stream.
	/// This method is overloaded to support signed and unsigned 8-bit, 16-bit, 32-bit, and 64-bit integers.
	/// Formatting is affected by the current base, number padding, justification,
	/// sign characters, integer precision, and prefix settings.
	///
	/// ### Example
	/// ```c++
	/// Util::System::out.println(42); // prints "42\n"
	/// Util::System::out.println(-42); // prints "-42\n"
	///
	/// Util::System::out.setIntegerPrecision(5);
	/// Util::System::out.println(42); // prints "00042\n"
	///
	/// Util::System::out.setPositiveSign('+');
	/// Util::System::out.println(42); // prints "+00042\n"
	///
	/// Util::System::out.setNumberPadding(8);
	/// Util::System::out.println(42); // prints "  +00042\n"
	///
	/// Util::System::out.setIntegerPrefix("0x");
	/// Util::System::out.setBase(16);
	/// Util::System::out.println(255); // prints "+0x000FF\n"
	///
	/// Util::System::out.flush();
	/// ```
    void println(int16_t number);

	/// Print an unsigned 16-bit integer followed by a new line.
	/// If the stream is configured to flush on new lines, it will also flush the underlying output stream.
	/// This method is overloaded to support signed and unsigned 8-bit, 16-bit, 32-bit, and 64-bit integers.
	/// Formatting is affected by the current base, number padding, justification,
	/// sign characters, integer precision, and prefix settings.
	///
	/// ### Example
	/// ```c++
	/// Util::System::out.println(42); // prints "42\n"
	///
	/// Util::System::out.setIntegerPrecision(5);
	/// Util::System::out.println(42); // prints "00042\n"
	///
	/// Util::System::out.setPositiveSign('+');
	/// Util::System::out.println(42); // prints "+00042\n"
	///
	/// Util::System::out.setNumberPadding(8);
	/// Util::System::out.println(42); // prints "  +00042\n"
	///
	/// Util::System::out.setIntegerPrefix("0x");
	/// Util::System::out.setBase(16);
	/// Util::System::out.println(255); // prints "+0x000FF\n"
	///
	/// Util::System::out.flush();
	/// ```
    void println(uint16_t number);

	/// Print a signed 32-bit integer followed by a new line.
	/// If the stream is configured to flush on new lines, it will also flush the underlying output stream.
	/// This method is overloaded to support signed and unsigned 8-bit, 16-bit, 32-bit, and 64-bit integers.
	/// Formatting is affected by the current base, number padding, justification,
	/// sign characters, integer precision, and prefix settings.
	///
	/// ### Example
	/// ```c++
	/// Util::System::out.println(42); // prints "42\n"
	/// Util::System::out.println(-42); // prints "-42\n"
	///
	/// Util::System::out.setIntegerPrecision(5);
	/// Util::System::out.println(42); // prints "00042\n"
	///
	/// Util::System::out.setPositiveSign('+');
	/// Util::System::out.println(42); // prints "+00042\n"
	///
	/// Util::System::out.setNumberPadding(8);
	/// Util::System::out.println(42); // prints "  +00042\n"
	///
	/// Util::System::out.setIntegerPrefix("0x");
	/// Util::System::out.setBase(16);
	/// Util::System::out.println(255); // prints "+0x000FF\n"
	///
	/// Util::System::out.flush();
	/// ```
    void println(int32_t number);

	/// Print an unsigned 32-bit integer followed by a new line.
	/// If the stream is configured to flush on new lines, it will also flush the underlying output stream.
	/// This method is overloaded to support signed and unsigned 8-bit, 16-bit, 32-bit, and 64-bit integers.
	/// Formatting is affected by the current base, number padding, justification,
	/// sign characters, integer precision, and prefix settings.
	///
	/// ### Example
	/// ```c++
	/// Util::System::out.println(42); // prints "42\n"
	///
	/// Util::System::out.setIntegerPrecision(5);
	/// Util::System::out.println(42); // prints "00042\n"
	///
	/// Util::System::out.setPositiveSign('+');
	/// Util::System::out.println(42); // prints "+00042\n"
	///
	/// Util::System::out.setNumberPadding(8);
	/// Util::System::out.println(42); // prints "  +00042\n"
	///
	/// Util::System::out.setIntegerPrefix("0x");
	/// Util::System::out.setBase(16);
	/// Util::System::out.println(255); // prints "+0x000FF\n"
	///
	/// Util::System::out.flush();
	/// ```
    void println(uint32_t number);

	/// Print a signed 64-bit integer followed by a new line.
	/// If the stream is configured to flush on new lines, it will also flush the underlying output stream.
	/// This method is overloaded to support signed and unsigned 8-bit, 16-bit, 32-bit, and 64-bit integers.
	/// Formatting is affected by the current base, number padding, justification,
	/// sign characters, integer precision, and prefix settings.
	///
	/// ### Example
	/// ```c++
	/// Util::System::out.println(42); // prints "42\n"
	/// Util::System::out.println(-42); // prints "-42\n"
	///
	/// Util::System::out.setIntegerPrecision(5);
	/// Util::System::out.println(42); // prints "00042\n"
	///
	/// Util::System::out.setPositiveSign('+');
	/// Util::System::out.println(42); // prints "+00042\n"
	///
	/// Util::System::out.setNumberPadding(8);
	/// Util::System::out.println(42); // prints "  +00042\n"
	///
	/// Util::System::out.setIntegerPrefix("0x");
	/// Util::System::out.setBase(16);
	/// Util::System::out.println(255); // prints "+0x000FF\n"
	///
	/// Util::System::out.flush();
	/// ```
    void println(int64_t number);

	/// Print an unsigned 64-bit integer followed by a new line.
	/// If the stream is configured to flush on new lines, it will also flush the underlying output stream.
	/// This method is overloaded to support signed and unsigned 8-bit, 16-bit, 32-bit, and 64-bit integers.
	/// Formatting is affected by the current base, number padding, justification,
	/// sign characters, integer precision, and prefix settings.
	///
	/// ### Example
	/// ```c++
	/// Util::System::out.println(42); // prints "42\n"
	///
	/// Util::System::out.setIntegerPrecision(5);
	/// Util::System::out.println(42); // prints "00042\n"
	///
	/// Util::System::out.setPositiveSign('+');
	/// Util::System::out.println(42); // prints "+00042\n"
	///
	/// Util::System::out.setNumberPadding(8);
	/// Util::System::out.println(42); // prints "  +00042\n"
	///
	/// Util::System::out.setIntegerPrefix("0x");
	/// Util::System::out.setBase(16);
	/// Util::System::out.println(255); // prints "+0x000FF\n"
	///
	/// Util::System::out.flush();
	/// ```
    void println(uint64_t number);

	/// Print a pointer (memory address) as an unsigned integer followed by a new line.
	/// If the stream is configured to flush on new lines, it will also flush the underlying output stream.
	/// The address is cast to `uintptr_t` and printed according to the current base and formatting settings.
	///
	/// ### Example
	/// ```c++
	/// int variable = 42;
	///
	/// // Print the address of `variable` as a decimal value followed by a new line
	/// Util::System::out.println(&variable);
	///
	/// // Print the address of `variable` as a hexadecimal value,
	/// // padded to 8 digits with "0x" prefix followed by a new line
	/// Util::System::out.setBase(16);
	/// Util::System::out.setIntegerPrefix("0x");
	/// Util::System::out.setIntegerPrecision(8);
	/// Util::System::out.println(&variable);
	///
	/// Util::System::out.flush();
	/// ```
    void println(void *pointer);

	/// Print a single character via the stream insertion operator.
	///
	/// ### Example
	/// ```c++
	/// Util::System::out
	///		<< 'A'; // prints "A"
	///		<< ' '; // prints " "
	///		<< 'a' // prints "a"
	///		<< Util::Io::PrintStream::ln << Util::Io::PrintStream::flush; // prints a new line and flushes the stream
	/// ```
    PrintStream& operator<<(char c);

	/// Print a null-terminated C string via the stream insertion operator.
	///
	/// ### Example
	/// ```c++
	/// Util::System::out
	///		<< "Hello, World!" // prints "Hello, World!"
	///		<< Util::Io::PrintStream::ln << Util::Io::PrintStream::flush; // prints a new line and flushes the stream
	/// ```
    PrintStream& operator<<(const char *string);

	/// Print a string via the stream insertion operator.
	///
	/// ### Example
	/// ```c++
	/// const auto string = Util::String("Hello, World!");
	///
	/// Util::System::out
	///		<< string // prints "Hello, World!"
	///		<< Util::Io::PrintStream::ln << Util::Io::PrintStream::flush; // prints a new line and flushes the stream
	/// ```
    PrintStream& operator<<(const String &string);

	/// Print a boolean value as "true" or "false" via the stream insertion operator.
	///
	///	### Example
	///	```c++
	///	Util::System::out
	///		<< true << Util::Io::PrintStream::ln // prints "true\n"
	///		<< false << Util::Io::PrintStream::ln // prints "false\n"
	///		<< (2 > 1) << Util::Io::PrintStream::ln // prints "true\n"
	///		<< (1 == 2) << Util::Io::PrintStream::ln // prints "false\n"
	///		<< Util::Io::PrintStream::flush; // flushes the stream
	/// ```
    PrintStream& operator<<(bool boolean);

	/// Print a signed 8-bit integer via the stream insertion operator.
	/// This operator is overloaded to support signed and unsigned 8-bit, 16-bit, 32-bit, and 64-bit integers.
	/// Formatting is affected by the current base, number padding, justification,
	/// sign characters, integer precision, and prefix settings.
	///
	/// ### Example
	/// ```c++
	/// Util::System::out
	///     << 42 << Util::Io::PrintStream::ln // prints "42\n"
	///     << -42 << Util::Io::PrintStream::ln; // prints "-42\n"
	///
	/// Util::System::out.setIntegerPrecision(5);
	/// Util::System::out << 42 << Util::Io::PrintStream::ln; // prints "00042\n"
	///
	/// Util::System::out.setPositiveSign('+');
	/// Util::System::out << 42 << Util::Io::PrintStream::ln; // prints "+00042\n"
	///
	/// Util::System::out.setNumberPadding(8);
	/// Util::System::out << 42 << Util::Io::PrintStream::ln; // prints "  +00042\n"
	///
	/// Util::System::out.setIntegerPrefix("0x");
	/// Util::System::out << Util::Io::PrintStream::hex << 255 // prints "+0x000FF"
	///		<< Util::Io::PrintStream::ln << Util::Io::PrintStream::flush; // prints a new line and flushes the stream
	/// ```
	PrintStream& operator<<(int8_t number);

	/// Print an unsigned 8-bit integer via the stream insertion operator.
	/// This operator is overloaded to support signed and unsigned 8-bit, 16-bit, 32-bit, and 64-bit integers.
	/// Formatting is affected by the current base, number padding, justification,
	/// sign characters, integer precision, and prefix settings.
	///
	/// ### Example
	/// ```c++
	/// Util::System::out << 42 << Util::Io::PrintStream::ln // prints "42\n"
	///
	/// Util::System::out.setIntegerPrecision(5);
	/// Util::System::out << 42 << Util::Io::PrintStream::ln; // prints "00042\n"
	///
	/// Util::System::out.setPositiveSign('+');
	/// Util::System::out << 42 << Util::Io::PrintStream::ln; // prints "+00042\n"
	///
	/// Util::System::out.setNumberPadding(8);
	/// Util::System::out << 42 << Util::Io::PrintStream::ln; // prints "  +00042\n"
	///
	/// Util::System::out.setIntegerPrefix("0x");
	/// Util::System::out << Util::Io::PrintStream::hex << 255 // prints "+0x000FF"
	///		<< Util::Io::PrintStream::ln << Util::Io::PrintStream::flush; // prints a new line and flushes the stream
	/// ```
	PrintStream& operator<<(uint8_t number);

	/// Print a signed 16-bit integer via the stream insertion operator.
	/// This operator is overloaded to support signed and unsigned 8-bit, 16-bit, 32-bit, and 64-bit integers.
	/// Formatting is affected by the current base, number padding, justification,
	/// sign characters, integer precision, and prefix settings.
	///
	/// ### Example
	/// ```c++
	/// Util::System::out
	///     << 42 << Util::Io::PrintStream::ln // prints "42\n"
	///     << -42 << Util::Io::PrintStream::ln; // prints "-42\n"
	///
	/// Util::System::out.setIntegerPrecision(5);
	/// Util::System::out << 42 << Util::Io::PrintStream::ln; // prints "00042\n"
	///
	/// Util::System::out.setPositiveSign('+');
	/// Util::System::out << 42 << Util::Io::PrintStream::ln; // prints "+00042\n"
	///
	/// Util::System::out.setNumberPadding(8);
	/// Util::System::out << 42 << Util::Io::PrintStream::ln; // prints "  +00042\n"
	///
	/// Util::System::out.setIntegerPrefix("0x");
	/// Util::System::out << Util::Io::PrintStream::hex << 255 // prints "+0x000FF"
	///		<< Util::Io::PrintStream::ln << Util::Io::PrintStream::flush; // prints a new line and flushes the stream
	/// ```
    PrintStream& operator<<(int16_t number);

	/// Print an unsigned 16-bit integer via the stream insertion operator.
	/// This operator is overloaded to support signed and unsigned 8-bit, 16-bit, 32-bit, and 64-bit integers.
	/// Formatting is affected by the current base, number padding, justification,
	/// sign characters, integer precision, and prefix settings.
	///
	/// ### Example
	/// ```c++
	/// Util::System::out << 42 << Util::Io::PrintStream::ln // prints "42\n"
	///
	/// Util::System::out.setIntegerPrecision(5);
	/// Util::System::out << 42 << Util::Io::PrintStream::ln; // prints "00042\n"
	///
	/// Util::System::out.setPositiveSign('+');
	/// Util::System::out << 42 << Util::Io::PrintStream::ln; // prints "+00042\n"
	///
	/// Util::System::out.setNumberPadding(8);
	/// Util::System::out << 42 << Util::Io::PrintStream::ln; // prints "  +00042\n"
	///
	/// Util::System::out.setIntegerPrefix("0x");
	/// Util::System::out << Util::Io::PrintStream::hex << 255 // prints "+0x000FF"
	///		<< Util::Io::PrintStream::ln << Util::Io::PrintStream::flush; // prints a new line and flushes the stream
	/// ```
    PrintStream& operator<<(uint16_t number);

	/// Print a signed 32-bit integer via the stream insertion operator.
	/// This operator is overloaded to support signed and unsigned 8-bit, 16-bit, 32-bit, and 64-bit integers.
	/// Formatting is affected by the current base, number padding, justification,
	/// sign characters, integer precision, and prefix settings.
	///
	/// ### Example
	/// ```c++
	/// Util::System::out
	///     << 42 << Util::Io::PrintStream::ln // prints "42\n"
	///     << -42 << Util::Io::PrintStream::ln; // prints "-42\n"
	///
	/// Util::System::out.setIntegerPrecision(5);
	/// Util::System::out << 42 << Util::Io::PrintStream::ln; // prints "00042\n"
	///
	/// Util::System::out.setPositiveSign('+');
	/// Util::System::out << 42 << Util::Io::PrintStream::ln; // prints "+00042\n"
	///
	/// Util::System::out.setNumberPadding(8);
	/// Util::System::out << 42 << Util::Io::PrintStream::ln; // prints "  +00042\n"
	///
	/// Util::System::out.setIntegerPrefix("0x");
	/// Util::System::out << Util::Io::PrintStream::hex << 255 // prints "+0x000FF"
	///		<< Util::Io::PrintStream::ln << Util::Io::PrintStream::flush; // prints a new line and flushes the stream
	/// ```
    PrintStream& operator<<(int32_t number);

	/// Print an unsigned 32-bit integer via the stream insertion operator.
	/// This operator is overloaded to support signed and unsigned 8-bit, 16-bit, 32-bit, and 64-bit integers.
	/// Formatting is affected by the current base, number padding, justification,
	/// sign characters, integer precision, and prefix settings.
	///
	/// ### Example
	/// ```c++
	/// Util::System::out << 42 << Util::Io::PrintStream::ln // prints "42\n"
	///
	/// Util::System::out.setIntegerPrecision(5);
	/// Util::System::out << 42 << Util::Io::PrintStream::ln; // prints "00042\n"
	///
	/// Util::System::out.setPositiveSign('+');
	/// Util::System::out << 42 << Util::Io::PrintStream::ln; // prints "+00042\n"
	///
	/// Util::System::out.setNumberPadding(8);
	/// Util::System::out << 42 << Util::Io::PrintStream::ln; // prints "  +00042\n"
	///
	/// Util::System::out.setIntegerPrefix("0x");
	/// Util::System::out << Util::Io::PrintStream::hex << 255 // prints "+0x000FF"
	///		<< Util::Io::PrintStream::ln << Util::Io::PrintStream::flush; // prints a new line and flushes the stream
	/// ```
    PrintStream& operator<<(uint32_t number);

	/// Print a signed 64-bit integer via the stream insertion operator.
	/// This operator is overloaded to support signed and unsigned 8-bit, 16-bit, 32-bit, and 64-bit integers.
	/// Formatting is affected by the current base, number padding, justification,
	/// sign characters, integer precision, and prefix settings.
	///
	/// ### Example
	/// ```c++
	/// Util::System::out
	///     << 42 << Util::Io::PrintStream::ln // prints "42\n"
	///     << -42 << Util::Io::PrintStream::ln; // prints "-42\n"
	///
	/// Util::System::out.setIntegerPrecision(5);
	/// Util::System::out << 42 << Util::Io::PrintStream::ln; // prints "00042\n"
	///
	/// Util::System::out.setPositiveSign('+');
	/// Util::System::out << 42 << Util::Io::PrintStream::ln; // prints "+00042\n"
	///
	/// Util::System::out.setNumberPadding(8);
	/// Util::System::out << 42 << Util::Io::PrintStream::ln; // prints "  +00042\n"
	///
	/// Util::System::out.setIntegerPrefix("0x");
	/// Util::System::out << Util::Io::PrintStream::hex << 255 // prints "+0x000FF"
	///		<< Util::Io::PrintStream::ln << Util::Io::PrintStream::flush; // prints a new line and flushes the stream
	/// ```
    PrintStream& operator<<(int64_t number);

	/// Print an unsigned 64-bit integer via the stream insertion operator.
	/// This operator is overloaded to support signed and unsigned 8-bit, 16-bit, 32-bit, and 64-bit integers.
	/// Formatting is affected by the current base, number padding, justification,
	/// sign characters, integer precision, and prefix settings.
	///
	/// ### Example
	/// ```c++
	/// Util::System::out << 42 << Util::Io::PrintStream::ln // prints "42\n"
	///
	/// Util::System::out.setIntegerPrecision(5);
	/// Util::System::out << 42 << Util::Io::PrintStream::ln; // prints "00042\n"
	///
	/// Util::System::out.setPositiveSign('+');
	/// Util::System::out << 42 << Util::Io::PrintStream::ln; // prints "+00042\n"
	///
	/// Util::System::out.setNumberPadding(8);
	/// Util::System::out << 42 << Util::Io::PrintStream::ln; // prints "  +00042\n"
	///
	/// Util::System::out.setIntegerPrefix("0x");
	/// Util::System::out << Util::Io::PrintStream::hex << 255 // prints "+0x000FF"
	///		<< Util::Io::PrintStream::ln << Util::Io::PrintStream::flush; // prints a new line and flushes the stream
	/// ```
    PrintStream& operator<<(uint64_t number);

	/// Print a pointer (memory address) as an unsigned integer via the stream insertion operator.
	/// The address is cast to `uintptr_t` and printed according to the current base and formatting settings.
	///
	/// ### Example
	/// ```c++
	/// int variable = 42;
	///
	/// Print the address of `variable` as a decimal value
	/// Util::System::out << &variable
	///		<< Util::Io::PrintStream::ln; // prints a new line
	///
	/// // Print the address of `variable` as a hexadecimal value, padded to 8 digits with "0x" prefix
	///	Util::System::out.setIntegerPrefix("0x");
	/// Util::System::out.setIntegerPrecision(8);
	/// Util::System::out << Util::Io::PrintStream::hex << &variable
	///		<< Util::Io::PrintStream::ln << Util::Io::PrintStream::flush; // prints a new line and flushes the stream
	/// ```
    PrintStream& operator<<(void *pointer);

	/// Print a floating-point number via the stream insertion operator.
	/// The number of decimal places printed is controlled by the current decimal precision setting.
	/// Rounding is applied if the number has more decimal places than specified.
	///
	/// /// ### Example
	/// ```c++
	///	Util::System::out << 3.141592653589793 << Util::Io::PrintStream::ln; // prints "3.141592653589793\n"
	///
	///	Util::System::out.setDecimalPrecision(2);
	///	Util::System::out << 3.141592653589793 << Util::Io::PrintStream::ln; // prints "3.14\n"
	///
	/// Util::System::out.setDecimalPrecision(0);
	///	Util::System::out << 3.141592653589793 << Util::Io::PrintStream::ln; // prints "3\n"
	///
	/// Util::System::out.setDecimalPrecision(0);
	///	Util::System::out << 3.141592653589793 // prints "3"
	///		<< Util::Io::PrintStream::ln << Util::Io::PrintStream::flush; // prints a new line and flushes the stream
	/// ```
	PrintStream& operator<<(double number);

	/// Call a manipulator function via the stream insertion operator.
	/// Manipulator functions can be used to modify the state of the `PrintStream` or
	/// perform specific actions like flushing the stream or printing a new line.
	///
	/// ### Example
	/// ```c++
	/// Util::System::out
	///		<< "Hello, World!" // prints "Hello, World!"
	///		<< Util::Io::PrintStream::ln // prints a new line
	///		<< 255 // prints "255"
	///		<< Util::Io::PrintStream::hex // switch to hexadecimal base
	///		<< 255 // prints "FF"
	///		<< Util::Io::PrintStream::ln // prints a new line
	///		<< Util::Io::PrintStream::flush; // flushes the stream
	/// ```
    PrintStream& operator<<(PrintStream& (*f)(PrintStream&));

	/// Flush the underlying stream via the stream insertion operator.
	///
	/// ### Example
	/// ```c++
	/// Util::System::out
	///		<< "Hello, World!" // prints "Hello, World!"
	///		<< Util::Io::PrintStream::ln // prints a new line
	///		<< Util::Io::PrintStream::flush; // flushes the stream
	/// ```
    static PrintStream& flush(PrintStream& stream);

	/// Print a new line character ('\n') via the stream insertion operator.
	///
	/// ### Example
	/// ```c++
	/// Util::System::out
	///		<< "Hello, World!" // prints "Hello, World!"
	///		<< Util::Io::PrintStream::ln // prints a new line
	///		<< Util::Io::PrintStream::flush; // flushes the stream
	/// ```
    static PrintStream& ln(PrintStream& stream);

	/// Set the integer base to binary (base 2) via the stream insertion operator.
	///
	/// ### Example
	/// ```c++
	/// Util::System::out
	///		<< 255 // prints "255"
	///		<< Util::Io::PrintStream::bin // switch to binary base
	///		<< 255 // prints "11111111"
	///		<< Util::Io::PrintStream::ln << Util::Io::PrintStream::flush; // prints a new line and flushes the stream
	/// ```
	static PrintStream& bin(PrintStream& stream);

	/// Set the integer base to octal (base 8) via the stream insertion operator.
	///
	/// ### Example
	/// ```c++
	/// Util::System::out
	///		<< 255 // prints "255"
	///		<< Util::Io::PrintStream::oct // switch to binary base
	///		<< 255 // prints "377"
	///		<< Util::Io::PrintStream::ln << Util::Io::PrintStream::flush; // prints a new line and flushes the stream
	/// ```
    static PrintStream& oct(PrintStream& stream);

	/// Set the integer base to dec (base 10) via the stream insertion operator.
	///
	/// ### Example
	/// ```c++
	/// Util::System::out
	///		<< 255 // prints "255"
	///		<< Util::Io::PrintStream::bin // switch to binary base
	///		<< 255 // prints "11111111"
	///		<< Util::Io::PrintStream::dec // switch back to decimal base
	///		<< 255 // prints "255"
	///		<< Util::Io::PrintStream::ln << Util::Io::PrintStream::flush; // prints a new line and flushes the stream
	/// ```
	static PrintStream& dec(PrintStream& stream);

	/// Set the integer base to hexadecimal (base 16) via the stream insertion operator.
	///
	/// ### Example
	/// ```c++
	/// Util::System::out
	///		<< 255 // prints "255"
	///		<< Util::Io::PrintStream::hex // switch to binary base
	///		<< 255 // prints "FF"
	///		<< Util::Io::PrintStream::ln << Util::Io::PrintStream::flush; // prints a new line and flushes the stream
	/// ```
    static PrintStream& hex(PrintStream& stream);

private:

    bool flushOnNewLine;
    uint8_t base = 10;
    size_t numberPadding = 0;
	
	bool rightPadding = false;
	
	char negativeSign = '-';
	char positiveSign = '\0';
	char hexNumericBase = 'A';
	
	int32_t minimumIntegerPrecision = 1;
	int32_t decimalPrecision = -1;
	
	bool alwaysPrintDecimalPoint = false;
	
	uint32_t bytesWritten = 0;
	
	String integerPrefix = "";

	static constexpr size_t DEFAULT_DECIMAL_PRECISION = 20;
};

}

#endif
