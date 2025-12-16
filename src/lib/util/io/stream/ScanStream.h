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
 * The scan stream class is based on a master's thesis, written by Tobias Fabian Oehme.
 * The original source code can be found here: https://github.com/ToboterXP/hhuOS/tree/thesis
 */

#ifndef HHUOS_LIB_UTIL_IO_SCANSTREAM_H
#define HHUOS_LIB_UTIL_IO_SCANSTREAM_H

#include <stddef.h>
#include <stdint.h>
#include <stdarg.h>

#include "util/io/stream/FilterInputStream.h"

namespace Util {
namespace Io {

/// An input stream that provides formatted input functions similar to scanf.
/// It supports reading various data types such as integers (in various bases), floating-point numbers,
/// characters, and strings from the underlying input stream.
/// Formatted data can be read by directly calling the corresponding read methods for each data type,
/// using the overloaded stream extraction operator (>>), or by using the `scan()` method
/// with a format string and corresponding arguments (similar to scanf).
class ScanStream final : public FilterInputStream {

public:
	/// Create a scan stream instance that reads from the given input stream.
	explicit ScanStream(InputStream &stream) : FilterInputStream(stream) {}

	/// Read a single byte directly from the underlying input stream.
	int16_t read() override;

	/// Read multiple bytes directly from the underlying input stream into the provided buffer.
	int32_t read(uint8_t *targetBuffer, size_t offset, size_t length) override;

	/// Peek at the next byte in the underlying input stream without consuming it.
	int16_t peek() override;

	/// Check if the stream is ready to read without blocking.
	bool isReadyToRead() override;

	/// Get the total number of bytes read from the underlying input stream so far.
	size_t getReadBytes() const {
		return readBytes;
	}

	/// Read a signed 8-bit integer from the stream, interpreting the input according to the specified base.
	/// The default base is 0, which means the base is auto-detected (e.g. "0x" prefix for hex, "0" prefix for octal).
	/// Whitespaces are skipped before reading the number.
	/// An optional '+' or '-' sign is supported for signed numbers.
	/// If an illegal character is encountered before any digits are read,
	/// 0 is returned and the stream position remains unchanged.
	///
	/// ### Example
	/// ```c++
	/// // Set up a scan stream that reads from a string
	/// const auto *string = "42 -42 0xff 0b101010 3.14";
	/// auto byteStream = Util::Io::ByteArrayInputStream(reinterpret_cast<const uint8_t*>(string));
	/// auto scanStream = Util::Io::ScanStream(byteStream);
	///
	/// const auto number1 = scanStream.readSigned8(); // number1 = 42
	/// const auto number2 = scanStream.readSigned8(); // number2 = -42
	/// const auto number3 = scanStream.readSigned8(); // number3 = -1
	/// const auto number4 = scanStream.readSigned8(); // number4 = 42 (0b101010 in binary)
	/// const auto number5 = scanStream.readSigned8(); // number5 = 3 (reads integer part of 3.14)
	/// const auto number6 = scanStream.readSigned8(); // number6 = 0 (stream stops reading at illegal character '.')
	/// ```
	int8_t readSigned8(const uint8_t base = 0) {
		const auto value = readSigned64(base);
		return static_cast<int8_t>(value);
	}

	/// Read an unsigned 8-bit integer from the stream, interpreting the input according to the specified base.
	/// The default base is 0, which means the base is auto-detected (e.g. "0x" prefix for hex, "0" prefix for octal).
	/// If the read value is negative it is clamped to 0.
	/// Whitespaces are skipped before reading the number.
	/// An optional '+' or '-' sign is supported for signed numbers.
	/// If an illegal character is encountered before any digits are read,
	/// 0 is returned and the stream position remains unchanged.
	///
	/// ### Example
	/// ```c++
	/// // Set up a scan stream that reads from a string
	/// const auto *string = "42 -42 0xff 0b101010 3.14";
	/// auto byteStream = Util::Io::ByteArrayInputStream(reinterpret_cast<const uint8_t*>(string));
	/// auto scanStream = Util::Io::ScanStream(byteStream);
	///
	/// const auto number1 = scanStream.readUnsigned8(); // number1 = 42
	/// const auto number2 = scanStream.readUnsigned8(); // number2 = 0 (-42 is negative -> clamped to 0)
	/// const auto number3 = scanStream.readUnsigned8(); // number3 = 255
	/// const auto number4 = scanStream.readUnsigned8(); // number4 = 42 (0b101010 in binary)
	/// const auto number5 = scanStream.readUnsigned8(); // number5 = 3 (reads integer part of 3.14)
	/// const auto number6 = scanStream.readUnsigned8(); // number6 = 0 (stream stops reading at illegal character '.')
	/// ```
	uint8_t readUnsigned8(const uint8_t base = 0) {
		const auto value = readSigned64(base);
		return static_cast<uint8_t>(value) * (value > 0);
	}

