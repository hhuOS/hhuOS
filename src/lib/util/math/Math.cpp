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

#include <stdint.h>

#include "Math.h"

#include "util/base/Address.h"
#include "util/base/Panic.h"

namespace Util::Math {

uint32_t absolute(const int32_t value) {
    return value < 0 ? -value : value;
}

uint64_t absolute(const int64_t value) {
    return value < 0 ? -value : value;
}

#define ABSOLUTE(VALUE, RESULT) \
    asm volatile ( \
    "fabs;" \
    : "=t"(RESULT) \
    : "0"(VALUE) \
    );

float absolute(const float value) {
    float result;
    ABSOLUTE(value, result);

    return result;
}

double absolute(const double value) {
    double result;
    ABSOLUTE(value, result);

    return result;
}

float min(const float first, const float second) {
    return first < second ? first : second;
}

float max(const float first, const float second) {
    return first > second ? first : second;
}

double min(const double first, const double second) {
    return first < second ? first : second;
}

double max(const double first, const double second) {
    return first > second ? first : second;
}

float min(const float first, const float second, const float third) {
    return min(min(first, second), third);
}

float max(const float first, const float second, const float third) {
    return max(max(first, second), third);
}

double min(const double first, const double second, const double third) {
    return min(min(first, second), third);
}

double max(const double first, const double second, const double third) {
    return max(max(first, second), third);
}

bool equals(const float first, const float second, const float epsilon) {
    return absolute(first - second) < epsilon;
}

bool equals(const double first, const double second, const double epsilon) {
    return absolute(first - second) < epsilon;
}

float modulo(const float dividend, const float divisor) {
    return dividend - floor(dividend / divisor) * divisor;
}

double modulo(const double dividend, const double divisor) {
    return dividend - floor(dividend / divisor) * divisor;
}

#define ROUND(VALUE, RESULT) \
    asm volatile ( \
    "frndint;" \
    : "=t"(RESULT) \
    : "0"(VALUE) \
    );

float round(const float value) {
    float result;
    ROUND(value, result);

    return result;
}

double round(const double value) {
    double result;
    ROUND(value, result);

    return result;
}

#define TRUNCATE(VALUE, RESULT) \
    uint16_t fpuControlWordBackup; \
    uint16_t fpuControlWord; \
    asm volatile ( \
        "fnstcw %1;" /* Store FPU control word */ \
        "mov %1, %%ax;" /* Move control word to AX */ \
        "or $0x0c, %%ah;" /* Set rounding mode bits (11 -> Round towards zero/Truncate) */ \
        "mov %%ax, %2;" /* Store modified control word */ \
        "fldcw %2;" /* Load modified control word */ \
        "frndint;" /* Performing rounding operations */ \
        "fldcw %1;" /* Restore original FPU control word */ \
        : "=t"(RESULT), "+m"(fpuControlWordBackup), "+m"(fpuControlWord) \
        : "0"(VALUE) \
    );

float truncate(const float value) {
    float result;
    TRUNCATE(value, result);

    return result;
}

double truncate(const double value) {
    double result;
    TRUNCATE(value, result);

    return result;
}

#define FLOOR(VALUE, RESULT) \
    uint16_t fpuControlWordBackup; \
    uint16_t fpuControlWord; \
    asm volatile ( \
        "fnstcw %1;" /* Store FPU control word */ \
        "mov %1, %%ax;" /* Move control word to AX */ \
        "or $0x04, %%ah;" /* Set rounding mode bits (01 -> Round down) */ \
        "mov %%ax, %2;" /* Store modified control word */ \
        "fldcw %2;" /* Load modified control word */ \
        "frndint;" /* Performing rounding operations */ \
        "fldcw %1;" /* Restore original FPU control word */ \
        : "=t"(RESULT), "+m"(fpuControlWordBackup), "+m"(fpuControlWord) \
        : "0"(VALUE) \
    );

float floor(const float value) {
    float result;
    FLOOR(value, result);

    return result;
}

double floor(const double value) {
    double result;
    FLOOR(value, result);

    return result;
}

#define CEIL(VALUE, RESULT) \
    uint16_t fpuControlWordBackup; \
    uint16_t fpuControlWord; \
    asm volatile ( \
        "fnstcw %1;" /* Store FPU control word */ \
        "mov %1, %%ax;" /* Move control word to AX */ \
        "or $0x08, %%ah;" /* Set rounding mode bits (10 -> Round up) */ \
        "mov %%ax, %2;" /* Store modified control word */ \
        "fldcw %2;" /* Load modified control word */ \
        "frndint;" /* Performing rounding operations */ \
        "fldcw %1;" /* Restore original FPU control word */ \
        : "=t"(RESULT), "+m"(fpuControlWordBackup), "+m"(fpuControlWord) \
        : "0"(VALUE) \
    );

float ceil(const float value) {
    float result;
    CEIL(value, result);

    return result;
}

double ceil(const double value) {
    double result;
    CEIL(value, result);

    return result;
}

#define EXP(VALUE, RESULT) \
    asm volatile ( \
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

float exp(const float value) {
    float result;
    EXP(value, result);

    return result;
}

double exp(const double value) {
    float result;
    EXP(value, result);

    return result;
}

#define EXP2(VALUE, RESULT) \
    asm volatile ( \
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

float exp2(const float value) {
    float result;
    EXP2(value, result);

    return result;
}

double exp2(const double value) {
    double result;
    EXP2(value, result);

    return result;
}

#define LN(VALUE, RESULT) \
    asm volatile ( \
    "fld1;" /* st0 = 1, st1 = value */ \
    "fldl2e;" /* st0 = log2(e), st1 = 1, st2 = value */ \
    "fdivrp;" /* st0 = 1 / log2(e), st1 = value */ \
    "fxch %%st(1);" /* st0 = value, st1 = 1 / log2(e) */ \
    "fyl2x;" /* st0 = log(value) */ \
    : "=t"(RESULT) \
    : "0"(VALUE) \
    );

float ln(const float value) {
    if (value <= 0) {
        Panic::fire(Panic::INVALID_ARGUMENT, "Math: Logarithm of non-positive value!");
    }

    float result;
    LN(value, result);

    return result;
}

double ln(const double value) {
    if (value <= 0) {
        Panic::fire(Panic::INVALID_ARGUMENT, "Math: Logarithm of non-positive value!");
    }

    double result;
    LN(value, result);

    return result;
}

#define LOG10(VALUE, RESULT) \
    asm volatile ( \
    "fld1;" /* st0 = 1, st1 = value */ \
    "fldl2t;" /* st0 = log2(10), st1 = 1, st2 = value */ \
    "fdivrp;" /* st0 = 1 / log2(10), st1 = value */ \
    "fxch %%st(1);" /* st0 = value, st1 = 1 / log2(10) */ \
    "fyl2x;" /* st0 = log10(value) */ \
    : "=t"(RESULT) \
    : "0"(VALUE) \
    );

float log10(const float value) {
    if (value <= 0) {
        Panic::fire(Panic::INVALID_ARGUMENT, "Math: Logarithm of non-positive value!");
    }

    float result;
    LOG10(value, result);

    return result;
}

double log10(const double value) {
    if (value <= 0) {
        Panic::fire(Panic::INVALID_ARGUMENT, "Math: Logarithm of non-positive value!");
    }

    double result;
    LOG10(value, result);

    return result;
}

#define POW(BASE, EXPONENT, RESULT) \
    asm volatile ( \
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

float pow(const float base, const float exponent) {
    if (base == 0) {
        return 0;
    } else if (base > 0) {
        float result;
        POW(base, exponent, result);

        return result;
    } else {
        if (modulo(exponent, 1) != 0) {
            Panic::fire(Panic::INVALID_ARGUMENT, "Math: Negative base with non-integer exponent!");
        }

        bool baseOdd = static_cast<int64_t>(exponent) % 2 == 1;
        auto baseAbs = absolute(base);
        float result;
        POW(baseAbs, exponent, result);

        return baseOdd ? -result : result;
    }
}

double pow(const double base, const double exponent) {
    if (base == 0) {
        return 0;
    } else if (base > 0) {
        double result;
        POW(base, exponent, result);

        return result;
    } else {
        if (modulo(exponent, 1) != 0) {
            Panic::fire(Panic::INVALID_ARGUMENT, "Math: Negative base with non-integer exponent!");
        }

        bool baseOdd = static_cast<int64_t>(exponent) % 2 == 1;
        auto baseAbs = absolute(base);
        double result;
        POW(baseAbs, exponent, result);

        return baseOdd ? -result : result;
    }
}

#define SQRT(VALUE, RESULT) \
    asm volatile ( \
    "fsqrt;" \
    : "=t"(RESULT) \
    : "0"(VALUE) \
    );

float sqrt(const float value) {
    if (value < 0) {
        Util::Panic::fire(Panic::INVALID_ARGUMENT, "Math: Square root of negative number!");
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

#define TANGENT(VALUE, RESULT) \
    asm volatile ( \
    "fptan;" \
    "fstp %%st(0);" /* Pop 1.0 that was pushed by fptan */ \
    : "=t"(RESULT) \
    : "0"(VALUE) \
    );

float tangent(const float value) {
    float result;
    TANGENT(value, result);

    return result;
}

double tangent(const double value) {
    double result;
    TANGENT(value, result);

    return result;
}

#define COTANGENT(VALUE, RESULT) \
    asm volatile ( \
    "fptan;" \
    "fdivp;" /* fptan pushes the result and 1.0 onto the stack */ \
    : "=t"(RESULT) \
    : "0"(VALUE) \
    );

float cotangent(const float value) {
    float result;
    COTANGENT(value, result);

    return result;
}

double cotangent(const double value) {
    double result;
    COTANGENT(value, result);

    return result;
}

#define ARCTANGENT(VALUE, DIVISOR, RESULT) \
    asm volatile ( \
    "fpatan;" \
    : "=t"(RESULT) \
    : "u"(VALUE), "0"(DIVISOR) \
    );

float arctangent(const float value, const float divisor) {
    float result;
    ARCTANGENT(value, divisor, result);

    return result;
}

double arctangent(const double value, const double divisor) {
    double result;
    ARCTANGENT(value, divisor, result);

    return result;
}

float arctangent2(const float y, const float x) {
    if (x > 0) {
        return arctangent(y / x);
    }
    if (x < 0 && y >= 0) {
        return arctangent(y / x) + PI_FLOAT;
    }
    if (x < 0 && y < 0) {
        return arctangent(y / x) - PI_FLOAT;
    }
    if (x == 0 && y > 0) {
        return PI_DOUBLE / 2;
    }
    if (x == 0 && y < 0) {
        return -PI_DOUBLE / 2;
    }

    return 0; // x == 0 && y == 0
}

double arctangent2(const double y, const double x) {
    if (x > 0) {
        return arctangent(y / x);
    }
    if (x < 0 && y >= 0) {
        return arctangent(y / x) + PI_DOUBLE;
    }
    if (x < 0 && y < 0) {
        return arctangent(y / x) - PI_DOUBLE;
    }
    if (x == 0 && y > 0) {
        return PI_DOUBLE / 2;
    }
    if (x == 0 && y < 0) {
        return -PI_DOUBLE / 2;
    }

    return 0; // x == 0 && y == 0
}


float arcsine(const float value) {
    if (value > 1 || value < -1) {
        Panic::fire(Panic::INVALID_ARGUMENT, "Math: Arcsine of value outside of [-1, 1]!");
    }

    const auto divisor = sqrt(1 - value * value);
    return arctangent(value, divisor);
}

double arcsine(const double value) {
    if (value > 1 || value < -1) {
        Panic::fire(Panic::INVALID_ARGUMENT, "Math: Arcsine of value outside of [-1, 1]!");
    }

    const auto divisor = sqrt(1 - value * value);
    return arctangent(value, divisor);
}

float arccosine(const float value) {
    if (value > 1 || value < -1) {
        Panic::fire(Panic::INVALID_ARGUMENT, "Math: Arccosine of value outside of [-1, 1]!");
    }

    return PI_FLOAT / 2.0f - arcsine(value);
}

double arccosine(const double value) {
    if (value > 1 || value < -1) {
        Panic::fire(Panic::INVALID_ARGUMENT, "Math: Arccosine of value outside of [-1, 1]!");
    }

    return PI_DOUBLE / 2.0 - arcsine(value);
}

float toRadians(const float degrees) {
    return degrees * (PI_FLOAT / 180.0f);
}

double toRadians(const double degrees) {
    return degrees * (PI_DOUBLE / 180.0);
}

float toDegrees(const float radians) {
    return radians * (180.0f / PI_FLOAT);
}

double toDegrees(const double radians) {
    return radians * (180.0f / PI_DOUBLE);
}

Pair<float, int8_t> getInternals(float value) {
    uint32_t bits;
    Address(&bits).copyRange(Address(&value), sizeof(value));

    const auto exponent = static_cast<int8_t>(((bits >> 23) & 0xff) - 126); // Get exponent bits

    bits &= ~(static_cast<uint32_t>(0xff) << 23); // Clear exponent bits
    bits |= static_cast<uint32_t>(126) << 23; // Set exponent to -1

    float mantissa;
    Address(&mantissa).copyRange(Address(&bits), sizeof(bits));

    return Pair<float, int8_t>(mantissa, exponent);
}

Pair<double, int16_t> getInternals(const double value) {
    uint64_t bits;
    Address(&bits).copyRange(Address(&value), sizeof(value));

    const auto exponent = static_cast<int16_t>(((bits >> 52) & 0x7ff) - 1022); // Get exponent bits
	
	bits &= ~(static_cast<uint64_t>(0x7ff) << 52); // Clear exponent bits
	bits |= static_cast<uint64_t>(1022) << 52; // Set exponent to -1

    double mantissa;
    Address(&mantissa).copyRange(Address(&bits), sizeof(bits));

	return Pair<double, int16_t>(mantissa, exponent);
}

bool isInfinity(const float value) {
    return value == 1.0f / 0.0f || value == -1.0f / 0.0f;
}

bool isInfinity(const double value) {
	return value == 1.0 / 0.0 || value == -1.0 / 0.0;
}

bool isNan(const float value) {
    return value != value; // NaN is not equal to itself
}

bool isNan(const double value) {
	return value != value; // NaN is not equal to itself
}

}
