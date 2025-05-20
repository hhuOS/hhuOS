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

#include "CharacterTypes.h"

namespace Util::CharacterTypes {

bool isAlphaNumeric(const int c) {
    return (c >= 48 && c <= 57) || (c >= 65 && c <= 90) || (c >= 97 && c <= 122);
}

bool isAlphabet(const int c) {
    return (c >= 65 && c <= 90) || (c >= 97 && c <= 122);
}

bool isLower(const int c) {
    return c >= 97 && c <= 122;
}

bool isUpper(const int c) {
    return c >= 65 && c <= 90;
}

bool isDigit(const int c) {
    return c >= 48 && c <= 57;
}

bool isHexDigit(const int c) {
    return (c >= 48 && c <= 57) || (c >= 65 && c <= 70) || (c >= 97 && c <= 102);
}

bool isControl(const int c) {
    return (c >= 0 && c <= 31) || c == 127;
}

bool isGraphical(const int c) {
    return c >= 33 && c <= 126;
}

bool isWhitespace(const int c) {
    return (c >= 9 && c <= 13) || c == 32;
}

bool isBlank(const int c) {
    return c == 9 || c == 32;
}

bool isPrintable(const int c) {
    return c >= 32 && c <= 126;
}

bool isPunctuation(const int c) {
    return (c >= 33 && c <= 47) || (c >= 58 && c <= 64) || (c >= 91 && c <= 96) || (c >= 123 && c <= 126);
}

}
