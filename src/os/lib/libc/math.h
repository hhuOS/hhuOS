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

#ifndef __math_include__
#define __math_include__

/**
 * Calculates the square root of a given number.
 *
 * @param x The number, whose square root shall be calculated.
 *
 * @return The square root of x.
 */
extern double sqrt(double x);

/**
 * Calculates the sinus of a given number.
 *
 * @param x The number, whose sinus shall be calculated.
 *
 * @return The sinus of x.
 */
extern double sin(double x);

/**
 * Calculates the cosinus of a given number.
 *
 * @param x The number, whose cosinus shall be calculated.
 *
 * @return The cosinus of x.
 */
extern double cos(double x);

/**
 * Calculates the tangens of a given number.
 *
 * @param x The number, whose tangens shall be calculated.
 *
 * @return The tangens of x.
 */
extern double tan(double x);

/**
 * Calculates the arctan of a given number.
 *
 * @param x The number, whose arctan shall be calculated.
 *
 * @return The arctan of x.
 */
extern double atan(double x);

/**
 * Calculates the arctan2 of a given number.
 *
 * @param x The x-coordinate.
 * @param y The y-coordinate.
 *
 * @return The arctan2 of x.
 */
extern double atan2(double x, double y);

/**
 * Calculates the exponential function of a given number.
 *
 * @param x The number, whose exponential function shall be calculated.
 *
 * @return The exponential value of x.
 */
extern double exp(double x);

/**
 * Breaks a floating-point number into a normalized fraction and an integral power of 2.
 * Example: x = m * (2 ^ e)
 *          1024 = 0.5 * (2 ^ 11)
 *
 * @param x The number, whose normalized fraction shall be calculated.
 * @param e A pointer to an integer, where the calculated exponent will be saved.
 *
 * @return The normalized fraction of x.
 */
extern double frexp(double x, int *e);

/**
 * Calculates the power function of a given number.
 *
 * @param x The number, that shall be raised.
 * @param y The exponent.
 *
 * @return The value of x raised to the power of y.
 */
extern double pow(double x, double y);

#endif
