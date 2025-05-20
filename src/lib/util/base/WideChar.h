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

#ifndef HHUOS_LIB_UTIL_WIDECHAR_H
#define HHUOS_LIB_UTIL_WIDECHAR_H

#include <stddef.h>

/// Provides functions to multibyte characters, equivalent to the C standard library functions in <stdlib.h>.
/// We use UTF-8 encoding for multibyte characters.
namespace Util::WideChar {

	/// Calculate the length of a UTF-8 encoded character (equivalent to `mblen()` in C).
	[[nodiscard]] int utf8Length(const char *bytes, size_t maxLength);

	/// Convert a UTF-8 character to a wide character (equivalent to `mbtowc()` in C).
	int utf8ToWchar(wchar_t *wideChar, const char *bytes, size_t maxLength);

	/// Convert a wide character to a UTF-8 character (equivalent to `wctomb()` in C).
	int wcharToUtf8(char * bytes, wchar_t wideChar);

}

#endif