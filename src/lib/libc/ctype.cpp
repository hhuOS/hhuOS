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
 * The stanard C library is based on a master's thesis, written by Tobias Fabian Oehme.
 * The original source code can be found here: https://github.com/ToboterXP/hhuOS/tree/thesis
 */

#include "ctype.h"

#include "util/base/CharacterTypes.h"

int isalnum(const int c) {
	return Util::CharacterTypes::isAlphaNumeric(c);
}

int isalpha(const int c) {
	return Util::CharacterTypes::isAlphabet(c);
}

int isdigit(const int c) {
	return Util::CharacterTypes::isDigit(c);
}

int islower(const int c) {
	return Util::CharacterTypes::isLower(c);
}

int isupper(const int c) {
	return Util::CharacterTypes::isUpper(c);
}

int isxdigit(const int c) {
	return Util::CharacterTypes::isHexDigit(c);
}

int iscntrl(const int c) {
	return Util::CharacterTypes::isControl(c);
}

int isgraph(const int c) {
	return Util::CharacterTypes::isGraphical(c);
}

int isspace(const int c) {
	return Util::CharacterTypes::isWhitespace(c);
}

int isblank(const int c) {
	return Util::CharacterTypes::isBlank(c);
}

int isprint(const int c) {
	return Util::CharacterTypes::isPrintable(c);
}

int ispunct(const int c) {
	return Util::CharacterTypes::isPunctuation(c);
}

int tolower(const int c) {
	return isupper(c) ? c + 32 : c;
}

int toupper(const int c) {
	return islower(c) ? c - 32 : c;
}