	/// Read a signed 16-bit integer from the stream, interpreting the input according to the specified base.
	/// The default base is 0, which means the base is auto-detected (e.g. "0x" prefix for hex, "0" prefix for octal).
	/// Whitespaces are skipped before reading the number.
	/// An optional '+' or '-' sign is supported for signed numbers.
	/// If an illegal character is encountered before any digits are read,
	/// 0 is returned and the stream position remains unchanged.
	///
	/// ### Example
	/// ```c++
	/// // Set up a scan stream that reads from a string
	/// const auto *string = "42 -42 0xffff 0b101010 3.14";
	/// auto byteStream = Util::Io::ByteArrayInputStream(reinterpret_cast<const uint8_t*>(string));
	/// auto scanStream = Util::Io::ScanStream(byteStream);
	///
	/// const auto number1 = scanStream.readSigned16(); // number1 = 42
	/// const auto number2 = scanStream.readSigned16(); // number2 = -42
	/// const auto number3 = scanStream.readSigned16(); // number3 = -1
	/// const auto number4 = scanStream.readSigned16(); // number4 = 42 (0b101010 in binary)
	/// const auto number5 = scanStream.readSigned16(); // number5 = 3 (reads integer part of 3.14)
	/// const auto number6 = scanStream.readSigned16(); // number6 = 0 (stream stops reading at illegal character '.')
	/// ```
	int16_t readSigned16(const uint8_t base = 0) {
		const auto value = readSigned64(base);
		return static_cast<int16_t>(value);
	}

	/// Read an unsigned 16-bit integer from the stream, interpreting the input according to the specified base.
	/// The default base is 0, which means the base is auto-detected (e.g. "0x" prefix for hex, "0" prefix for octal).
	/// If the read value is negative it is clamped to 0.
	/// Whitespaces are skipped before reading the number.
	/// An optional '+' or '-' sign is supported for signed numbers.
	/// If an illegal character is encountered before any digits are read,
	/// 0 is returned and the stream position remains unchanged.
	///
	/// ### Example
	/// ```c++
	/// // Set up a scan stream that reads from a string
	/// const auto *string = "42 -42 0xffff 0b101010 3.14";
	/// auto byteStream = Util::Io::ByteArrayInputStream(reinterpret_cast<const uint8_t*>(string));
	/// auto scanStream = Util::Io::ScanStream(byteStream);
	///
	/// const auto number1 = scanStream.readUnsigned16(); // number1 = 42
	/// const auto number2 = scanStream.readUnsigned16(); // number2 = 0 (-42 is negative -> clamped to 0)
	/// const auto number3 = scanStream.readUnsigned16(); // number3 = 65535
	/// const auto number4 = scanStream.readUnsigned16(); // number4 = 42 (0b101010 in binary)
	/// const auto number5 = scanStream.readUnsigned16(); // number5 = 3 (reads integer part of 3.14)
	/// const auto number6 = scanStream.readUnsigned16(); // number6 = 0 (stream stops reading at illegal character '.')
	/// ```
	uint16_t readUnsigned16(const uint8_t base = 0) {
		const auto value = readSigned64(base);
		return static_cast<uint16_t>(value) * (value > 0);
	}

	/// Read a signed 32-bit integer from the stream, interpreting the input according to the specified base.
	/// The default base is 0, which means the base is auto-detected (e.g. "0x" prefix for hex, "0" prefix for octal).
	/// Whitespaces are skipped before reading the number.
	/// An optional '+' or '-' sign is supported for signed numbers.
	/// If an illegal character is encountered before any digits are read,
	/// 0 is returned and the stream position remains unchanged.
	///
	/// ### Example
	/// ```c++
	/// // Set up a scan stream that reads from a string
	/// const auto *string = "42 -42 0xffffffff 0b101010 3.14";
	/// auto byteStream = Util::Io::ByteArrayInputStream(reinterpret_cast<const uint8_t*>(string));
	/// auto scanStream = Util::Io::ScanStream(byteStream);
	///
	/// const auto number1 = scanStream.readSigned32(); // number1 = 42
	/// const auto number2 = scanStream.readSigned32(); // number2 = -42
	/// const auto number3 = scanStream.readSigned32(); // number3 = -1
	/// const auto number4 = scanStream.readSigned32(); // number4 = 42 (0b101010 in binary)
	/// const auto number5 = scanStream.readSigned32(); // number5 = 3 (reads integer part of 3.14)
	/// const auto number6 = scanStream.readSigned32(); // number6 = 0 (stream stops reading at illegal character '.')
	/// ```
	int32_t readSigned32(const uint8_t base = 0) {
		const auto value = readSigned64(base);
		return static_cast<int32_t>(value);
	}

