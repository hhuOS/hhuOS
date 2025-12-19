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
 *
 * The file has been enhanced with more formatting options during a master's thesis, written by Tobias Fabian Oehme.
 * The original source code can be found here: https://github.com/ToboterXP/hhuOS/tree/thesis
 */

#ifndef HHUOS_LIB_UTIL_IO_PRINTSTREAM_H
#define HHUOS_LIB_UTIL_IO_PRINTSTREAM_H

#include <stddef.h>
#include <stdint.h>

#include "util/base/String.h"
#include "util/io/stream/FilterOutputStream.h"

namespace Util {
namespace Io {

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
	explicit PrintStream(OutputStream &stream, bool flushOnNewLine = false) :
		FilterOutputStream(stream), flushOnNewLine(flushOnNewLine) {}

	/// Write a single byte directly to the underlying output stream with no formatting applied.
	bool write(uint8_t byte) override;

	/// Write a buffer of bytes directly to the underlying output stream with no formatting applied.
	size_t write(const uint8_t *sourceBuffer, size_t offset, size_t length) override;

	/// Flush the underlying output stream.
	size_t flush() override {
		return FilterOutputStream::flush();
	}

	/// Get the total number of bytes written to the underlying output stream so far.
	size_t getBytesWritten() const {
		return bytesWritten;
	}

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
	void setBase(const uint8_t newBase) {
		base = newBase;
	}

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
	void setNumberPadding(int32_t padding) {
		if (padding < 0) {
			padding *= -1;
			setNumberJustification(true);
		}

		numberPadding = padding;
	}

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
	void setNumberJustification(const bool leftJustified) {
		rightPadding = leftJustified;
	}

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
	void setPositiveSign(const char sign) {
		positiveSign = sign;
	}

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
	void setNegativeSign(const char sign) {
		negativeSign = sign;
	}

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
	void setIntegerPrecision(const int32_t precision) {
		minimumIntegerPrecision = precision;
	}

	/// Set the padding character to use for integer precision padding.
	/// The default is '0' (zero).
	///
	/// ### Example
	/// ```c++
	/// Util::System::out.setIntegerPrecision(5);
	/// Util::System::out.println(42); // prints "00042"
	///
	/// Util::System::out.setIntegerPrecisionPaddingChar('*');
	/// Util::System::out.println(42); // prints "***42"
	///
	/// Util::System::out.setIntegerPrecisionPaddingChar(' ');
	/// Util::System::out.println(42); // prints "   42"
	///
	/// Util::System::out.flush();
	/// ```
	void setIntegerPrecisionPaddingChar(const char paddingChar) {
		integerPrecisionPaddingChar = paddingChar;
	}

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
	void setDecimalPrecision(const int32_t precision) {
		decimalPrecision = precision;
	}

	/// Set whether to fill the decimal precision with trailing zeros for floating-point numbers.
	/// For example, if the decimal precision is set to 3,
	/// the number 3.1 will be printed as "3.100" if this option is enabled.
	/// The default is false (do not fill with zeros).
	///
	/// ### Example
	/// ```c++
	/// Util::System::out.setDecimalPrecision(3);
	/// Util::System::out.println(3.1); // prints "3.1"
	///
	/// Util::System::out.setFillDecimalPrecisionWithZeros(true);
	/// Util::System::out.println(3.1); // prints "3.100"
	///
	/// Util::System::out.println(3.1234); // prints "3.123"
	///
	/// Util::System::out.flush();
	/// ```
	void setFillDecimalPrecisionWithZeros(const bool fillZeros) {
		fillDecimalPrecisionWithZeros = fillZeros;
	}

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
	void setIntegerPrefix(const String &prefix) {
		integerPrefix = prefix;
	}

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
	void setAlphaNumericBase(const char hexBase) {
		hexNumericBase = hexBase;
	}

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
	void setAlwaysPrintDecimalPoint(const bool value) {
		alwaysPrintDecimalPoint = value;
	}

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
	void print(const char c) {
		write(reinterpret_cast<const uint8_t*>(&c), 0, 1);
	}

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
	void print(const String &string) {
		write(static_cast<const uint8_t*>(string), 0, string.length());
	}

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
	void print(const bool boolean) {
		print(boolean ? "true" : "false");
	}

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
	void print(const int8_t number) {
		print(static_cast<int64_t>(number));
	}

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
	void print(const uint8_t number) {
		print(static_cast<uint64_t>(number));
	}

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
	void print(const int16_t number) {
		print(static_cast<int64_t>(number));
	}

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
	void print(const uint16_t number) {
		print(static_cast<uint64_t>(number));
	}

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
	void print(const int32_t number) {
		print(static_cast<int64_t>(number));
	}

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
	void print(const uint32_t number) {
		print(static_cast<uint64_t>(number));
	}

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
	void print(int64_t number) {
		char sign = '\0';
		if (number < 0) {
			sign = negativeSign;
			number = -number;
		} else {
			sign = positiveSign;
		}

		print(static_cast<uint64_t>(number), sign);
	}

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
	void print(const void *pointer) {
		print(reinterpret_cast<uintptr_t>(pointer));
	}

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
	void println() {
		write('\n');

		if (flushOnNewLine) {
			FilterOutputStream::flush();
		}
	}

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
	void println(char c) {
		print(c);
		println();
	}

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
	void println(const char *string, const int32_t maxBytes = -1) {
		print(string, maxBytes);
		println();
	}

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
	void println(const String &string) {
		print(string);
		println();
	}

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
	void println(const bool boolean) {
		print(boolean);
		println();
	}

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
	void println(const int8_t number) {
		print(number);
		println();
	}

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
	void println(const uint8_t number) {
		print(number);
		println();
	}

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
	void println(const int16_t number) {
		print(number);
		println();
	}

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
	void println(const uint16_t number) {
		print(number);
		println();
	}

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
	void println(const int32_t number) {
		print(number);
		println();
	}

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
	void println(const uint32_t number) {
		print(number);
		println();
	}

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
	void println(const int64_t number) {
		print(number);
		println();
	}

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
	void println(const uint64_t number) {
		print(number);
		println();
	}

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
	void println(const void *pointer) {
		print(pointer);
		println();
	}

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
	PrintStream& operator<<(const char c) {
		write(c);
		return *this;
	}

