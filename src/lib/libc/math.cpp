
#include <bit>

#include "lib/libc/math.h"
#include "lib/util/math/Math.h"
#include "lib/libc/errno.h"
#include "lib/util/base/System.h"
#include <float.h>

double fabs(double a) {
	return Util::Math::absolute(a);
}

float fabsf(float a) {
    return Util::Math::absolute(a);
}


double fmod(double x, double y) {
	if (y == 0) {
		setErrno(EDOM);
		return HUGE_VAL;
	}
	return Util::Math::modulo(x, y);
}



double exp(double arg) {
	return Util::Math::exp(arg);
}

double exp2(double arg) {
    return Util::Math::pow(2, arg);
}

double log(double arg) {
	if (arg <= 0) setErrno(EDOM);
	return Util::Math::ln(arg);
}

double log10(double arg) {
	if (arg <= 0) setErrno(EDOM);
	return Util::Math::log10(arg);
}

double log2(double arg) {
    if (arg <= 0) setErrno(EDOM);
    return Util::Math::ln(arg) / Util::Math::ln(2.0);
}

double pow(double base, double exponent) {
	return Util::Math::pow(base, exponent);
}

double sqrt( double arg ) {
	return Util::Math::sqrt( arg );
}


float expf(float arg) {
    return Util::Math::exp(arg);
}

float exp2f(float arg) {
    return Util::Math::exp2(arg);
}

float logf(float arg) {
    if (arg <= 0) setErrno(EDOM);
    return Util::Math::ln(arg);
}

float log10f(float arg) {
    if (arg <= 0) setErrno(EDOM);
    return Util::Math::log10(arg);
}

float log2f(float arg) {
    if (arg <= 0) setErrno(EDOM);
    return Util::Math::ln(arg) / Util::Math::ln(2.0f);
}

float powf(float base, float exponent) {
    return Util::Math::pow(base, exponent);
}

float sqrtf( float arg ) {
    return Util::Math::sqrt( arg );
}


double sin(double arg) {
	return Util::Math::sine(arg);
}

double cos(double arg) {
	return Util::Math::cosine(arg);
}

double tan(double arg) {
	return Util::Math::tangent(arg);
}

double asin(double arg) {
	if (arg < -1 || arg > 1) {
		setErrno(EDOM);
	}
	return Util::Math::arcsine(arg);
}

double acos(double arg) {
	if (arg < -1 || arg > 1) {
		setErrno(EDOM);
	}
	return Util::Math::arccosine(arg);
}

double atan(double arg) {
	return Util::Math::arctangent(arg);
}

double atan2(double y, double x) {
	return Util::Math::arctangent(y, x);
}

double sinh(double arg) {
	return (exp(arg) - exp(-arg)) / 2;
}

double cosh(double arg) {
	return (exp(arg) + exp(-arg)) / 2;
}

double tanh(double arg) {
	return (exp(arg) - exp(-arg)) / (exp(arg) + exp(-arg)) ;
}

double asinh(double arg) {
    return log(arg + sqrt(arg * arg + 1));
}

double acosinh(double arg) {
    return log(arg + sqrt(arg * arg - 1));
}

double atanh(double arg) {
    return log((1 + arg) / (1 - arg)) / 2;
}


float sinf(float arg) {
    return Util::Math::sine(arg);
}

float cosf(float arg) {
    return Util::Math::cosine(arg);
}

float tanf(float arg) {
    return Util::Math::tangent(arg);
}

float asinf(float arg) {
    if (arg < -1 || arg > 1) {
        setErrno(EDOM);
    }
    return Util::Math::arcsine(arg);
}

float acosf(float arg) {
    if (arg < -1 || arg > 1) {
        setErrno(EDOM);
    }
    return Util::Math::arccosine(arg);
}

float atanf(float arg) {
    return Util::Math::arctangent(arg);
}

float atan2f(float y, float x) {
    return Util::Math::arctangent(y, x);
}

float sinhf(float arg) {
    return (expf(arg) - expf(-arg)) / 2;
}

float coshf(float arg) {
    return (expf(arg) + expf(-arg)) / 2;
}

float tanhf(float arg) {
    return (expf(arg) - expf(-arg)) / (expf(arg) + expf(-arg)) ;
}

float asinhf(float arg) {
    return logf(arg + sqrtf(arg * arg + 1));
}

float acoshf(float arg) {
    return logf(arg + sqrtf(arg * arg - 1));
}

float atanhf(float arg) {
    return logf((1 + arg) / (1 - arg)) / 2;
}



double ceil(double arg) {
	double ret = (double)((long long)arg);
	if (ret < arg) ret++;
	return ret;
}

double floor(double arg) {
	return Util::Math::floor(arg);
}

double round(double arg) {
    return Util::Math::round(arg);
}

double trunc(double arg) {
    return Util::Math::truncate(arg);
}

double nearbyint(double arg) {
    return Util::Math::round(arg);
}


float ceilf(float arg) {
    float ret = (float)((long long)arg);
    if (ret < arg) ret++;
    return ret;
}

float floorf(float arg) {
    return Util::Math::floor(arg);
}

float roundf(float arg) {
    return Util::Math::round(arg);
}

float truncf(float arg) {
    return Util::Math::truncate(arg);
}

float nearbyint(float arg) {
    return Util::Math::round(arg);
}


double frexp(double arg, int * exp) {
	return Util::Math::getDoubleInternals(arg, exp);
}

double ldexp(double arg, int exp) {
	long long argb = std::bit_cast<long long>(arg);
	
	exp += ((int)((argb >> 52) & ((1<<11)-1))); //get exponent bits
	argb &= ~(((1LL<<11)-1)<<52); //clear exponent bits
	argb |= (((long long)exp)<<52); // set exponent to new value
	
	return std::bit_cast<double>(argb);
}

double modf(double arg, double * iptr) {
	*iptr = (long long)arg;
	return arg - *iptr;
}

int isinf(double arg) {
	return Util::Math::isInfinity(arg);
}

int isnan(double arg) {
	return Util::Math::isNan(arg);
}