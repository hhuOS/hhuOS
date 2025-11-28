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

#ifndef HHUOS_LIB_UTIL_MATH_H
#define HHUOS_LIB_UTIL_MATH_H

#include <stdint.h>

#include "util/collection/Pair.h"

/// Contains several mathematical functions and constants.
/// Most floating point functions are implemented using x87 FPU instructions with inline assembly.
namespace Util::Math {

/// Calculate the absolute value (the value without its sign) of a 32-bit integer.
///
/// ### Example
/// ```c++
/// const uint32_t value1 = 42;
/// const uint32_t value2 = -42;
///
/// const auto absolute1 = Util::Math::absolute(value1); // 42
/// const auto absolute2 = Util::Math::absolute(value2); // 42
/// ```
uint32_t absolute(int32_t value);

/// Calculate the absolute value (the value without its sign) of a 64-bit integer.
///
/// ### Example
/// ```c++
/// const uint64_t value1 = 42;
/// const uint64_t value2 = -42;
///
/// const auto absolute1 = Util::Math::absolute(value1); // 42
/// const auto absolute2 = Util::Math::absolute(value2); // 42
/// ```
uint64_t absolute(int64_t value);

/// Calculate the absolute value (the value without its sign) of a single precision float.
///
/// ### Example
/// ```c++
/// const float value1 = 42.0f;
/// const float value2 = -42.0f;
///
/// const auto absolute1 = Util::Math::absolute(value1); // 42.0f
/// const auto absolute2 = Util::Math::absolute(value2); // 42.0f
/// ```
float absolute(float value);

/// Calculate the absolute value (the value without its sign) of a double precision float.
///
/// ### Example
/// ```c++
/// const double value1 = 42.0;
/// const double value2 = -42.0;
///
/// const auto absolute1 = Util::Math::absolute(value1); // 42.0
/// const auto absolute2 = Util::Math::absolute(value2); // 42.0
/// ```
double absolute(double value);

/// Calculate the minimum of two single precision floats.
///
/// ### Example
/// ```c++
/// const float min1 = Util::Math::min(3.14f, 2.71f); // 2.71f
/// const float min2 = Util::Math::min(1.0f, 1.0f); // 1.0f
/// ```
float min(float first, float second);

/// Calculate the maximum of two single precision floats.
///
/// ### Example
/// ```c++
/// const float max1 = Util::Math::max(3.14f, 2.71f); // 3.14f
/// const float max2 = Util::Math::max(1.0f, 1.0f); // 1.0f
/// ```
float max(float first, float second);

/// Calculate the minimum of two double precision floats.
///
/// ### Example
/// ```c++
/// const double min1 = Util::Math::min(3.14, 2.71); // 2.71
/// const double min2 = Util::Math::min(1.0, 1.0); // 1.0
/// ```
double min(double first, double second);

/// Calculate the maximum of two double precision floats.
///
/// ### Example
/// ```c++
/// const double max1 = Util::Math::max(3.14, 2.71); // 3.14
/// const double max2 = Util::Math::max(1.0, 1.0); // 1.0
/// ```
double max(double first, double second);

/// Calculate the minimum of three single precision floats.
///
/// ### Example
/// ```c++
/// const float min1 = Util::Math::min(3.14f, 2.71f, 1.41f); // 1.41f
/// const float min2 = Util::Math::min(1.0f, 1.0f, 1.0f); // 1.0f
/// ```
float min(float first, float second, float third);

/// Calculate the maximum of three single precision floats.
///
/// ### Example
/// ```c++
/// const float max1 = Util::Math::max(3.14f, 2.71f, 1.41f); // 3.14f
/// const float max2 = Util::Math::max(1.0f, 1.0f, 1.0f); // 1.0f
/// ```
float max(float first, float second, float third);

/// Calculate the minimum of three double precision floats.
///
/// ### Example
/// ```c++
/// const double min1 = Util::Math::min(3.14, 2.71, 1.41); // 1.41
/// const double min2 = Util::Math::min(1.0, 1.0, 1.0); // 1.0
/// ```
double min(double first, double second, double third);

/// Calculate the maximum of three double precision floats.
///
/// ### Example
/// ```c++
/// const double max1 = Util::Math::max(3.14, 2.71, 1.41); // 3.14
/// const double max2 = Util::Math::max(1.0, 1.0, 1.0); // 1.0
/// ```
double max(double first, double second, double third);

/// Check if two single precision floats are equal within a given epsilon.
/// If the absolute difference between the two values is less than the epsilon, they are considered equal.
///
/// ### Example
/// ```c++
/// const float equals1 = Util::Math::equals(3.14f, 3.149f, 0.01f); // true
/// const float equals2 = Util::Math::equals(3.14f, 3.15f, 0.01f); // false
/// ```
bool equals(float first, float second, float epsilon);

/// Check if two double precision floats are equal within a given epsilon.
/// If the absolute difference between the two values is less than the epsilon, they are considered equal.
///
/// ### Example
/// ```c++
/// const double equals1 = Util::Math::equals(3.14, 3.149, 0.01); // true
/// const double equals2 = Util::Math::equals(3.14, 3.15, 0.01); // false
/// ```
bool equals(double first, double second, double epsilon);

/// Calculate the modulo of two single precision floats.
/// This works with negative dividends and divisors as well.
///
/// ### Example
/// ```c++
/// const float mod1 = Util::Math::modulo(5.0f, 2.0f); // 1.0f
/// const float mod2 = Util::Math::modulo(-7.0f, 3.0f); // 2.0f
/// const float mod3 = Util::Math::modulo(8.0f, -3.0f); // -1.0f
/// const float mod4 = Util::Math::modulo(-8.0f, -3.0f); // -2.0f
/// ```
float modulo(float dividend, float divisor);

/// Calculate the modulo of two double precision floats.
/// This works with negative dividends and divisors as well.
///
/// ### Example
/// ```c++
/// const double mod1 = Util::Math::modulo(5.0, 2.0); // 1.0
/// const double mod2 = Util::Math::modulo(-7.0, 3.0); // 2.0
/// const double mod3 = Util::Math::modulo(8.0, -3.0); // -1.0
/// const double mod4 = Util::Math::modulo(-8.0, -3.0); // -2.0
/// ```
double modulo(double dividend, double divisor);

/// Round a single precision float to the nearest integer value.
///
/// ### Example
/// ```c++
/// const float rounded1 = Util::Math::round(3.14f); // 3.0f
/// const float rounded2 = Util::Math::round(3.5f); // 4.0f
/// const float rounded3 = Util::Math::round(-2.7f); // -3.0f
/// const float rounded4 = Util::Math::round(-2.5f); // -2.0f
/// ```
float round(float value);

/// Round a double precision float to the nearest integer value.
///
/// ### Example
/// ```c++
/// const double rounded1 = Util::Math::round(3.14); // 3.0
/// const double rounded2 = Util::Math::round(3.5); // 4.0
/// const double rounded3 = Util::Math::round(-2.7); // -3.0
/// const double rounded4 = Util::Math::round(-2.5); // -2.0
/// ```
double round(double value);

/// Truncate a single precision float (round towards zero).
///
/// ### Example
/// ```c++
/// const float truncated1 = Util::Math::truncate(3.14f); // 3.0f
/// const float truncated2 = Util::Math::truncate(3.5f); // 3.0f
/// const float truncated3 = Util::Math::truncate(-2.7f); // -2.0f
/// const float truncated4 = Util::Math::truncate(-2.5f); // -2.0f
/// ```
float truncate(float value);

/// Truncate a double precision float, rounding to the nearest integer value towards zero.
///
/// ### Example
/// ```c++
/// const double truncated1 = Util::Math::truncate(3.14); // 3.0
/// const double truncated2 = Util::Math::truncate(3.5); // 3.0
/// const double truncated3 = Util::Math::truncate(-2.7); // -2.0
/// const double truncated4 = Util::Math::truncate(-2.5); // -2.0
/// ```
double truncate(double value);

/// Round a single precision float down to the nearest integer value.
///
/// ### Example
/// ```c++
/// const float floored1 = Util::Math::floor(3.14f); // 3.0f
/// const float floored2 = Util::Math::floor(3.5f); // 3.0f
/// const float floored3 = Util::Math::floor(-2.7f); // -3.0f
/// const float floored4 = Util::Math::floor(-2.5f); // -3.0f
/// ```
float floor(float value);

/// Round a double precision float down to the nearest integer value.
///
/// ### Example
/// ```c++
/// const double floored1 = Util::Math::floor(3.14); // 3.0
/// const double floored2 = Util::Math::floor(3.5); // 3.0
/// const double floored3 = Util::Math::floor(-2.7); // -3.0
/// const double floored4 = Util::Math::floor(-2.5); // -3.0
/// ```
double floor(double value);

/// Round a single precision float up to the nearest integer value.
///
/// ### Example
/// ```c++
/// const float ceiled1 = Util::Math::ceil(3.14f); // 4.0f
/// const float ceiled2 = Util::Math::ceil(3.5f); // 4.0f
/// const float ceiled3 = Util::Math::ceil(-2.7f); // -2.0f
/// const float ceiled4 = Util::Math::ceil(-2.5f); // -2.0f
/// ```
float ceil(float value);

/// Round a double precision float up to the nearest integer value.
///
/// ### Example
/// ```c++
/// const double ceiled1 = Util::Math::ceil(3.14); // 4.0
/// const double ceiled2 = Util::Math::ceil(3.5); // 4.0
/// const double ceiled3 = Util::Math::ceil(-2.7); // -2.0
/// const double ceiled4 = Util::Math::ceil(-2.5); // -2.0
/// ```
double ceil(double value);

/// Calculate the exponential function (e^x) for a single precision float.
///
/// ### Example
/// ```c++
/// const float exp1 = Util::Math::exp(1.0f); // 2.718281828459045f
/// const float exp2 = Util::Math::exp(0.0f); // 1.0f
/// const float exp3 = Util::Math::exp(-1.0f); // 0.36787944117144233f
/// const float exp4 = Util::Math::exp(10.0f); // 22026.465794806718f
/// ```
float exp(float value);

/// Calculate the exponential function (e^x) for a double precision float.
///
/// ### Example
/// ```c++
/// const double exp1 = Util::Math::exp(1.0); // 2.718281828459045
/// const double exp2 = Util::Math::exp(0.0); // 1.0
/// const double exp3 = Util::Math::exp(-1.0); // 0.36787944117144233
/// const double exp4 = Util::Math::exp(10.0); // 22026.465794806718
/// ```
double exp(double value);

/// Calculate the base-2 exponential function (2^x) for a single precision float.
///
/// ### Example
/// ```c++
/// const float exp1 = Util::Math::exp2(1.0f); // 2.0f
/// const float exp2 = Util::Math::exp2(0.0f); // 1.0f
/// const float exp3 = Util::Math::exp2(-1.0f); // 0.5f
/// const float exp4 = Util::Math::exp2(10.0f); // 1024.0f
/// ```
float exp2(float value);

/// Calculate the base-2 exponential function (2^x) for a double precision float.
///
/// ###
/// ```c++
/// const double exp1 = Util::Math::exp2(1.0); // 2.0
/// const double exp2 = Util::Math::exp2(0.0); // 1.0
/// const double exp3 = Util::Math::exp2(-1.0); // 0.5
/// const double exp4 = Util::Math::exp2(10.0); // 1024.0
/// ```
double exp2(double value);

/// Calculate the natural logarithm (base e) of a single precision float.
/// The logarithm is only defined for positive values. If the value is less than or equal to zero, a panic is fired.
///
/// ### Example
/// ```c++
/// const float ln1 = Util::Math::ln(Util::Math::E_FLOAT); // 1.0f
/// const float ln2 = Util::Math::ln(1.0f); // 0.0f
/// const float ln3 = Util::Math::ln(10.0f); // 2.302585092994046f
/// const float ln4 = Util::Math::ln(-1.0f); // Panic: Logarithm of non-positive value
/// ```
float ln(float value);

/// Calculate the natural logarithm (base e) of a double precision float.
/// The logarithm is only defined for positive values. If the value is less than or equal to zero, a panic is fired.
///
/// ### Example
/// ```c++
/// const double ln1 = Util::Math::ln(Util::Math::E_DOUBLE); // 1.0
/// const double ln2 = Util::Math::ln(1.0); // 0.0
/// const double ln3 = Util::Math::ln(10.0); // 2.302585092994046
/// const double ln4 = Util::Math::ln(-1.0); // Panic: Logarithm of non-positive value
/// ```
double ln(double value);

/// Calculate the base-10 logarithm of a single precision float.
/// The logarithm is only defined for positive values. If the value is less than or equal to zero, a panic is fired.
///
/// ### Example
/// ```c++
/// const float log1 = Util::Math::log10(10.0f); // 1.0f
/// const float log2 = Util::Math::log10(100.0f); // 2.0f
/// const float log3 = Util::Math::log10(1.0f); // 0.0f
/// const float log4 = Util::Math::log10(-1.0f); // Panic: Logarithm of non-positive value
/// ```
float log10(float value);

/// Calculate the base-10 logarithm of a double precision float.
/// The logarithm is only defined for positive values. If the value is less than or equal to zero, a panic is fired.
///
/// ### Example
/// ```c++
/// const double log1 = Util::Math::log10(10.0); // 1.0
/// const double log2 = Util::Math::log10(100.0); // 2.0
/// const double log3 = Util::Math::log10(1.0); // 0.0
/// const double log4 = Util::Math::log10(-1.0); // Panic: Logarithm of non-positive value
/// ```
double log10(double value);

/// Calculate the power of a single precision float raised to an exponent.
/// If the base is negative, the exponent must be an integer. Otherwise, a panic is fired.
///
/// ### Example
/// ```c++
/// const float pow1 = Util::Math::pow(2.0f, 3.0f); // 8.0f
/// const float pow2 = Util::Math::pow(3.0f, 2.0f); // 9.0f
/// const float pow3 = Util::Math::pow(-2.0f, 3.0f); // -8.0f
/// const float pow3 = Util::Math::pow(-2.0f, 2.0f); // 4.0f
/// const float pow4 = Util::Math::pow(2.0f, -2.0f); // 0.25f
/// const float pow6 = Util::Math::pow(-2.0f, 2.5f); // Panic: Negative base with non-integer exponent
/// ```
float pow(float base, float exponent);

/// Calculate the power of a double precision float raised to an exponent.
/// If the base is negative, the exponent must be an integer. Otherwise, a panic is fired.
///
/// ###
/// ```c++
/// const double pow1 = Util::Math::pow(2.0, 3.0); // 8.0
/// const double pow2 = Util::Math::pow(3.0, 2.0); // 9.0
/// const double pow3 = Util::Math::pow(-2.0, 3.0); // -8.0
/// const double pow4 = Util::Math::pow(-2.0, 2.0); // 4.0
/// const double pow5 = Util::Math::pow(2.0, -2.0); // 0.25
/// const double pow6 = Util::Math::pow(-2.0, 2.5); // Panic: Negative base with non-integer exponent
/// ```
double pow(double base, double exponent);

/// Calculate the square root of a single precision float.
/// The value must be non-negative. If the value is negative, a panic is fired.
///
/// ### Example
/// ```c++
/// const float sqrt1 = Util::Math::sqrt(4.0f); // 2.0f
/// const float sqrt2 = Util::Math::sqrt(9.0f); // 3.0f
/// const float sqrt3 = Util::Math::sqrt(0.0f); // 0.0f
/// const float sqrt4 = Util::Math::sqrt(-1.0f); // Panic: Negative value
/// ```
float sqrt(float value);

/// Calculate the square root of a double precision float.
/// The value must be non-negative. If the value is negative, a panic is fired.
///
/// ### Example
/// ```c++
/// const double sqrt1 = Util::Math::sqrt(4.0); // 2.0
/// const double sqrt2 = Util::Math::sqrt(9.0); // 3.0
/// const double sqrt3 = Util::Math::sqrt(0.0); // 0.0
/// const double sqrt4 = Util::Math::sqrt(-1.0); // Panic: Negative value
/// ```
double sqrt(double value);

/// Convert degrees to radians for a single precision float.
///
/// ### Example
/// ```c++
/// const float radians1 = Util::Math::toRadians(180.0f); // 3.141592653589793f (Util::Math::PI_FLOAT)
/// const float radians2 = Util::Math::toRadians(90.0f); // 1.5707963267948966f (Util::Math::PI_FLOAT / 2.0f)
/// const float radians3 = Util::Math::toRadians(0.0f); // 0.0f
/// const float radians4 = Util::Math::toRadians(-180.0f); // -3.141592653589793f (-Util::Math::PI_FLOAT)
/// const float radians5 = Util::Math::toRadians(360.0f); // 6.283185307179586f (Util::Math::PI_FLOAT * 2)
/// const float radians6 = Util::Math::toRadians(-360.0f); // -6.283185307179586f (-Util::Math::PI_FLOAT * 2)
/// ```
float toRadians(float degrees);

/// Convert degrees to radians for a double precision float.
///
/// ### Example
/// ```c++
/// const double radians1 = Util::Math::toRadians(180.0); // 3.141592653589793 (Util::Math::PI_DOUBLE)
/// const double radians2 = Util::Math::toRadians(90.0); // 1.5707963267948966 (Util::Math::PI_DOUBLE / 2.0)
/// const double radians3 = Util::Math::toRadians(0.0); // 0.0
/// const double radians4 = Util::Math::toRadians(-180.0); // -3.141592653589793 (-Util::Math::PI_DOUBLE)
/// const double radians5 = Util::Math::toRadians(360.0); // 6.283185307179586 (Util::Math::PI_DOUBLE * 2)
/// const double radians6 = Util::Math::toRadians(-360.0); // -6.283185307179586 (-Util::Math::PI_DOUBLE * 2)
/// ```
double toRadians(double degrees);

/// Convert radians to degrees for a single precision float.
///
/// ### Example
/// ```c++
/// const float degrees1 = Util::Math::toDegrees(Util::Math::PI_FLOAT); // 180.0f
/// const float degrees2 = Util::Math::toDegrees(Util::Math::PI_FLOAT / 2.0f); // 90.0f
/// const float degrees3 = Util::Math::toDegrees(0.0f); // 0.0f
/// const float degrees4 = Util::Math::toDegrees(-Util::Math::PI_FLOAT); // -180.0f
/// const float degrees5 = Util::Math::toDegrees(Util::Math::PI_FLOAT * 2.0f); // 360.0f
/// const float degrees6 = Util::Math::toDegrees(-Util::Math::PI_FLOAT * 2.0f); // -360.0f
/// ```
float toDegrees(float radians);

/// Convert radians to degrees for a double precision float.
///
/// ### Example
/// ```c++
/// const double degrees1 = Util::Math::toDegrees(Util::Math::PI_DOUBLE); // 180.0
/// const double degrees2 = Util::Math::toDegrees(Util::Math::PI_DOUBLE / 2.0); // 90.0
/// const double degrees3 = Util::Math::toDegrees(0.0); // 0.0
/// const double degrees4 = Util::Math::toDegrees(-Util::Math::PI_DOUBLE); // -180.0
/// const double degrees5 = Util::Math::toDegrees(Util::Math::PI_DOUBLE * 2.0); // 360.0
/// const double degrees6 = Util::Math::toDegrees(-Util::Math::PI_DOUBLE * 2.0); // -360.0
/// ```
double toDegrees(double radians);

#define SINE(VALUE, RESULT) \
    asm volatile ( \
    "fsin;" \
    : "=t"(RESULT) \
    : "0"(VALUE) \
    );

/// Calculate the sine of a single precision float.
/// The value must be given in radians.
///
/// ### Example
/// ```c++
/// const float sine1 = Util::Math::sine(0.0f); // 0.0f
/// const float sine2 = Util::Math::sine(Util::Math::PI_FLOAT / 2.0f); // 1.0f
/// const float sine3 = Util::Math::sine(Util::Math::PI_FLOAT); // 0.0f
/// const float sine4 = Util::Math::sine(Util::Math::PI_FLOAT * 3.0f / 2.0f); // -1.0f
/// const float sine5 = Util::Math::sine(Util::Math::PI_FLOAT * 2.0f); // 0.0f
/// ```
inline float sine(const float value) {
    float result;
    SINE(value, result);

    return result;
}

/// Calculate the sine of a double precision float.
/// The value must be given in radians.
///
/// ### Example
/// ```c++
/// const double sine1 = Util::Math::sine(0.0); // 0.0
/// const double sine2 = Util::Math::sine(Util::Math::PI_DOUBLE / 2.0); // 1.0
/// const double sine3 = Util::Math::sine(Util::Math::PI_DOUBLE); // 0.0
/// const double sine4 = Util::Math::sine(Util::Math::PI_DOUBLE * 3.0 / 2.0); // -1.0
/// const double sine5 = Util::Math::sine(Util::Math::PI_DOUBLE * 2.0); // 0.0
/// ```
inline double sine(const double value) {
    double result;
    SINE(value, result);

    return result;
}

#define COSINE(VALUE, RESULT) \
    asm volatile ( \
    "fcos;" \
    : "=t"(RESULT) \
    : "0"(VALUE) \
    );

/// Calculate the cosine of a single precision float.
/// The value must be given in radians.
///
/// ### Example
/// ```c++
/// const float cosine1 = Util::Math::cosine(0.0f); // 1.0f
/// const float cosine2 = Util::Math::cosine(Util::Math::PI_FLOAT / 2.0f); // 0.0f
/// const float cosine3 = Util::Math::cosine(Util::Math::PI_FLOAT); // -1.0f
/// const float cosine4 = Util::Math::cosine(Util::Math::PI_FLOAT * 3.0f / 2.0f); // 0.0f
/// const float cosine5 = Util::Math::cosine(Util::Math::PI_FLOAT * 2.0f); // 1.0f
/// ```
inline float cosine(const float value) {
    float result;
    COSINE(value, result);

    return result;
}

/// Calculate the cosine of a double precision float.
/// The value must be given in radians.
///
/// ### Example
/// ```c++
/// const double cosine1 = Util::Math::cosine(0.0); // 1.0
/// const double cosine2 = Util::Math::cosine(Util::Math::PI_DOUBLE / 2.0); // 0.0
/// const double cosine3 = Util::Math::cosine(Util::Math::PI_DOUBLE); // -1.0
/// const double cosine4 = Util::Math::cosine(Util::Math::PI_DOUBLE * 3.0 / 2.0); // 0.0
/// const double cosine5 = Util::Math::cosine(Util::Math::PI_DOUBLE * 2.0); // 1.0
/// ```
inline double cosine(const double value) {
    double result;
    COSINE(value, result);

    return result;
}

/// Calculate the tangent of a single precision float.
/// The value must be given in radians.
///
/// ### Example
/// ```c++
/// const float tangent1 = Util::Math::tangent(0.0f); // 0.0f
/// const float tangent2 = Util::Math::tangent(Util::Math::PI_FLOAT / 4.0f); // 1.0f
/// const float tangent3 = Util::Math::tangent(Util::Math::PI_FLOAT / 2.0f); // Infinity
/// const float tangent4 = Util::Math::tangent(Util::Math::PI_FLOAT * 3.0f / 4.0f); // -1.0f
/// const float tangent5 = Util::Math::tangent(Util::Math::PI_FLOAT); // 0.0f
/// ```
float tangent(float value);

/// Calculate the tangent of a double precision float.
/// The value must be given in radians.
///
/// ### Example
/// ```c++
/// const double tangent1 = Util::Math::tangent(0.0); // 0.0
/// const double tangent2 = Util::Math::tangent(Util::Math::PI_DOUBLE / 4.0); // 1.0
/// const double tangent3 = Util::Math::tangent(Util::Math::PI_DOUBLE / 2.0); // Infinity
/// const double tangent4 = Util::Math::tangent(Util::Math::PI_DOUBLE * 3.0 / 4.0); // -1.0
/// const double tangent5 = Util::Math::tangent(Util::Math::PI_DOUBLE); // 0.0
/// ```
double tangent(double value);

/// Calculate the cotangent of a single precision float.
/// The value must be given in radians.
///
/// ### Example
/// ```c++
/// const float cotangent1 = Util::Math::cotangent(0.0f); // Infinity
/// const float cotangent2 = Util::Math::cotangent(Util::Math::PI_FLOAT / 4.0f); // 1.0f
/// const float cotangent3 = Util::Math::cotangent(Util::Math::PI_FLOAT / 2.0f); // 0.0f
/// const float cotangent4 = Util::Math::cotangent(Util::Math::PI_FLOAT * 3.0f / 4.0f); // -1.0f
/// const float cotangent5 = Util::Math::cotangent(Util::Math::PI_FLOAT); // Infinity
/// ```
float cotangent(float value);

/// Calculate the cotangent of a double precision float.
/// The value must be given in radians.
///
/// ### Example
/// ```c++
/// const double cotangent1 = Util::Math::cotangent(0.0); // Infinity
/// const double cotangent2 = Util::Math::cotangent(Util::Math::PI_DOUBLE / 4.0); // 1.0
/// const double cotangent3 = Util::Math::cotangent(Util::Math::PI_DOUBLE / 2.0); // 0.0
/// const double cotangent4 = Util::Math::cotangent(Util::Math::PI_DOUBLE * 3.0 / 4.0); // -1.0
/// const double cotangent5 = Util::Math::cotangent(Util::Math::PI_DOUBLE); // Infinity
/// ```
double cotangent(double value);

/// Calculate the arctangent of a single precision float.
/// The value must be given in radians.
/// An optional divisor can be provided to calculate the arctangent of (value / divisor).
/// This is done, because the x87 FPU instruction `fpatan` expects a divisor.
/// The default divisor is 1, which means that the function calculates the arctangent of the value itself.
///
/// ### Example
/// ```c++
/// const float atan1 = Util::Math::arctangent(1.0f); // 0.7853981633974483f (PI_FLOAT / 4.0f)
/// const float atan2 = Util::Math::arctangent(0.0f); // 0.0f
/// const float atan3 = Util::Math::arctangent(-1.0f); // -0.7853981633974483f (-PI_FLOAT / 4.0f)
/// const float atan4 = Util::Math::arctangent(1.0f, 2.0f); // 0.4636476090008061f
/// ```
float arctangent(float value, float divisor = 1);

/// Calculate the arctangent of a double precision float.
/// The value must be given in radians.
/// An optional divisor can be provided to calculate the arctangent of (value / divisor).
/// This is done, because the x87 FPU instruction `fpatan` expects a divisor.
/// The default divisor is 1, which means that the function calculates the arctangent of the value itself.
///
/// ### Example
/// ```c++
/// const double atan1 = Util::Math::arctangent(1.0); // 0.7853981633974483 (PI_DOUBLE / 4.0)
/// const double atan2 = Util::Math::arctangent(0.0); // 0.0
/// const double atan3 = Util::Math::arctangent(-1.0); // -0.7853981633974483 (-PI_DOUBLE / 4.0)
/// const double atan4 = Util::Math::arctangent(1.0, 2.0); // 0.4636476090008061
/// ```
double arctangent(double value, double divisor = 1);

/// Calculate the arctangent2 of two single precision floats.
/// The function calculates the arctangent of (y / x) using the signs of both values to determine the correct quadrant.
///
/// ### Example
/// ```c++
/// const float atan1 = Util::Math::arctangent2(1.0f, 1.0f); // 0.7853981633974483f (PI_FLOAT / 4.0f)
/// const float atan2 = Util::Math::arctangent2(0.0f, 1.0f); // 0.0f
/// const float atan3 = Util::Math::arctangent2(-1.0f, 1.0f); // -0.7853981633974483f (-PI_FLOAT / 4.0f)
/// const float atan4 = Util::Math::arctangent2(1.0f, -1.0f); // 2.356194490192345f (3 * PI_FLOAT / 4.0f)
/// const float atan5 = Util::Math::arctangent2(-1.0f, -1.0f); // -2.356194490192345f (-3 * PI_FLOAT / 4.0f)
/// ```
float arctangent2(float y, float x);

/// Calculate the arctangent2 of two double precision floats.
/// The function calculates the arctangent of (y / x) using the signs of both values to determine the correct quadrant.
///
/// ### Example
/// ```c++
/// const double atan1 = Util::Math::arctangent2(1.0, 1.0); // 0.7853981633974483 (PI_DOUBLE / 4.0)
/// const double atan2 = Util::Math::arctangent2(0.0, 1.0); // 0.0
/// const double atan3 = Util::Math::arctangent2(-1.0, 1.0); // -0.7853981633974483 (-PI_DOUBLE / 4.0)
/// const double atan4 = Util::Math::arctangent2(1.0, -1.0); // 2.356194490192345 (3 * PI_DOUBLE / 4.0)
/// const double atan5 = Util::Math::arctangent2(-1.0, -1.0); // -2.356194490192345 (-3 * PI_DOUBLE / 4.0)
/// ```
double arctangent2(double y, double x);

/// Calculate the arcsine of a single precision float.
/// The value must be in the range [-1, 1]. Other values will fire a panic.
///
/// ### Example
/// ```c++
/// const float asin1 = Util::Math::arcsine(0.0f); // 0.0f
/// const float asin2 = Util::Math::arcsine(1.0f); // 1.5707963267948966f (PI_FLOAT / 2.0f)
/// const float asin3 = Util::Math::arcsine(-1.0f); // -1.5707963267948966f (-PI_FLOAT / 2.0f)
/// const float asin4 = Util::Math::arcsine(0.5f); // 0.5235987755982989f (PI_FLOAT / 6.0f)
/// const float asin5 = Util::Math::arcsine(-0.5f); // -0.5235987755982989f (-PI_FLOAT / 6.0f)
/// const float asin5 = Util::Math::arcsine(1.5f); // Panic: Value out of range [-1, 1]
/// ```
float arcsine(float value);

/// Calculate the arcsine of a double precision float.
/// The value must be in the range [-1, 1]. Other values will fire a panic.
///
/// ### Example
/// ```c++
/// const double asin1 = Util::Math::arcsine(0.0); // 0.0
/// const double asin2 = Util::Math::arcsine(1.0); // 1.5707963267948966 (PI_DOUBLE / 2.0)
/// const double asin3 = Util::Math::arcsine(-1.0); // -1.5707963267948966 (-PI_DOUBLE / 2.0)
/// const double asin4 = Util::Math::arcsine(0.5); // 0.5235987755982989 (PI_DOUBLE / 6.0)
/// const double asin5 = Util::Math::arcsine(-0.5); // -0.5235987755982989 (-PI_DOUBLE / 6.0)
/// const double asin6 = Util::Math::arcsine(1.5); // Panic: Value out of range [-1, 1]
/// ```
double arcsine(double value);

/// Calculate the arccosine of a single precision float.
/// The value must be in the range [-1, 1]. Other values will fire a panic.
///
/// ### Example
/// ```c++
/// const float acos1 = Util::Math::arccosine(0.0f); // 1.5707963267948966f (PI_FLOAT / 2.0f)
/// const float acos2 = Util::Math::arccosine(1.0f); // 0.0f
/// const float acos3 = Util::Math::arccosine(-1.0f); // 3.141592653589793f (PI_FLOAT)
/// const float acos4 = Util::Math::arccosine(0.5f); // 1.0471975511965976f (PI_FLOAT / 3.0f)
/// const float acos5 = Util::Math::arccosine(-0.5f); // 2.0943951023931957f (2 * PI_FLOAT / 3.0f)
/// const float acos6 = Util::Math::arccosine(1.5f); // Panic: Value out of range [-1, 1]
/// ```
float arccosine(float value);

/// Calculate the arccosine of a double precision float.
/// The value must be in the range [-1, 1]. Other values will fire a panic.
///
/// ### Example
/// ```c++
/// const double acos1 = Util::Math::arccosine(0.0); // 1.5707963267948966 (PI_DOUBLE / 2.0)
/// const double acos2 = Util::Math::arccosine(1.0); // 0.0
/// const double acos3 = Util::Math::arccosine(-1.0); // 3.141592653589793 (PI_DOUBLE)
/// const double acos4 = Util::Math::arccosine(0.5); // 1.0471975511965976 (PI_DOUBLE / 3.0)
/// const double acos5 = Util::Math::arccosine(-0.5); // 2.0943951023931957 (2 * PI_DOUBLE / 3.0)
/// const double acos6 = Util::Math::arccosine(1.5); // Panic: Value out of range [-1, 1]
/// ```
double arccosine(double value);

/// Disassemble a single precision float into its internal representation (mantissa and exponent).
/// The return value is a pair containing the mantissa as first element and the exponent as second element.
/// The original value can be reconstructed using the formula `value = mantissa * 2^exponent`.
///
/// ### Example
/// ```c++
/// const auto internals = Util::Math::getInternals(3.14f);
/// const auto mantissa = internals.getFirst(); // 0.785000026f
/// const auto exponent = internals.getSecond(); // 2
/// const auto reconstructed = mantissa * Util::Math::pow(2.0f, exponent); // 0.785f * 2^2 = 3.14f
/// ```
Pair<float, int8_t> getInternals(float value);

/// Disassemble a double precision float into its internal representation (mantissa and exponent).
/// The return value is a pair containing the mantissa as first element and the exponent as second element.
/// The original value can be reconstructed using the formula `value = mantissa * 2^exponent`.
///
/// ### Example
/// ```c++
/// const auto internals = Util::Math::getInternals(3.14);
/// const auto mantissa = internals.getFirst(); // 0.78500000000000003
/// const auto exponent = internals.getSecond(); // 2
/// const auto reconstructed = mantissa * Util::Math::pow(2.0, exponent); // 0.785 * 2^2 = 3.14
/// ```
Pair<double, int16_t> getInternals(double value);

/// Check if a single precision float is infinity.
/// This is done by comparing the value 1.0f / 0.0f and -1.0f / 0.0f, which yield positive and negative infinity.
/// If the value is equal to either of these, it is considered infinity.
///
/// ### Example
/// ```c++
/// const bool isInf1 = Util::Math::isInfinity(1.0f / 0.0f); // true
/// const bool isInf2 = Util::Math::isInfinity(-1.0f / 0.0f); // true
/// const bool isInf3 = Util::Math::isInfinity(0.0f / 0.0f); // false
/// const bool isInf4 = Util::Math::isInfinity(0.0f); // false
/// ```
bool isInfinity(float value);

/// Check if a double precision float is infinity.
/// This is done by comparing the value 1.0 / 0.0 and -1.0 / 0.0, which yield positive and negative infinity.
/// If the value is equal to either of these, it is considered infinity.
///
/// ### Example
/// ```c++
/// const bool isInf1 = Util::Math::isInfinity(1.0 / 0.0); // true
/// const bool isInf2 = Util::Math::isInfinity(-1.0 / 0.0); // true
/// const bool isInf3 = Util::Math::isInfinity(0.0 / 0.0); // false
/// const bool isInf4 = Util::Math::isInfinity(0.0); // false
/// ```
bool isInfinity(double value);

/// Check if a single precision float is NaN (Not a Number).
/// This is done by comparing the value to itself (NaN is the only value that is not equal to itself).
///
/// ### Example
/// ```c++
/// const bool isNan1 = Util::Math::isNan(0.0f / 0.0f); // true
/// const bool isNan2 = Util::Math::isNan(1.0f / 0.0f); // false
/// const bool isNan3 = Util::Math::isNan(-1.0f / 0.0f); // false
/// const bool isNan4 = Util::Math::isNan(0.0f); // false
/// ```
bool isNan(float value);

/// Check if a double precision float is NaN (Not a Number).
/// This is done by comparing the value to itself (NaN is the only value that is not equal to itself).
///
/// ### Example
/// ```c++
/// const bool isNan1 = Util::Math::isNan(0.0 / 0.0); // true
/// const bool isNan2 = Util::Math::isNan(1.0 / 0.0); // false
/// const bool isNan3 = Util::Math::isNan(-1.0 / 0.0); // false
/// const bool isNan4 = Util::Math::isNan(0.0); // false
/// ```
bool isNan(double value);

/// The constant PI as a double precision float.
static constexpr double PI_DOUBLE = 3.14159265358979323846;

/// The constant PI as a single precision float.
static constexpr float PI_FLOAT = PI_DOUBLE;

/// The constant E (Euler's number) as a double precision float.
static constexpr double E_DOUBLE = 2.718281828459045235360;

/// The constant E (Euler's number) as a single precision float.
static constexpr float E_FLOAT = E_DOUBLE;

}


#endif