	/// Print a null-terminated C string via the stream insertion operator.
	///
	/// ### Example
	/// ```c++
	/// Util::System::out
	///		<< "Hello, World!" // prints "Hello, World!"
	///		<< Util::Io::PrintStream::ln << Util::Io::PrintStream::flush; // prints a new line and flushes the stream
	/// ```
	PrintStream& operator<<(const char *string) {
		print(string);
		return *this;
	}

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
	PrintStream& operator<<(const String &string) {
		print(string);
		return *this;
	}

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
	PrintStream& operator<<(const bool boolean) {
		print(boolean);
		return *this;
	}

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
	PrintStream& operator<<(const int8_t number) {
		print(number);
		return *this;
	}

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
	PrintStream& operator<<(const uint8_t number) {
		print(number);
		return *this;
	}

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
	PrintStream& operator<<(const int16_t number) {
		print(number);
		return *this;
	}

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
	PrintStream& operator<<(const uint16_t number) {
		print(number);
		return *this;
	}
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
	PrintStream& operator<<(const int32_t number) {
		print(number);
		return *this;
	}

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
	PrintStream& operator<<(const uint32_t number) {
		print(number);
		return *this;
	}

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
	PrintStream& operator<<(const int64_t number) {
		print(number);
		return *this;
	}

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
	PrintStream& operator<<(const uint64_t number) {
		print(number);
		return *this;
	}

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
	PrintStream& operator<<(const void *pointer) {
		print(pointer);
		return *this;
	}

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
	PrintStream& operator<<(const double number) {
		print(number);
		return *this;
	}

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
	PrintStream& operator<<(PrintStream& (*f)(PrintStream&)) {
		return f(*this);
	}

	/// Flush the underlying stream via the stream insertion operator.
	///
	/// ### Example
	/// ```c++
	/// Util::System::out
	///		<< "Hello, World!" // prints "Hello, World!"
	///		<< Util::Io::PrintStream::ln // prints a new line
	///		<< Util::Io::PrintStream::flush; // flushes the stream
	/// ```
	static PrintStream& flush(PrintStream& stream) {
		stream.flush();
		return stream;
	}

	/// Print a new line character ('\n') via the stream insertion operator.
	///
	/// ### Example
	/// ```c++
	/// Util::System::out
	///		<< "Hello, World!" // prints "Hello, World!"
	///		<< Util::Io::PrintStream::ln // prints a new line
	///		<< Util::Io::PrintStream::flush; // flushes the stream
	/// ```
	static PrintStream& ln(PrintStream& stream) {
		stream.println();
		return stream;
	}

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
	static PrintStream& bin(PrintStream& stream) {
		stream.setBase(2);
		return stream;
	}

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
	static PrintStream& oct(PrintStream& stream) {
		stream.setBase(8);
		return stream;
	}

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
	static PrintStream& dec(PrintStream& stream) {
		stream.setBase(10);
		return stream;
	}

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
	static PrintStream& hex(PrintStream& stream) {
		stream.setBase(16);
		return stream;
	}

private:

	bool flushOnNewLine;
	uint8_t base = 10;
	size_t numberPadding = 0;
	
	bool rightPadding = false;
	
	char negativeSign = '-';
	char positiveSign = '\0';
	char hexNumericBase = 'A';
	
	int32_t minimumIntegerPrecision = 1;
	char integerPrecisionPaddingChar = '0';

	int32_t decimalPrecision = -1;
	bool fillDecimalPrecisionWithZeros = false;
	
	bool alwaysPrintDecimalPoint = false;
	
	uint32_t bytesWritten = 0;
	
	String integerPrefix = "";

	static constexpr size_t DEFAULT_DECIMAL_PRECISION = 20;
};

}
}

#endif
