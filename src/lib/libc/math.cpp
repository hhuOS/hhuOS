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
 *
 * The stanard C library is based on a master's thesis, written by Tobias Fabian Oehme.
 * The original source code can be found here: https://github.com/ToboterXP/hhuOS/tree/thesis
 */

#include <string.h>

#include "math.h"

#include "util/math/Math.h"
#include "lib/libc/errno.h"

double fabs(const double a) {
	return Util::Math::absolute(a);
}

float fabsf(const float a) {
    return Util::Math::absolute(a);
}

double fmod(const double x, const double y) {
	if (y == 0) {
		setErrno(EDOM);
		return HUGE_VAL;
	}

	return Util::Math::modulo(x, y);
}

double exp(const double arg) {
	return Util::Math::exp(arg);
}

double exp2(const double arg) {
    return Util::Math::exp2(arg);
}

double log(const double arg) {
	if (arg <= 0) {
	    setErrno(EDOM);
	}

	return Util::Math::lnUnchecked(arg);
}

double log10(const double arg) {
	if (arg <= 0) {
	    setErrno(EDOM);
	}

	return Util::Math::log10Unchecked(arg);
}

double log2(const double arg) {
    if (arg <= 0) {
        setErrno(EDOM);
    }

    return Util::Math::lnUnchecked(arg) / Util::Math::lnUnchecked(2.0);
}

double pow(const double base, const double exponent) {
	return Util::Math::pow(base, exponent);
}

double sqrt(const double arg) {
	return Util::Math::sqrt(arg);
}


float expf(const float arg) {
    return Util::Math::exp(arg);
}

float exp2f(const float arg) {
    return Util::Math::exp2(arg);
}

float logf(const float arg) {
    if (arg <= 0) {
        setErrno(EDOM);
    }

    return Util::Math::lnUnchecked(arg);
}

float log10f(const float arg) {
    if (arg <= 0) {
        setErrno(EDOM);
    }

    return Util::Math::log10Unchecked(arg);
}

float log2f(const float arg) {
    if (arg <= 0) {
        setErrno(EDOM);
    }

    return Util::Math::lnUnchecked(arg) / Util::Math::lnUnchecked(2.0f);
}

float powf(const float base, const float exponent) {
    return Util::Math::pow(base, exponent);
}

float sqrtf(const float arg) {
    return Util::Math::sqrt(arg);
}


double sin(const double arg) {
	return Util::Math::sine(arg);
}

double cos(const double arg) {
	return Util::Math::cosine(arg);
}

double tan(const double arg) {
	return Util::Math::tangent(arg);
}

double asin(const double arg) {
	if (arg < -1 || arg > 1) {
		setErrno(EDOM);
	}

	return Util::Math::arcsineUnchecked(arg);
}

double acos(const double arg) {
	if (arg < -1 || arg > 1) {
		setErrno(EDOM);
	}

	return Util::Math::arccosineUnchecked(arg);
}

double atan(const double arg) {
	return Util::Math::arctangent(arg);
}

double atan2(const double y, const double x) {
	return Util::Math::arctangent(y, x);
}

double sinh(const double arg) {
	return (exp(arg) - exp(-arg)) / 2;
}

double cosh(const double arg) {
	return (exp(arg) + exp(-arg)) / 2;
}

double tanh(const double arg) {
	return (exp(arg) - exp(-arg)) / (exp(arg) + exp(-arg)) ;
}

double asinh(const double arg) {
    return log(arg + sqrt(arg * arg + 1));
}

double acosinh(const double arg) {
    return log(arg + sqrt(arg * arg - 1));
}

double atanh(const double arg) {
    return log((1 + arg) / (1 - arg)) / 2;
}


float sinf(const float arg) {
    return Util::Math::sine(arg);
}

float cosf(const float arg) {
    return Util::Math::cosine(arg);
}

float tanf(const float arg) {
    return Util::Math::tangent(arg);
}

float asinf(const float arg) {
    if (arg < -1 || arg > 1) {
        setErrno(EDOM);
    }

    return Util::Math::arcsineUnchecked(arg);
}

float acosf(const float arg) {
    if (arg < -1 || arg > 1) {
        setErrno(EDOM);
    }

    return Util::Math::arccosineUnchecked(arg);
}

float atanf(const float arg) {
    return Util::Math::arctangent(arg);
}

float atan2f(const float y, const float x) {
    return Util::Math::arctangent(y, x);
}

float sinhf(const float arg) {
    return (expf(arg) - expf(-arg)) / 2;
}

float coshf(const float arg) {
    return (expf(arg) + expf(-arg)) / 2;
}

float tanhf(const float arg) {
    return (expf(arg) - expf(-arg)) / (expf(arg) + expf(-arg)) ;
}

float asinhf(const float arg) {
    return logf(arg + sqrtf(arg * arg + 1));
}

float acoshf(const float arg) {
    return logf(arg + sqrtf(arg * arg - 1));
}

float atanhf(const float arg) {
    return logf((1 + arg) / (1 - arg)) / 2;
}

double ceil(const double arg) {
	return Util::Math::ceil(arg);
}

double floor(const double arg) {
	return Util::Math::floor(arg);
}

double round(const double arg) {
    return Util::Math::round(arg);
}

double trunc(const double arg) {
    return Util::Math::truncate(arg);
}

double nearbyint(const double arg) {
    return Util::Math::round(arg);
}

float ceilf(const float arg) {
    return Util::Math::ceil(arg);
}

float floorf(const float arg) {
    return Util::Math::floor(arg);
}

float roundf(const float arg) {
    return Util::Math::round(arg);
}

float truncf(const float arg) {
    return Util::Math::truncate(arg);
}

float nearbyint(const float arg) {
    return Util::Math::round(arg);
}

double frexp(const double arg, int *exp) {
	const auto internals = Util::Math::getInternals(arg);

    *exp = internals.getSecond();
    return internals.getFirst();
}

double ldexp(const double arg, int exp) {
	long long argb;
    memcpy(&argb, &arg, sizeof(arg));
	
	exp += static_cast<int>((argb >> 52) & ((1 << 11) - 1)); //get exponent bits
	argb &= ~(((1LL << 11) - 1) << 52); //clear exponent bits
	argb |= (static_cast<long long>(exp) << 52); // set exponent to new value

    double ret;
    Util::Address(&ret).copyRange(Util::Address(&argb), sizeof(ret));
	return ret;
}

double modf(const double arg, double *iptr) {
	*iptr = static_cast<long long>(arg);
	return arg - *iptr;
}

int isinf(const double arg) {
	return Util::Math::isInfinity(arg);
}

int isnan(const double arg) {
	return Util::Math::isNan(arg);
}