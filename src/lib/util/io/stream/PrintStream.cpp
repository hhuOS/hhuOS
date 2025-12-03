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

#include "PrintStream.h"

#include "ByteArrayOutputStream.h"
#include "lib/util/io/stream/OutputStream.h"
#include "lib/util/base/Address.h"
#include "lib/util/math/Math.h"

namespace Util {
namespace Io {
bool PrintStream::write(const uint8_t byte) {
	const auto written = write(&byte, 0, 1);
	if (written == 0) {
		return false;
	}

	bytesWritten++;
	return true;
}

size_t PrintStream::write(const uint8_t *sourceBuffer, size_t offset, size_t length) {
	const auto written = FilterOutputStream::write(sourceBuffer, offset, length);
	bytesWritten += written;

	return written;
}

void PrintStream::print(const char *string, const int32_t maxBytes) {
	auto len = Address(string).stringLength();
	if (maxBytes >= 0 && len > static_cast<size_t>(maxBytes)) {
		len = maxBytes;
	}

	write(reinterpret_cast<const uint8_t*>(string), 0, len);
}

void PrintStream::print(uint64_t number, const char sign) {
	ByteArrayOutputStream numberStream{};
	PrintStream formatStream(numberStream);

	size_t div = 1;
	while (number / div >= base) {
		div *= base;
	}

	while (div > 0) {
		const auto digit = static_cast<char>(number / div);

		if (digit < 10) {
			formatStream << static_cast<char>('0' + digit);
		} else {
			formatStream << static_cast<char>(hexNumericBase + digit - 10);
		}

		number %= div;
		div /= base;
	}

	auto fullNumberLength = numberStream.getPosition();
	if (minimumIntegerPrecision >= 0 && fullNumberLength < static_cast<size_t>(minimumIntegerPrecision)) {
		fullNumberLength = minimumIntegerPrecision;
	}
	fullNumberLength += integerPrefix.length();
	if (sign) {
		fullNumberLength++;
	}

	if (!rightPadding) {
		for (uint32_t i = fullNumberLength; i < numberPadding; i++) {
			write(' ');
		}
	}

	if (sign) {
		write(sign);
	}

	print(integerPrefix);

	if (minimumIntegerPrecision >= 0) {
		for (size_t i = numberStream.getPosition(); i < static_cast<size_t>(minimumIntegerPrecision); i++) {
			write('0');
		}
	}

	print(numberStream.getContent());

	if (rightPadding) {
		for (size_t i = numberStream.getPosition(); i < numberPadding; i++) {
			write(' ');
		}
	}
}

void PrintStream::print(double number) {
	ByteArrayOutputStream numberStream;
	PrintStream formatStream(numberStream);

	char sign = '\0';

	if (number < 0) {
		sign = negativeSign;
		number *= -1;
	} else {
		sign = positiveSign;
	}

	int32_t mul = 1;
	while (Math::pow(10.0, mul) <= number) {
		mul++;
	}
	mul--;


	while (mul >= 0) {
		formatStream.write('0' + static_cast<size_t>(number / Math::pow(10.0, mul)) % 10);
		mul--;
	}

	if (decimalPrecision != 0 || alwaysPrintDecimalPoint) {
		formatStream.write('.');
	}
	number -= static_cast<int32_t>(number);

	size_t i = 0;
	while (i < (decimalPrecision >= 0 ? decimalPrecision : DEFAULT_DECIMAL_PRECISION)) {
		number *= 10;

		if (1 - (number - static_cast<uint8_t>(number)) < 0.0001) {
			formatStream.write('0' + static_cast<uint8_t>(number));
			i++;
			break;
		}

		formatStream.write('0' + static_cast<uint8_t>(number));
		number -= static_cast<int32_t>(number);
		i++;

		if (number < 0.0000001) {
			break;
		}
	}

	if (decimalPrecision >= 0) {
		while (i < static_cast<size_t>(decimalPrecision)) {
			formatStream.write('0');
			i++;
		}
	}

	auto fullNumberLength = numberStream.getPosition();
	if (sign) {
		fullNumberLength++;
	}

	if (!rightPadding) {
		for (size_t i = fullNumberLength; i < numberPadding; i++) {
			write(' ');
		}
	}

	if (sign) {
		write(sign);
	}
	print(numberStream.getContent());

	if (rightPadding) {
		for (size_t i = numberStream.getPosition(); i < numberPadding; i++) {
			write(' ');
		}
	}
}

}
}