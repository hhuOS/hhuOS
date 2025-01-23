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
#include "lib/util/base/Exception.h"
#include <bit>

namespace Util::Math {

void endMmx() {
    asm volatile ( "emms" );
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

#define EXP(VALUE, RESULT) asm volatile ( \
        "fldl2e;" /* st0: log2(e), st1: value*/ \
        "fmulp;" /* st0: ex * log2(e) */ \
        "fld1;" /* st0: 1, st1: ex * log2(e) */ \
        "fld %%st(1);" /* st0: ex * log2(e), st1: 1, st2: ex * log2(e) */ \
        "fprem;" /* st0: rem(ex * log2(e)), st: 1, st2: ex * log2(e) */ \
        "f2xm1;" /* st0: 2 ^ rem(exponent * log2(base)) - 1, s1: 1, s2: ex * log2(e) */ \
        "faddp;" /* st0: 2 ^ rem(exponent * log2(base)), st1: ex * log2(e) */ \
        "fscale;" /* st0 *= 2^ int (ex * log2(e)), st1: ex * log2(e) */ \
        "fstp %%st(1);" /* clear st1 -> result in st0 */ \
        : "=t"(RESULT) \
        : "0"(VALUE)  \
        );

float exp(float value) {
    float result;
    EXP(value, result);

    return result;
}

double exp(double value) {
    float result;
    EXP(value, result);

    return result;
}

#define EXP2(VALUE, RESULT) asm volatile ( \
        "fld1;" /* st0 = 1, st1 = value */ \
        "fscale;" /* st0 = 2 ^ int(ex), st1 = ex */ \
        "fld1;" /* st0 = 1, st1 = 2 ^ int(ex), st2 = ex */ \
        "fld %%st(2);" /* st0 = ex, st1 = 1, st2 = 2 ^ int(ex), st3 = ex */ \
        "fprem;" /* st0 = rem(ex), st1 = 1, st2 = 2 ^ int(ex), st3 = ex */ \
        "f2xm1;" /* st0 = 2 ^ rem(ex) - 1, st1 = 1, st2 = 2 ^ int(ex), st3 = ex */ \
        "faddp;" /* st0 = 2 ^ rem(ex), st1 = 2 ^ int(ex), st2 = ex */ \
        "fmulp;" /* st0 = 2 ^ ex; st1 = ex */ \
        "fstp %%st(1);" /* clear st1 -> result in st0 */ \
        : "=t"(RESULT) \
        : "0"(VALUE) \
        );

float exp2(float value) {
    float result;
    EXP2(value, result);

    return result;
}

double exp2(double value) {
    double result;
    EXP2(value, result);

    return result;
}

#define LN(VALUE, RESULT) asm volatile ( \
        "fld1;" /* st0 = 1, st1 = value */ \
        "fldl2e;" /* st0 = log2(e), st1 = 1, st2 = value */ \
        "fdivrp;" /* st0 = 1 / log2(e), st1 = value */ \
        "fxch %%st(1);" /* st0 = value, st1 = 1 / log2(e) */ \
        "fyl2x;" /* st0 = log(value) */ \
        : "=t"(RESULT) \
        : "0"(VALUE) \
        );

float ln(float value) {
    if (value <= 0) {
        Util::Exception::throwException(Util::Exception::INVALID_ARGUMENT, "Math: Logarithm of non-positive value!");
    }

    float result;
    LN(value, result);

    return result;
}

double ln(double value) {
    if (value <= 0) {
        Util::Exception::throwException(Util::Exception::INVALID_ARGUMENT, "Math: Logarithm of non-positive value!");
    }

    double result;
    LN(value, result);

    return result;
}

#define LOG10(VALUE, RESULT) asm volatile ( \
        "fld1;" /* st0 = 1, st1 = value */ \
        "fldl2t;" /* st0 = log2(10), st1 = 1, st2 = value */ \
        "fdivrp;" /* st0 = 1 / log2(10), st1 = value */ \
        "fxch %%st(1);" /* st0 = value, st1 = 1 / log2(10) */ \
        "fyl2x;" /* st0 = log10(value) */ \
        : "=t"(RESULT) \
        : "0"(VALUE) \
        );

float log10(float value) {
    if (value <= 0) {
        Util::Exception::throwException(Util::Exception::INVALID_ARGUMENT, "Math: Logarithm of non-positive value!");
    }

    float result;
    LOG10(value, result);

    return result;
}

double log10(double value) {
    if (value <= 0) {
        Util::Exception::throwException(Util::Exception::INVALID_ARGUMENT, "Math: Logarithm of non-positive value!");
    }

    double result;
    LOG10(value, result);

    return result;
}

#define POW(BASE, EXPONENT, RESULT) asm volatile ( \
        "fyl2x;" /* st0: ex * log2(b) */ \
        "fld1;" /* st0: 1, st1: ex * log2(b) */ \
        "fld %%st(1);" /* st0: ex * log2(b)  st1: 1, st2: ex * log2(b) */ \
        "fprem;" /* st0: rem(ex * log2(b) ), st: 1, st2: ex * log2(b) */ \
        "f2xm1;" /* st0: 2 ^ rem(exponent * log2(base)) - 1, s1: 1, s2: ex * log2(b) */ \
        "faddp;" /* st0: 2 ^ rem(exponent * log2(base)), st1: ex * log2(b) */ \
        "fscale;" /* st0 *= 2^ int (ex * log2(b) ) */ \
        "fstp %%st(1);" /* clear st1 -> result in st0 */ \
        : "=t"(RESULT) \
        : "0"(BASE),"u"(EXPONENT) \
        );

