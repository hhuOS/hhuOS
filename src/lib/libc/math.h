#ifndef LIBC_MATH_H
#define LIBC_MATH_H 

#define HUGE_VAL 1.0/0.0


#ifdef __cplusplus
extern "C" {
#endif

double fabs(double a);
double fmod(double x, double y);

double exp(double arg);
double log(double arg);
double log10(double arg);
double pow(double base, double exponent);
double sqrt( double arg );

double sin(double arg);
double cos(double arg);
double tan(double arg);
double asin(double arg);
double acos(double arg);
double atan(double arg);
double atan2(double y, double x);
double sinh(double arg);
double cosh(double arg);
double tanh(double arg);

double ceil(double arg);
double floor(double arg);

double frexp(double arg, int * exp);
double ldexp(double arg, int exp);
double modf(double arg, double * iptr);

int isinf(double arg);
int isnan(double arg);

#ifdef __cplusplus
}
#endif

#endif