	/// Read an unsigned 32-bit integer from the stream, interpreting the input according to the specified base.
	/// The default base is 0, which means the base is auto-detected (e.g. "0x" prefix for hex, "0" prefix for octal).
	/// If the read value is negative it is clamped to 0.
	/// Whitespaces are skipped before reading the number.
	/// An optional '+' or '-' sign is supported for signed numbers.
	/// If an illegal character is encountered before any digits are read,
	/// 0 is returned and the stream position remains unchanged.
	///
	/// ### Example
	/// ```c++
	/// // Set up a scan stream that reads from a string
	/// const auto *string = "42 -42 0xffffffff 0b101010 3.14";
	/// auto byteStream = Util::Io::ByteArrayInputStream(reinterpret_cast<const uint8_t*>(string));
	/// auto scanStream = Util::Io::ScanStream(byteStream);
	///
	/// const auto number1 = scanStream.readUnsigned32(); // number1 = 42
	/// const auto number2 = scanStream.readUnsigned32(); // number2 = 0 (-42 is negative -> clamped to 0)
	/// const auto number3 = scanStream.readUnsigned32(); // number3 = 4294967295
	/// const auto number4 = scanStream.readUnsigned32(); // number4 = 42 (0b101010 in binary)
	/// const auto number5 = scanStream.readUnsigned32(); // number5 = 3 (reads integer part of 3.14)
	/// const auto number6 = scanStream.readUnsigned32(); // number6 = 0 (stream stops reading at illegal character '.')
	/// ```
	uint32_t readUnsigned32(const uint8_t base = 0) {
		const auto value = readSigned64(base);
		return static_cast<uint32_t>(value) * (value > 0);
	}

	/// Read a signed 64-bit integer from the stream, interpreting the input according to the specified base.
	/// The default base is 0, which means the base is auto-detected (e.g. "0x" prefix for hex, "0" prefix for octal).
	/// Whitespaces are skipped before reading the number.
	/// An optional '+' or '-' sign is supported for signed numbers.
	/// If an illegal character is encountered before any digits are read,
	/// 0 is returned and the stream position remains unchanged.
	///
	/// ### Example
	/// ```c++
	/// // Set up a scan stream that reads from a string
	/// const auto *string = "42 -42 0xffffffffffffffff 0b101010 3.14";
	/// auto byteStream = Util::Io::ByteArrayInputStream(reinterpret_cast<const uint8_t*>(string));
	/// auto scanStream = Util::Io::ScanStream(byteStream);
	///
	/// const auto number1 = scanStream.readSigned32(); // number1 = 42
	/// const auto number2 = scanStream.readSigned32(); // number2 = -42
	/// const auto number3 = scanStream.readSigned32(); // number3 = -1
	/// const auto number4 = scanStream.readSigned32(); // number4 = 42 (0b101010 in binary)
	/// const auto number5 = scanStream.readSigned32(); // number5 = 3 (reads integer part of 3.14)
	/// const auto number6 = scanStream.readSigned32(); // number6 = 0 (stream stops reading at illegal character '.')
	/// ```
	int64_t readSigned64(uint8_t base = 0);

