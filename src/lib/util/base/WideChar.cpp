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
 * The C standard library for hhuOS has been implemented during a master's thesis by Tobias Fabian Oehme.
 * The original source code can be found here: https://github.com/ToboterXP/hhuOS/tree/thesis
 */

#include "WideChar.h"

#include "base/Panic.h"

 namespace Util::WideChar {

int utf8Length(const char *bytes, const size_t maxLength) {
	if (bytes == nullptr) {
		return 0;
	}

	if (*bytes == '\0') {
		return 0;
	}

	if (maxLength == 0) {
		return -1;
	}
	
	size_t length = 0;
	
	if ((*bytes & 0b10000000) == 0b00000000) {
		length = 1; // Type 0b0xxxxxxx
	} else if ((*bytes & 0b11100000) == 0b11000000) {
		length = 2; // Type 0b110xxxxx
	} else if ((*bytes & 0b11110000) == 0b11100000) {
		length = 3; // Type 0b1110xxxx
	} else if ((*bytes & 0b11111000) == 0b11110000) {
		length = 4; // Type 0b11110xxx
	}
	
	if (length > maxLength) {
		return -1;
	}
	
	for (size_t i = 1; i < length; i++) {
		if ((*(bytes + i) & 0b11000000) != 0b10000000) {
			return -1; // Check that following bytes have 0x10xxxxxx form
		}
	}
	
	return static_cast<int>(length);
}

int utf8ToWchar(wchar_t *wideChar, const char *bytes, const size_t maxLength) {
	const size_t length = utf8Length(bytes, maxLength);

	if (*bytes == '\0') {
		return static_cast<int>(length); // Same error handling as mblen
	}

	if (wideChar == nullptr) {
		return static_cast<int>(length);
	}

	size_t firstByteLength = 0;
	switch (length) {
		case 1:
			*wideChar = static_cast<wchar_t>(*bytes);
			if (*bytes == 0) {
				return 0;
			}
			return static_cast<int>(length);
		case 2:
			firstByteLength = 5;
			break;
		case 3:
			firstByteLength = 4;
			break;
		case 4:
			firstByteLength = 3;
			break;
		default:
			Panic::fire(Panic::INVALID_ARGUMENT, "Invalid UTF-8 character length");
	}
	
	*wideChar = 0;
	size_t offset = 0;
	for (int i = static_cast<int>(length) - 1; i > 0; i--, offset += 6) {
		// Assemble the values of the following bytes
		*wideChar |= (static_cast<wchar_t>(*(bytes + i)) & 0b00111111) << offset;
	}

	*wideChar |= (static_cast<wchar_t>(*bytes) & ((1 << firstByteLength) - 1)) << offset; // Add first byte value

	return static_cast<int>(length);
}

int wcharToUtf8(char * bytes, const wchar_t wideChar) {
	if (wideChar >= 0x110000) {
		return -1;
	}

	if (bytes == nullptr) {
		return 0;
	}
	
	size_t len = 1;
	if (wideChar >= 0x10000) {
		len = 4;
	} else if (wideChar >= 0x800) {
		len = 3;
	} else if (wideChar >= 0x80) {
		len = 2;
	}
	
	switch (len) {
		case 1:
			*bytes = static_cast<char>(wideChar);
			return static_cast<int>(len);
		case 2:
			*bytes = static_cast<char>(0b11000000 | ((wideChar >> 6) & 0b00011111));
			*(bytes+1) = static_cast<char>(0b10000000 | (wideChar & 0b00111111));

			return 2;
		case 3:
			*bytes = static_cast<char>(0b11100000 | ((wideChar >> 12) & 0b1111));
			*(bytes+1) = static_cast<char>(0b10000000 | ((wideChar >> 6) & 0b111111));
			*(bytes+2) = static_cast<char>(0b10000000 | (wideChar & 0b111111));

			return 3;
		case 4:
			*bytes = static_cast<char>(0b11110000 | ((wideChar >> 18) & 0b111));
			*(bytes+1) = static_cast<char>(0b10000000 | ((wideChar >> 12) & 0b111111));
			*(bytes+2) = static_cast<char>(0b10000000 | ((wideChar >> 6) & 0b111111));
			*(bytes+3) = static_cast<char>(0b10000000 | (wideChar & 0b111111));

			return 4;
		default:
			Panic::fire(Panic::INVALID_ARGUMENT, "Invalid UTF-8 character length");
	}
}

}
