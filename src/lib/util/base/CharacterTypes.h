/*
 * Copyright (C) 2017-2026 Heinrich Heine University Düsseldorf,
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

#ifndef HHUOS_LIB_UTIL_CHARACTERTYPES_H
#define HHUOS_LIB_UTIL_CHARACTERTYPES_H

namespace Util {

/// Provides functions to check the type of a character, equivalent to the C standard library functions in <ctype.h>.
namespace CharacterTypes {

/// Check if the given character is alphanumeric (a-z, A-Z, 0-9).
inline bool isAlphaNumeric(const int c) {
    return (c >= 48 && c <= 57) || (c >= 65 && c <= 90) || (c >= 97 && c <= 122);
}

/// Check if the given character is an alphabetic character (a-z, A-Z).
inline bool isAlphabet(const int c) {
    return (c >= 65 && c <= 90) || (c >= 97 && c <= 122);
}

/// Check if the given character is a lowercase letter (a-z).
inline bool isLower(const int c) {
    return c >= 97 && c <= 122;
}

/// Check if the given character is an uppercase letter (A-Z).
inline bool isUpper(const int c) {
    return c >= 65 && c <= 90;
}

/// Check if the given character is a digit (0-9).
inline bool isDigit(const int c) {
    return c >= 48 && c <= 57;
}

/// Check if the given character is a hexadecimal digit (0-9, A-F, a-f).
inline bool isHexDigit(const int c) {
    return (c >= 48 && c <= 57) || (c >= 65 && c <= 70) || (c >= 97 && c <= 102);
}

/// Check if the given character is a control character (0-31, 127).
inline bool isControl(const int c) {
    return (c >= 0 && c <= 31) || c == 127;
}

/// Check if the given character has a graphical representation (33-126).
inline bool isGraphical(const int c) {
    return c >= 33 && c <= 126;
}

/// Check if the given character is a whitespace character (9-13, 32).
inline bool isWhitespace(const int c) {
    return (c >= 9 && c <= 13) || c == 32;
}

/// Check if the given character is a blank character (9, 32).
inline bool isBlank(const int c) {
    return c == 9 || c == 32;
}

/// Check if the given character is a printable character (32-126).
inline bool isPrintable(const int c) {
    return c >= 32 && c <= 126;
}

/// Check if the given character is a punctuation character (33-47, 58-64, 91-96, 123-126).
inline bool isPunctuation(const int c) {
    return (c >= 33 && c <= 47) || (c >= 58 && c <= 64) || (c >= 91 && c <= 96) || (c >= 123 && c <= 126);
}

}
}


#endif