	/// Read an unsigned 64-bit integer from the stream, interpreting the input according to the specified base.
	/// The default base is 0, which means the base is auto-detected (e.g. "0x" prefix for hex, "0" prefix for octal).
	/// If the read value is negative it is clamped to 0.
	/// Whitespaces are skipped before reading the number.
	/// An optional '+' or '-' sign is supported for signed numbers.
	/// If an illegal character is encountered before any digits are read,
	/// 0 is returned and the stream position remains unchanged.
	///
	/// ### Example
	/// ```c++
	/// // Set up a scan stream that reads from a string
	/// const auto *string = "42 -42 0xffffffffffffffff 0b101010 3.14";
	/// auto byteStream = Util::Io::ByteArrayInputStream(reinterpret_cast<const uint8_t*>(string));
	/// auto scanStream = Util::Io::ScanStream(byteStream);
	///
	/// const auto number1 = scanStream.readUnsigned32(); // number1 = 42
	/// const auto number2 = scanStream.readUnsigned32(); // number2 = 0 (-42 is negative -> clamped to 0)
	/// const auto number3 = scanStream.readUnsigned32(); // number3 = 18446744073709551615
	/// const auto number4 = scanStream.readUnsigned32(); // number4 = 42 (0b101010 in binary)
	/// const auto number5 = scanStream.readUnsigned32(); // number5 = 3 (reads integer part of 3.14)
	/// const auto number6 = scanStream.readUnsigned32(); // number6 = 0 (stream stops reading at illegal character '.')
	/// ```
	uint64_t readUnsigned64(const uint8_t base = 0) {
		const auto value = readSigned64(base);
		return static_cast<uint64_t>(value) * (value > 0);
	}

	/// Read a floating point number from the stream.
	/// The number can be in standard decimal notation (e.g. "3.14") or scientific notation (e.g. "1.5e10").
	/// Whitespaces are skipped before reading the number.
	/// An optional '+' or '-' sign is supported.
	/// If an illegal character is encountered before any digits are read,
	/// 0.0 is returned and the stream position remains unchanged.
	///
	/// ### Example
	/// ```c++
	/// // Set up a scan stream that reads from a string
	/// const auto *string = "42, -42.5, 3.14e2, -1.5E-3";
	/// auto byteStream = Util::Io::ByteArrayInputStream(reinterpret_cast<const uint8_t*>(string));
	/// auto scanStream = Util::Io::ScanStream(byteStream);
	///
	/// const auto number1 = scanStream.readDouble(); // number1 = 42.0
	/// const auto number2 = scanStream.readDouble(); // number2 = -42.5
	/// const auto number3 = scanStream.readDouble(); // number3 = 314.0
	/// const auto number4 = scanStream.readDouble(); // number4 = -0.0015
	/// ```
	long double readFloatingPointNumber();

	/// Read a single wide character (`wchar_t`) from the stream.
	///
	/// ### Example
	/// ```c++
	/// // Set up a scan stream that reads from a string
	/// const auto *string = "abc €äöü";
	/// auto byteStream = Util::Io::ByteArrayInputStream(reinterpret_cast<const uint8_t*>(string));
	/// auto scanStream = Util::Io::ScanStream(byteStream);
	///
	/// const auto char1 = scanStream.readWideCharacter(); // char1 = 'a'
	/// const auto char2 = scanStream.readWideCharacter(); // char2 = 'b'
	/// const auto char3 = scanStream.readWideCharacter(); // char3 = 'c'
	/// const auto char4 = scanStream.readWideCharacter(); // char4 = ' '
	/// const auto char5 = scanStream.readWideCharacter(); // char5 = '€'
	/// const auto char6 = scanStream.readWideCharacter(); // char6 = 'ä'
	/// const auto char7 = scanStream.readWideCharacter(); // char7 = 'ö'
	/// const auto char8 = scanStream.readWideCharacter(); // char8 = 'ü'
	/// ```
	wchar_t readWideCharacter();

	/// Read formatted input from the stream according to the specified format string.
	/// Supports all format specifiers defined by `scanf()` in the C89 standard.
	/// The return value is the number of input items successfully matched and assigned,
	/// which can be less than the number requested if a matching failure occurs.
	/// A single whitespace in the format string consumes all whitespace characters in the stream.
	/// Literal characters in the format string must match exactly in the input stream.
	///
	/// ### Example
	/// ```
	/// // Set up a scan stream that reads from a string
	/// const auto *string = "42 -42 skip 3.14 abcdef €äöü";
	/// auto byteStream = Util::Io::ByteArrayInputStream(reinterpret_cast<const uint8_t*>(string));
	/// auto scanStream = Util::Io::ScanStream(byteStream);
	///
	/// int number1, number2;
	/// double number3;
	/// char str1[4];
	/// char str2[4];
	/// wchar_t wideStr[5];
	///
	/// // number1 = 42, number2 = -42, number3 = 3.14, str1 = "abc", str2 = "def", wideStr = "€äöü", itemsRead = 6
	/// const auto itemsRead = scanStream.scan("%d %d skip %lf %3s %3s %ls",
	///		&number1, &number2, &number3, str1, str2, wideStr);
	/// ```
	int32_t scan(const char* format, ...);

