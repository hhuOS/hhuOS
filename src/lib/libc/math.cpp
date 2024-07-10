
#include <bit>

#include "lib/libc/math.h"
#include "lib/util/math/Math.h"
#include "lib/libc/errno.h"
#include "lib/util/base/System.h"
#include <float.h>

#define HUGE_VAL pow(1000, 1000)

double fabs(double a) {
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

double log(double arg) {
	if (arg <= 0) setErrno(EDOM);
	return Util::Math::ln(arg);
}

double log10(double arg) {
	if (arg <= 0) setErrno(EDOM);
	return Util::Math::log10(arg);
}

double pow(double base, double exponent) {
	return Util::Math::pow(base, exponent);
}

double sqrt( double arg ) {
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




double ceil(double arg) {
	double ret = (double)((long long)arg);
	if (ret < arg) ret++;
	return ret;
}

double floor(double arg) {
	double ret = (double)((long long)arg);
	if (ret > arg) ret--;
	return ret;
}

double frexp(double arg, int * exp) {
	long long argb = std::bit_cast<long long>(arg);
	
	*exp = ((int)((argb >> 52) & ((1<<11)-1))) - 1022; //get exponent bits
	
	argb &= ~(((1LL<<11)-1)<<52); //clear exponent bits
	argb |= (1022LL<<52); // set exponent to -1
	return std::bit_cast<double>(argb);
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
	int exp;
	arg = frexp(arg, &exp);

	return (exp == 1025) && (arg == 0);
}

int isnan(double arg) {
	int exp;
	arg = frexp(arg, &exp);
	
	return (exp == 1025) && (arg != 0);
}