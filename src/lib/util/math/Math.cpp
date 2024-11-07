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

#include "Math.h"
#include <bit>

namespace Util::Math {

void endMmx() {
    asm volatile ("emms");
}

uint32_t absolute(int32_t value) {
    return value < 0 ? value * -1 : value;
}

uint64_t absolute(int64_t value) {
    return value < 0 ? value * -1 : value;
}

float absolute(float value) {
    return value < 0 ? value * -1 : value;
}

double absolute(double value) {
    return value < 0 ? value * -1 : value;
}

float min(float first, float second) {
    return first < second ? first : second;
}

float max(float first, float second) {
    return first > second ? first : second;
}

double min(double first, double second) {
    return first < second ? first : second;
}

double max(double first, double second) {
    return first > second ? first : second;
}

float min(float first, float second, float third) {
    return min(min(first, second), third);
}

float max(float first, float second, float third) {
    return max(max(first, second), third);
}

double min(double first, double second, double third) {
    return min(min(first, second), third);
}

double max(double first, double second, double third) {
    return max(max(first, second), third);
}

float exp(float arg) {
    float ret = 0;
    asm volatile (
            "flds (%0);"
            "fldl2e;"
            "fmulp;" //st0: ex * log2(e)
            "fld1;" //st0: 1, st1: ex * log2(e)
            "fld %%st(1);" // st0: ex * log2(e)  st1: 1, st2: ex * log2(e)
            "fprem;" // st0: rem(ex * log2(e) ), st: 1, st2: ex * log2(e)
            "f2xm1;" //st0: 2 ^ rem(exponent * log2(base)) - 1, s1: 1, s2: ex * log2(e)
            "faddp;" //st0: 2 ^ rem(exponent * log2(base)), st1: ex * log2(e)
            "fscale;" // st0 *= 2^ int (ex * log2(e) )
            "fxch %%st(1);"
            "fstp %%st;" //clear st1
            "fstps (%1);"
            : :
            "r"(&arg),"r"(&ret)
            );

    return ret;
}

double exp(double arg) {
	double ret = 0;
    asm volatile (
			"fldl (%0);"
			"fldl2e;"
			"fmulp;" //st0: ex * log2(e) 
			"fld1;" //st0: 1, st1: ex * log2(e) 
			"fld %%st(1);" // st0: ex * log2(e)  st1: 1, st2: ex * log2(e) 
			"fprem;" // st0: rem(ex * log2(e) ), st: 1, st2: ex * log2(e) 
			"f2xm1;" //st0: 2 ^ rem(exponent * log2(base)) - 1, s1: 1, s2: ex * log2(e) 
			"faddp;" //st0: 2 ^ rem(exponent * log2(base)), st1: ex * log2(e) 
			"fscale;" // st0 *= 2^ int (ex * log2(e) ) 
			"fxch %%st(1);"
			"fstp %%st;" //clear st1
            "fstpl (%1);"
            : :
            "r"(&arg),"r"(&ret)
            );

    return ret;
}

float exp2(float arg) {
    float ret = 0;
    asm volatile (
            "flds (%0);"
            "fld1;"
            "fscale;" // st0 = 2 ^ int(ex), st1 = ex
            "fld1;" // st0 = 1, st1 = 2 ^ int(ex), st2 = ex
            "fld %%st(2);" // st0 = ex, st1 = 1, st2 = 2 ^ int(ex), st3 = ex
            "fprem;" // st0 = rem(ex), st1 = 1, st2 = 2 ^ int(ex), st3 = ex
            "f2xm1;" // st0 = 2 ^ rem(ex) - 1, st1 = 1, st2 = 2 ^ int(ex), st3 = ex
            "faddp;" // st0 = 2 ^ rem(ex), st1 = 2 ^ int(ex), st2 = ex
            "fmulp;" // st0 = 2 ^ ex; st1 = ex
            "fstps (%1);"
            "fstp %%st;" //clear st1
            : :
            "r"(&arg),"r"(&ret)
            );

    return ret;
}

double exp2(double arg) {
    double ret = 0;
    asm volatile (
            "fldl (%0);"
            "fld1;"
            "fscale;" // st0 = 2 ^ int(ex), st1 = ex
            "fld1;" // st0 = 1, st1 = 2 ^ int(ex), st2 = ex
            "fld %%st(2);" // st0 = ex, st1 = 1, st2 = 2 ^ int(ex), st3 = ex
            "fprem;" // st0 = rem(ex), st1 = 1, st2 = 2 ^ int(ex), st3 = ex
            "f2xm1;" // st0 = 2 ^ rem(ex) - 1, st1 = 1, st2 = 2 ^ int(ex), st3 = ex
            "faddp;" // st0 = 2 ^ rem(ex), st1 = 2 ^ int(ex), st2 = ex
            "fmulp;" // st0 = 2 ^ ex; st1 = ex
            "fstpl (%1);"
            "fstp %%st;" //clear st1
            : :
            "r"(&arg),"r"(&ret)
            );

    return ret;
}

float ln(float arg) {
    float ret = 0;
    asm volatile (
            "fld1;"
            "fldl2e;"
            "fdivrp;" //store 1/log2(e) in stl(0)
            "flds (%0);"
            "fyl2x;" //calculate log
            "fstps (%1);"
            : :
            "r"(&arg), "r"(&ret)
            );

    return ret;
}

double ln(double arg) {
	double ret = 0;
    asm volatile (
			"fld1;"
			"fldl2e;"
			"fdivrp;" //store 1/log2(e) in stl(0)
			"fldl (%0);"
			"fyl2x;" //calculate log
            "fstpl (%1);"
            : :
            "r"(&arg), "r"(&ret)
            );

    return ret;
}

float log10(float arg) {
    float ret = 0;
    asm volatile (
            "fld1;"
            "fldl2t;"
            "fdivrp;" //store 1/log2(e) in stl(0)
            "flds (%0);"
            "fyl2x;" //calculate log
            "fstps (%1);"
            : :
            "r"(&arg), "r"(&ret)
            );

    return ret;
}

double log10(double arg) {
	double ret = 0;
    asm volatile (
			"fld1;"
			"fldl2t;"
			"fdivrp;" //store 1/log2(e) in stl(0)
			"fldl (%0);"
			"fyl2x;" //calculate log
            "fstpl (%1);"
            : :
            "r"(&arg), "r"(&ret)
            );

    return ret;
}

float pow(float base, float exponent) {
    float ret = 0;
    asm volatile (
            "flds (%1);"
            "flds (%0);"
            "fyl2x;" //st0: ex * log2(b)
            "fld1;" //st0: 1, st1: ex * log2(b)
            "fld %%st(1);" // st0: ex * log2(b)  st1: 1, st2: ex * log2(b)
            "fprem;" // st0: rem(ex * log2(b) ), st: 1, st2: ex * log2(b)
            "f2xm1;" //st0: 2 ^ rem(exponent * log2(base)) - 1, s1: 1, s2: ex * log2(b)
            "faddp;" //st0: 2 ^ rem(exponent * log2(base)), st1: ex * log2(b)
            "fscale;" // st0 *= 2^ int (ex * log2(b) )
            "fxch %%st(1);"
            "fstp %%st;" //clear st1
            "fstps (%2);"
            : :
            "r"(&base),"r"(&exponent), "r"(&ret)
            );

    return ret;
}

double pow(double base, double exponent) {
	double ret = 0;
    asm volatile (
			"fldl (%1);"
			"fldl (%0);"
			"fyl2x;" //st0: ex * log2(b) 
			"fld1;" //st0: 1, st1: ex * log2(b) 
			"fld %%st(1);" // st0: ex * log2(b)  st1: 1, st2: ex * log2(b) 
			"fprem;" // st0: rem(ex * log2(b) ), st: 1, st2: ex * log2(b) 
			"f2xm1;" //st0: 2 ^ rem(exponent * log2(base)) - 1, s1: 1, s2: ex * log2(b) 
			"faddp;" //st0: 2 ^ rem(exponent * log2(base)), st1: ex * log2(b) 
			"fscale;" // st0 *= 2^ int (ex * log2(b) ) 
			"fxch %%st(1);"
			"fstp %%st;" //clear st1
            "fstpl (%2);"
            : :
            "r"(&base),"r"(&exponent), "r"(&ret)
            );

    return ret;
}

float sine(float value) {
    float ret = 0;
    asm volatile (
            "flds (%0);"
            "fsin;"
            "fstps (%1)"
            : :
            "r"(&value), "r"(&ret)
            );

    return ret;
}

double sine(double value) {
    double ret = 0;
    asm volatile (
            "fldl (%0);"
            "fsin;"
            "fstpl (%1);"
            : :
            "r"(&value), "r"(&ret)
            );

    return ret;
}

float cosine(float value) {
    float ret = 0;
    asm volatile (
            "flds (%0);"
            "fcos;"
            "fstps (%1);"
            : :
            "r"(&value), "r"(&ret)
            );

    return ret;
}

double cosine(double value) {
    double ret = 0;
    asm volatile (
            "fldl (%0);"
            "fcos;"
            "fstpl (%1)"
            : :
            "r"(&value), "r"(&ret)
            );

    return ret;
}

float tangent(float value) {
    float ret = 0;
    asm volatile (
            "flds (%0);"
            "fptan;"
            "fstps (%1);"
            "fstps (%1);" // Pop twice, because fptan pushes the result and 1.0 onto the stack
            : :
            "r"(&value), "r"(&ret)
            );

    return ret;
}

double tangent(double value) {
    double ret = 0;
    asm volatile (
            "fldl (%0);"
            "fptan;"
            "fstpl (%1);"
            "fstpl (%1);" // Pop twice, because fptan pushes the result and 1.0 onto the stack
            : :
            "r"(&value), "r"(&ret)
            );

    return ret;
}

float cotangent(float value) {
    float ret = 0;
    asm volatile (
            "flds (%0);"
            "fptan;"
            "fdivp;" // fptan pushes the result and 1.0 onto the stack
            "fstps (%1);"
            : :
            "r"(&value), "r"(&ret)
            );

    return ret;
}

double cotangent(double value) {
    double ret = 0;
    asm volatile (
            "fldl (%0);"
            "fptan;"
            "fdivp;" // fptan pushes the result and 1.0 onto the stack
            "fstpl (%1);"
            : :
            "r"(&value), "r"(&ret)
            );

    return ret;
}

float arctangent(float value, float divisor) {
    float ret = 0;
    asm volatile (
            "flds (%0);"
            "flds (%1);"
            "fpatan;"
            "fstps (%2);"
            : :
            "r"(&value), "r"(&divisor), "r"(&ret)
            );

    return ret;
}

double arctangent(double value, double divisor) {
    double ret = 0;
    asm volatile (
            "fldl (%0);"
            "fldl (%1);"
            "fpatan;"
            "fstpl (%2);"
            : :
            "r"(&value), "r"(&divisor), "r"(&ret)
            );

    return ret;
}


float sqrt(float value) {
    float ret = 0;
    asm volatile(
            "flds (%0);"
            "fsqrt;"
            "fstps (%1)"
            : :
            "r"(&value), "r"(&ret)
            );

    return ret;
}

double sqrt(double value) {
    double ret = 0;
    asm volatile(
            "fldl (%0);"
            "fsqrt;"
            "fstpl (%1)"
            : :
            "r"(&value), "r"(&ret)
            );

    return ret;
}


float arcsine(float value) {
    if (value > 1 || value < -1) {
        return 0;
    }

    return arctangent(value, sqrt(1 - value * value));
}

double arcsine(double value) {
    if (value > 1 || value < -1) {
        return 0;
    }

    return arctangent(value, sqrt(1 - value * value));
}

float arccosine(float value) {
    return static_cast<float>(PI / 2.0) - arcsine(value);
}

double arccosine(double value) {
    return (PI / 2.0) - arcsine(value);
}

double powInt(double value, int exponent) {
	double ret = 1;
	
	if (exponent < 0) {
		for(int i = 1; i <= -exponent; i++){
			ret /= value;
		}
	} else {
		for(int i = 1; i <= exponent; i++){
			ret *= value;
		}
	}
	
    return ret;
}

float modulo(float dividend, float divisor) {
    return dividend - static_cast<int32_t>(dividend / divisor) * divisor;
}

double modulo(double dividend, double divisor) {
    return dividend - static_cast<int32_t>(dividend / divisor) * divisor;
}

bool equals(float first, float second, float epsilon) {
    return absolute(first - second)  < epsilon;
}

bool equals(double first, double second, double epsilon) {
    return absolute(first - second)  < epsilon;
}

double round(double value) {
    double ret = 0;
    asm volatile(
            "fldl (%0);"
            "frndint;"
            "fstpl (%1)"
            : :
            "r"(&value), "r"(&ret)
            );

    return ret;
}

float round(float value) {
    float ret = 0;
    asm volatile(
            "flds (%0);"
            "frndint;"
            "fstps (%1)"
            : :
            "r"(&value), "r"(&ret)
            );

    return ret;
}

double truncate(double value) {
    return ((double) ((uint64_t) value));
}

float truncate(float value) {
    return ((float) ((uint64_t) value));
}

double floor(double arg) {
	double ret = (double)((long long)arg);
	if (ret > arg) ret--;
	return ret;
}
	
float floor(float arg) {
	float ret = (float)((long long)arg);
    if (ret > arg) ret--;
    return ret;
}

double getDoubleInternals(double arg, int * exponent) {
	long long argb = std::bit_cast<long long>(arg);
	
	*exponent = ((int)((argb >> 52) & ((1<<11)-1))) - 1022; //get exponent bits
	
	argb &= ~(((1LL<<11)-1)<<52); //clear exponent bits
	argb |= (1022LL<<52); // set exponent to -1
	return std::bit_cast<double>(argb);
}

int isInfinity(double arg) {
	int exp;
	arg = getDoubleInternals(arg, &exp);

	return (exp == 1025) && (arg == 0);
}

int isNan(double arg) {
	int exp;
	arg = getDoubleInternals(arg, &exp);
	
	return (exp == 1025) && (arg != 0);
}

}