	/// Read formatted input from the stream according to the specified format string.
	/// Supports all format specifiers defined by `scanf()` in the C89 standard.
	/// The return value is the number of input items successfully matched and assigned,
	/// which can be less than the number requested if a matching failure occurs.
	/// A single whitespace in the format string consumes all whitespace characters in the stream.
	/// Literal characters in the format string must match exactly in the input stream.
	///
	/// The arguments are passed as a `va_list`, which is useful if the calling function itself is variadic
	/// and wants to pass the arguments to this function. For example, `scanf()` is implemented using this function.
	int32_t scan(const char* format, va_list args);

	/// Read a signed 8-bit integer from the stream via the stream extraction operator.
	/// This operator is overloaded to support signed and unsigned 8-bit, 16-bit, 32-bit, and 64-bit integers.
	/// The base is auto-detected (e.g. "0x" prefix for hex, "0" prefix for octal) and defaults to 10 (decimal).
	/// Whitespaces are skipped before reading the number.
	/// An optional '+' or '-' sign is supported for signed numbers.
	/// If an illegal character is encountered before any digits are read,
	/// 0 is returned and the stream position remains unchanged.
	///
	/// ### Example
	/// ```c++
	/// // Set up a scan stream that reads from a string
	/// const auto *string = "42 -42 0xff 0b101010 3.14";
	/// auto byteStream = Util::Io::ByteArrayInputStream(reinterpret_cast<const uint8_t*>(string));
	/// auto scanStream = Util::Io::ScanStream(byteStream);
	///
	/// int8_t number1, number2, number3, number4, number5, number6;
	///
	/// // number1 = 42, number2 = -42, number3 = -1, number4 = 42,
	/// // number5 = 3 (reads integer part of 3.14), number6 = 0 (stream stops reading at illegal character '.')
	///	scanStream >> number1 >> number2 >> number3 >> number4 >> number5 >> number6;
	/// ```
	ScanStream& operator>>(int8_t &number) {
		number = readSigned8();
		return *this;
	}

	/// Read an unsigned 8-bit integer from the stream via the stream extraction operator.
	/// This operator is overloaded to support signed and unsigned 8-bit, 16-bit, 32-bit, and 64-bit integers.
	/// The base is auto-detected (e.g. "0x" prefix for hex, "0" prefix for octal) and defaults to 10 (decimal).
	/// If the read value is negative it is clamped to 0.
	/// Whitespaces are skipped before reading the number.
	/// An optional '+' or '-' sign is supported for signed numbers.
	/// If an illegal character is encountered before any digits are read,
	/// 0 is returned and the stream position remains unchanged.
	///
	/// ### Example
	/// ```c++
	/// // Set up a scan stream that reads from a string
	/// const auto *string = "42 -42 0xff 0b101010 3.14";
	/// auto byteStream = Util::Io::ByteArrayInputStream(reinterpret_cast<const uint8_t*>(string));
	/// auto scanStream = Util::Io::ScanStream(byteStream);
	///
	/// uint8_t number1, number2, number3, number4, number5, number6;
	///
	/// // number1 = 42, number2 = 0 (negative -> clamped to 0), number3 = 255, number4 = 42,
	/// // number5 = 3 (reads integer part of 3.14), number6 = 0 (stream stops reading at illegal character '.')
	///	scanStream >> number1 >> number2 >> number3 >> number4 >> number5 >> number6;
	/// ```
	ScanStream& operator>>(uint8_t &number) {
		number = readUnsigned8();
		return *this;
	}

	/// Read a signed 16-bit integer from the stream via the stream extraction operator.
	/// This operator is overloaded to support signed and unsigned 8-bit, 16-bit, 32-bit, and 64-bit integers.
	/// The base is auto-detected (e.g. "0x" prefix for hex, "0" prefix for octal) and defaults to 10 (decimal).
	/// Whitespaces are skipped before reading the number.
	/// An optional '+' or '-' sign is supported for signed numbers.
	/// If an illegal character is encountered before any digits are read,
	/// 0 is returned and the stream position remains unchanged.
	///
	/// ### Example
	/// ```c++
	/// // Set up a scan stream that reads from a string
	/// const auto *string = "42 -42 0xffff 0b101010 3.14";
	/// auto byteStream = Util::Io::ByteArrayInputStream(reinterpret_cast<const uint8_t*>(string));
	/// auto scanStream = Util::Io::ScanStream(byteStream);
	///
	/// int16_t number1, number2, number3, number4, number5, number6;
	///
	/// // number1 = 42, number2 = -42, number3 = -1, number4 = 42,
	/// // number5 = 3 (reads integer part of 3.14), number6 = 0 (stream stops reading at illegal character '.')
	///	scanStream >> number1 >> number2 >> number3 >> number4 >> number5 >> number6;
	/// ```
	ScanStream& operator>>(int16_t &number) {
		number = readSigned16();
		return *this;
	}

