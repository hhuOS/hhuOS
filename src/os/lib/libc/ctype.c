/*
 * Copyright (C) 2018 Burak Akguel, Christian Gesse, Fabian Ruhland, Filip Krakowski, Michael Schoettner
 * Heinrich-Heine University
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

#include "ctype.h"

int isalnum(int c) {
    return isalpha(c) || isdigit(c);
}

int isalpha(int c) {
    return isupper(c) || islower(c);
}

int iscntrl(int c) {
    return !isprint(c);
}

int isdigit(int c) {
    return c >= 0x30 && c <= 0x39;
}

int isgraph(int c) {
    return c >= 0x21 && c <= 0x7e;
}

int islower(int c) {
    return c >= 0x61 && c <= 0x7a;
}

int isprint(int c) {
    return c >= 0x20 && c <= 0x7e;
}

int ispunct(int c) {
    return isgraph(c) && !isalnum(c);
}

int isspace(int c) {
    return c >= 0x09 && c <= 0x0d;
}

int isupper(int c) {
    return c >= 0x41 && c <= 0x5a;
}

int isxdigit(int c) {
    return isdigit(c) || (c >= 0x41 && c <= 0x46) || (c >= 0x61 && c <= 0x66);
} 
