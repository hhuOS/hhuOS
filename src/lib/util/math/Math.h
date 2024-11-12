/*
 * Copyright (C) 2018-2024 Heinrich-Heine-Universitaet Duesseldorf,
 * Institute of Computer Science, Department Operating Systems
 * Burak Akguel, Christian Gesse, Fabian Ruhland, Filip Krakowski, Michael Schoettner
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

#ifndef HHUOS_GLMATH_H
#define HHUOS_MATH_H


#include <stdint.h>

namespace Util::Math {

    void endMmx();

    uint32_t absolute(int32_t value);

    uint64_t absolute(int64_t value);

    float absolute(float value);

    double absolute(double value);

    float exp(float arg);
	
	double exp(double arg);

    float exp2(float arg);

    double exp2(double arg);

    float ln(float arg);

	double ln(double arg);

    float log10(float arg);

	double log10(double arg);

    float pow(float base, float exponent);

	double pow(double base, double exponent);

    float sine(float value);

    double sine(double value);

    float cosine(float value);

    double cosine(double value);

    float tangent(float value);

    double tangent(double value);

    float cotangent(float value);

    double cotangent(double value);

    float arctangent(float value, float divisor = 1);

    double arctangent(double value, double divisor = 1);

    float arcsine(float value);

    double arcsine(double value);

    float arccosine(float value);

    double arccosine(double value);

    float sqrt(float value);

    double sqrt(double value);

    double powInt(double value, int exponent);

    float min(float first, float second);

    float max(float first, float second);

    double min(double first, double second);

    double max(double first, double second);

    float min(float first, float second, float third);

    float max(float first, float second, float third);

    double min(double first, double second, double third);

    double max(double first, double second, double third);

    float modulo(float dividend, float divisor);

    double modulo(double dividend, double divisor);

    bool equals(float first, float second, float epsilon);

    bool equals(double first, double second, double epsilon);

    double round(double value);

    float round(float value);

    double truncate(double value);

    float truncate(float value);
	
	double floor(double arg);
	
	float floor(float arg);
	
	double getDoubleInternals(double arg, int * exponent); //returns the mantissa
	
	int isInfinity(double arg);
	
	int isNan(double arg);

    static const constexpr double PI = 3.14159265358979323846;
	static const constexpr double E = 2.718281828459045235360;
}


#endif