	/// Read an unsigned 16-bit integer from the stream via the stream extraction operator.
	/// This operator is overloaded to support signed and unsigned 8-bit, 16-bit, 32-bit, and 64-bit integers.
	/// The base is auto-detected (e.g. "0x" prefix for hex, "0" prefix for octal) and defaults to 10 (decimal).
	/// If the read value is negative it is clamped to 0.
	/// Whitespaces are skipped before reading the number.
	/// An optional '+' or '-' sign is supported for signed numbers.
	/// If an illegal character is encountered before any digits are read,
	/// 0 is returned and the stream position remains unchanged.
	///
	/// ### Example
	/// ```c++
	/// // Set up a scan stream that reads from a string
	/// const auto *string = "42 -42 0xffff 0b101010 3.14";
	/// auto byteStream = Util::Io::ByteArrayInputStream(reinterpret_cast<const uint8_t*>(string));
	/// auto scanStream = Util::Io::ScanStream(byteStream);
	///
	/// uint16_t number1, number2, number3, number4, number5, number6;
	///
	/// // number1 = 42, number2 = 0 (negative -> clamped to 0), number3 = 32767, number4 = 42,
	/// // number5 = 3 (reads integer part of 3.14), number6 = 0 (stream stops reading at illegal character '.')
	///	scanStream >> number1 >> number2 >> number3 >> number4 >> number5 >> number6;
	/// ```
	ScanStream& operator>>(uint16_t &number) {
		number = readUnsigned16();
		return *this;
	}

	/// Read a signed 32-bit integer from the stream via the stream extraction operator.
	/// This operator is overloaded to support signed and unsigned 8-bit, 16-bit, 32-bit, and 64-bit integers.
	/// The base is auto-detected (e.g. "0x" prefix for hex, "0" prefix for octal) and defaults to 10 (decimal).
	/// Whitespaces are skipped before reading the number.
	/// An optional '+' or '-' sign is supported for signed numbers.
	/// If an illegal character is encountered before any digits are read,
	/// 0 is returned and the stream position remains unchanged.
	///
	/// ### Example
	/// ```c++
	/// // Set up a scan stream that reads from a string
	/// const auto *string = "42 -42 0xffffffff 0b101010 3.14";
	/// auto byteStream = Util::Io::ByteArrayInputStream(reinterpret_cast<const uint8_t*>(string));
	/// auto scanStream = Util::Io::ScanStream(byteStream);
	///
	/// int32_t number1, number2, number3, number4, number5, number6;
	///
	/// // number1 = 42, number2 = -42, number3 = -1, number4 = 42,
	/// // number5 = 3 (reads integer part of 3.14), number6 = 0 (stream stops reading at illegal character '.')
	///	scanStream >> number1 >> number2 >> number3 >> number4 >> number5 >> number6;
	/// ```
	ScanStream& operator>>(int32_t &number) {
		number = readSigned32();
		return *this;
	}

	/// Read an unsigned 32-bit integer from the stream via the stream extraction operator.
	/// This operator is overloaded to support signed and unsigned 8-bit, 16-bit, 32-bit, and 64-bit integers.
	/// The base is auto-detected (e.g. "0x" prefix for hex, "0" prefix for octal) and defaults to 10 (decimal).
	/// If the read value is negative it is clamped to 0.
	/// Whitespaces are skipped before reading the number.
	/// An optional '+' or '-' sign is supported for signed numbers.
	/// If an illegal character is encountered before any digits are read,
	/// 0 is returned and the stream position remains unchanged.
	///
	/// ### Example
	/// ```c++
	/// // Set up a scan stream that reads from a string
	/// const auto *string = "42 -42 0xffffffff 0b101010 3.14";
	/// auto byteStream = Util::Io::ByteArrayInputStream(reinterpret_cast<const uint8_t*>(string));
	/// auto scanStream = Util::Io::ScanStream(byteStream);
	///
	/// uint32_t number1, number2, number3, number4, number5, number6;
	///
	/// // number1 = 42, number2 = 0 (negative -> clamped to 0), number3 = 2147483647, number4 = 42,
	/// // number5 = 3 (reads integer part of 3.14), number6 = 0 (stream stops reading at illegal character '.')
	///	scanStream >> number1 >> number2 >> number3 >> number4 >> number5 >> number6;
	/// ```
	ScanStream& operator>>(uint32_t &number) {
		number = readUnsigned32();
		return *this;
	}

