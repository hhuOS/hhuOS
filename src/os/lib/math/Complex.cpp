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

#include "Complex.h"

extern "C" {
    #include "lib/libc/math.h"
}

Complex::Complex(double real, double imaginary) : real(real), imaginary(imaginary) {

}

Complex::Complex(const Complex &other) : real(other.real), imaginary(other.imaginary) {

}

Complex Complex::operator+(const Complex &other) {

    return Complex(real + other.real, imaginary + other.imaginary);
}

Complex Complex::operator-(const Complex &other) {

    return Complex(real - other.real, imaginary - other.imaginary);
}

Complex Complex::operator*(const Complex &other) {

    return Complex(real * other.real - imaginary * other.imaginary, real * other.imaginary + imaginary * other.real);
}

Complex Complex::operator/(const Complex &other) {

    const double c = other.real;

    const double d = other.imaginary;

    double q;

    double denominator;

    if (abs(c) < abs(d)) {

        q = c / d;

        denominator = c * q  + d;

    } else {

        q = d / c;

        denominator = d * q + c;
    }

    return Complex((imaginary * q + real) / denominator, (imaginary - real * q) / denominator);
}

double Complex::absolute() {

    return sqrt(real * real + imaginary * imaginary);
}

double Complex::squaredAbsolute() {

    return real * real + imaginary * imaginary;
}

Complex &Complex::operator=(const Complex &other) = default;