float pow(float base, float exponent) {
    if (base == 0) {
        return 0;
    } else if (base > 0) {
        float result;
        POW(base, exponent, result);

        return result;
    } else {
        if (modulo(exponent, 1) != 0) {
            Util::Exception::throwException(Util::Exception::INVALID_ARGUMENT, "Math: Negative base with non-integer exponent!");
        }

        bool baseOdd = static_cast<int64_t>(exponent) % 2 == 1;
        auto baseAbs = absolute(base);
        float result;
        POW(baseAbs, exponent, result);

        return baseOdd ? -result : result;
    }
}

double pow(double base, double exponent) {
    if (base == 0) {
        return 0;
    } else if (base > 0) {
        double result;
        POW(base, exponent, result);

        return result;
    } else {
        if (modulo(exponent, 1) != 0) {
            Util::Exception::throwException(Util::Exception::INVALID_ARGUMENT, "Math: Negative base with non-integer exponent!");
        }

        bool baseOdd = static_cast<int64_t>(exponent) % 2 == 1;
        auto baseAbs = absolute(base);
        double result;
        POW(baseAbs, exponent, result);

        return baseOdd ? -result : result;
    }
}

#define SINE(VALUE, RESULT) asm volatile ( \
        "fsin;" \
        : "=t"(RESULT) \
        : "0"(VALUE) \
        );

float sine(float value) {
    float result;
    SINE(value, result);

    return result;
}

double sine(double value) {
    double result;
    SINE(value, result);

    return result;
}

#define COSINE(VALUE, RESULT) asm volatile ( \
        "fcos;" \
        : "=t"(RESULT) \
        : "0"(VALUE) \
        );

float cosine(float value) {
    float result;
    COSINE(value, result);

    return result;
}

double cosine(double value) {
    double result;
    COSINE(value, result);

    return result;
}

#define TANGENT(VALUE, RESULT) asm volatile ( \
        "fptan;" \
        "fstp %%st(0);" /* Pop 1.0 that was pushed by fptan */ \
        : "=t"(RESULT) \
        : "0"(VALUE) \
        );

float tangent(float value) {
    float result;
    TANGENT(value, result);

    return result;
}

double tangent(double value) {
    double result;
    TANGENT(value, result);

    return result;
}

#define COTANGENT(VALUE, RESULT) asm volatile ( \
        "fptan;" \
        "fdivp;" /* fptan pushes the result and 1.0 onto the stack */ \
        : "=t"(RESULT) \
        : "0"(VALUE) \
        );

float cotangent(float value) {
    float result;
    COTANGENT(value, result);

    return result;
}

double cotangent(double value) {
    double result;
    COTANGENT(value, result);

    return result;
}

#define ARCTANGENT(VALUE, DIVISOR, RESULT) asm volatile ( \
        "fpatan;" \
        : "=t"(RESULT) \
        : "u"(VALUE), "0"(DIVISOR) \
        );

float arctangent(float value, float divisor) {
    float result;
    ARCTANGENT(value, divisor, result);

    return result;
}

double arctangent(double value, double divisor) {
    double result;
    ARCTANGENT(value, divisor, result);

    return result;
}

#define SQRT(VALUE, RESULT) asm volatile ( \
        "fsqrt;" \
        : "=t"(RESULT) \
        : "0"(VALUE) \
        );

float sqrt(float value) {
    if (value < 0) {
        Util::Exception::throwException(Util::Exception::INVALID_ARGUMENT, "Math: Square root of negative number!");
    }

    float result;
    SQRT(value, result);

    return result;
}

double sqrt(double value) {
    if (value < 0) {
        return 0;
    }

    float result;
    SQRT(value, result);

    return result;
}


float arcsine(float value) {
    if (value > 1 || value < -1) {
        Util::Exception::throwException(Util::Exception::INVALID_ARGUMENT, "Math: Arcsine of value outside of [-1, 1]!");
    }

    auto divisor = sqrt(1 - value * value);
    return arctangent(value, divisor);
}

double arcsine(double value) {
    if (value > 1 || value < -1) {
        Util::Exception::throwException(Util::Exception::INVALID_ARGUMENT, "Math: Arcsine of value outside of [-1, 1]!");
    }

    auto divisor = sqrt(1 - value * value);
    return arctangent(value, divisor);
}

float arccosine(float value) {
    if (value > 1 || value < -1) {
        Util::Exception::throwException(Util::Exception::INVALID_ARGUMENT, "Math: Arccosine of value outside of [-1, 1]!");
    }

    return static_cast<float>(PI / 2.0) - arcsine(value);
}

double arccosine(double value) {
    if (value > 1 || value < -1) {
        Util::Exception::throwException(Util::Exception::INVALID_ARGUMENT, "Math: Arccosine of value outside of [-1, 1]!");
    }

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

#define ROUND(VALUE, RESULT) asm volatile ( \
        "frndint;" \
        : "=t"(RESULT) \
        : "0"(VALUE) \
        );

float round(float value) {
    float result;
    ROUND(value, result);

    return result;
}

double round(double value) {
    double result;
    ROUND(value, result);

    return result;
}

double truncate(double value) {
    return (static_cast<double>((static_cast<long long>(value))));
}

float truncate(float value) {
    return (static_cast<float>((static_cast<long long>(value))));
}

double floor(double arg) {
	auto ret = static_cast<double>((static_cast<long long>(arg)));
	if (ret > arg) {
        ret--;
    };

	return ret;
}
	
float floor(float arg) {
    auto ret = static_cast<float>((static_cast<long long>(arg)));
    if (ret > arg) {
        ret--;
    };

    return ret;
}

double getDoubleInternals(double arg, int *exponent) {
	auto argb = std::bit_cast<long long>(arg);
	
	*exponent = ((int)((argb >> 52) & ((1<<11)-1))) - 1022; // get exponent bits
	
	argb &= ~(((1LL<<11)-1)<<52); // clear exponent bits
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