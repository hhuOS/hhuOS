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

#ifndef __ctype_include__
#define __ctype_include__

/**
 * Tests, if a given character is a letter or a digit.
 *
 * @param c The character to be tested.
 *
 * @return true, if c is a letter or a digit;
 *         false, if not.
 */
extern int isalnum(int c);

/**
 * Tests, if a given character is a letter.
 *
 * @param c The character to be tested.
 *
 * @return true, if c is a letter;
 *         false, if not.
 */
extern int isalpha(int c);

/**
 * Tests, if a given character is a control-character.
 *
 * @param c The character to be tested.
 *
 * @return true, if c is a control-character;
 *         false, if not.
 */
extern int iscntrl(int c);

/**
 * Tests, if a given character is a digit.
 *
 * @param c The character to be tested.
 *
 * @return true, if c is a digit;
 *         false, if not.
 */
extern int isdigit(int c);

/**
 * Tests, if a given character can be printed (except space).
 *
 * @param c The character to be tested.
 *
 * @return true, if c can be printed;
 *         false, if not.
 */
extern int isgraph(int c);

/**
 * Tests, if a given character is a lower-case letter.
 *
 * @param c The character to be tested.
 *
 * @return true, if c is a lower-case letter;
 *         false, if not.
 */
extern int islower(int c);

/**
 * Tests, if a given character can be printed (including space).
 *
 * @param c The character to be tested.
 *
 * @return true, if c can be printed;
 *         false, if not.
 */
extern int isprint(int c);

/**
 * Tests, if a given character is a punctuation character.
 *
 * @param c The character to be tested.
 *
 * @return true, if c is a punctuation character;
 *         false, if not.
 */
extern int ispunct(int c);

/**
 * Tests, if a given character is a white-space character.
 *
 * @param c The character to be tested.
 *
 * @return true, if c is a white-space character;
 *         false, if not.
 */
extern int isspace(int c);

/**
 * Tests, if a given character is an upper-case letter.
 *
 * @param c The character to be tested.
 *
 * @return true, if c is a upper-case letter;
 *         false, if not.
 */
extern int isupper(int c);

/**
 * Tests, if a given character is a hexadecimal-digit.
 *
 * @param c The character to be tested.
 *
 * @return true, if c is a hexadecimal-digit;
 *         false, if not.
 */
extern int isxdigit(int c);

#endif