	/// Read a signed 64-bit integer from the stream via the stream extraction operator.
	/// This operator is overloaded to support signed and unsigned 8-bit, 16-bit, 32-bit, and 64-bit integers.
	/// The base is auto-detected (e.g. "0x" prefix for hex, "0" prefix for octal) and defaults to 10 (decimal).
	/// Whitespaces are skipped before reading the number.
	/// An optional '+' or '-' sign is supported for signed numbers.
	/// If an illegal character is encountered before any digits are read,
	/// 0 is returned and the stream position remains unchanged.
	///
	/// ### Example
	/// ```c++
	/// // Set up a scan stream that reads from a string
	/// const auto *string = "42 -42 0xffffffff 0b101010 3.14";
	/// auto byteStream = Util::Io::ByteArrayInputStream(reinterpret_cast<const uint8_t*>(string));
	/// auto scanStream = Util::Io::ScanStream(byteStream);
	///
	/// int64_t number1, number2, number3, number4, number5, number6;
	///
	/// // number1 = 42, number2 = -42, number3 = -1, number4 = 42,
	/// // number5 = 3 (reads integer part of 3.14), number6 = 0 (stream stops reading at illegal character '.')
	///	scanStream >> number1 >> number2 >> number3 >> number4 >> number5 >> number6;
	/// ```
	ScanStream& operator>>(int64_t &number) {
		number = readSigned64();
		return *this;
	}

	/// Read an unsigned 64-bit integer from the stream via the stream extraction operator.
	/// This operator is overloaded to support signed and unsigned 8-bit, 16-bit, 32-bit, and 64-bit integers.
	/// The base is auto-detected (e.g. "0x" prefix for hex, "0" prefix for octal) and defaults to 10 (decimal).
	/// If the read value is negative it is clamped to 0.
	/// Whitespaces are skipped before reading the number.
	/// An optional '+' or '-' sign is supported for signed numbers.
	/// If an illegal character is encountered before any digits are read,
	/// 0 is returned and the stream position remains unchanged.
	///
	/// ### Example
	/// ```c++
	/// // Set up a scan stream that reads from a string
	/// const auto *string = "42 -42 0xffffffff 0b101010 3.14";
	/// auto byteStream = Util::Io::ByteArrayInputStream(reinterpret_cast<const uint8_t*>(string));
	/// auto scanStream = Util::Io::ScanStream(byteStream);
	///
	/// uint64_t number1, number2, number3, number4, number5, number6;
	///
	/// // number1 = 42, number2 = 0 (negative -> clamped to 0), number3 = 18446744073709551615, number4 = 42,
	/// // number5 = 3 (reads integer part of 3.14), number6 = 0 (stream stops reading at illegal character '.')
	///	scanStream >> number1 >> number2 >> number3 >> number4 >> number5 >> number6;
	/// ```
	ScanStream& operator>>(uint64_t &number) {
		number = readUnsigned64();
		return *this;
	}

	/// Read a floating point number from the stream via the stream extraction operator.
	/// This operator is overloaded to support `float`, `double`, and `long double` types.
	/// The number can be in standard decimal notation (e.g. "3.14") or scientific notation (e.g. "1.5e10").
	/// Whitespaces are skipped before reading the number.
	/// An optional '+' or '-' sign is supported.
	/// If an illegal character is encountered before any digits are read,
	/// 0.0 is returned and the stream position remains unchanged.
	///
	/// ### Example
	/// ```c++
	/// // Set up a scan stream that reads from a string
	/// const auto *string = "42, -42.5, 3.14e2, -1.5E-3";
	/// auto byteStream = Util::Io::ByteArrayInputStream(reinterpret_cast<const uint8_t*>(string));
	/// auto scanStream = Util::Io::ScanStream(byteStream);
	///
	/// float number1, number2, number3, number4;
	///
	/// // number1 = 42.0, number2 = -42.5, number3 = 314.0, number4 = -0.0015
	/// scanStream >> number1 >> number2 >> number3 >> number4;
	/// ```
	ScanStream& operator>>(float &number) {
		number = static_cast<float>(readFloatingPointNumber());
		return *this;
	}

	/// Read a floating point number from the stream via the stream extraction operator.
	/// This operator is overloaded to support `float`, `double`, and `long double` types.
	/// The number can be in standard decimal notation (e.g. "3.14") or scientific notation (e.g. "1.5e10").
	/// Whitespaces are skipped before reading the number.
	/// An optional '+' or '-' sign is supported.
	/// If an illegal character is encountered before any digits are read,
	/// 0.0 is returned and the stream position remains unchanged.
	///
	/// ### Example
	/// ```c++
	/// // Set up a scan stream that reads from a string
	/// const auto *string = "42, -42.5, 3.14e2, -1.5E-3";
	/// auto byteStream = Util::Io::ByteArrayInputStream(reinterpret_cast<const uint8_t*>(string));
	/// auto scanStream = Util::Io::ScanStream(byteStream);
	///
	/// double number1, number2, number3, number4;
	///
	/// // number1 = 42.0, number2 = -42.5, number3 = 314.0, number4 = -0.0015
	/// scanStream >> number1 >> number2 >> number3 >> number4;
	/// ```
	ScanStream& operator>>(double &number) {
		number = static_cast<double>(readFloatingPointNumber());
		return *this;
	}

	/// Read a floating point number from the stream via the stream extraction operator.
	/// This operator is overloaded to support `float`, `double`, and `long double` types.
	/// The number can be in standard decimal notation (e.g. "3.14") or scientific notation (e.g. "1.5e10").
	/// Whitespaces are skipped before reading the number.
	/// An optional '+' or '-' sign is supported.
	/// If an illegal character is encountered before any digits are read,
	/// 0.0 is returned and the stream position remains unchanged.
	///
	/// ### Example
	/// ```c++
	/// // Set up a scan stream that reads from a string
	/// const auto *string = "42, -42.5, 3.14e2, -1.5E-3";
	/// auto byteStream = Util::Io::ByteArrayInputStream(reinterpret_cast<const uint8_t*>(string));
	/// auto scanStream = Util::Io::ScanStream(byteStream);
	///
	/// long double number1, number2, number3, number4;
	///
	/// // number1 = 42.0, number2 = -42.5, number3 = 314.0, number4 = -0.0015
	/// scanStream >> number1 >> number2 >> number3 >> number4;
	/// ```
	ScanStream& operator>>(long double &number) {
		number = readFloatingPointNumber();
		return *this;
	}

	/// Read a single wide character (`wchar_t`) from the stream via the stream extraction operator.
	///
	/// ### Example
	/// ```c++
	/// // Set up a scan stream that reads from a string
	/// const auto *string = "abc €äöü";
	/// auto byteStream = Util::Io::ByteArrayInputStream(reinterpret_cast<const uint8_t*>(string));
	/// auto scanStream = Util::Io::ScanStream(byteStream);
	///
	/// auto char1, char2, char3, char4, char5, char6, char7, char8;
	///
	/// // char1 = 'a', char2 = 'b', char3 = 'c', char4 = ' ', char5 = '€', char6 = 'ä', char7 = 'ö', char8 = 'ü'
	///	scanStream >> char1 >> char2 >> char3 >> char4 >> char5 >> char6 >> char7 >> char8;
	/// ```
	ScanStream& operator>>(wchar_t &character) {
		character = readWideCharacter();
		return *this;
	}

private:

	/// Set a limit for the number of bytes that can be read from the stream.
	/// If the number of total bytes read reaches this limit, further read operations will fail.
	/// The default limit is -1, which means no limit.
	///
	/// This method is used by `scan()` method to enforce the limit on the number of bytes read,
	/// if the format string contains a maximum field width specifier (e.g. `%10s`).
	void setReadLimit(const int64_t limit) {
		readLimit = limit;
	}

	/// Parse an integer from a single character according to the specified base.
	/// Returns the integer value of the character, or -1 if the character is not valid for the base.
	///
	/// This method is used internally by the number reading methods to convert characters to their integer values.
	/// For example, 'A' is converted to 10 in base 16 (hexadecimal), but -1 in base 10 (decimal).
	static int8_t charToInt(char c, uint8_t base);

	size_t readBytes = 0;
	int64_t readLimit = -1;
};

}
}

#endif