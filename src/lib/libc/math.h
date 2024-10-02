#ifndef LIBC_MATH_H
#define LIBC_MATH_H 

#define HUGE_VAL 1.0/0.0


#ifdef __cplusplus
extern "C" {
#endif

double fabs(double a);
float fabsf(float a);

double fmod(double x, double y);

double exp(double arg);
double exp2(double arg);
double log(double arg);
double log10(double arg);
double log2(double arg);
double pow(double base, double exponent);
double sqrt(double arg);

float expf(float arg);
float exp2f(float arg);
float logf(float arg);
float log10f(float arg);
float log2f(float arg);
float powf(float base, float exponent);
float sqrtf(float arg);

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
double asinh(double arg);
double acosh(double arg);
double atanh(double arg);

float sinf(float arg);
float cosf(float arg);
float tanf(float arg);
float asinf(float arg);
float acosf(float arg);
float atanf(float arg);
float atan2f(float y, float x);
float sinhf(float arg);
float coshf(float arg);
float tanhf(float arg);
float asinhf(float arg);
float acoshf(float arg);
float atanhf(float arg);

double ceil(double arg);
double floor(double arg);
double round(double arg);
double trunc(double arg);
double nearbyint(double arg);

float ceilf(float arg);
float floorf(float arg);
float roundf(float arg);
float truncf(float arg);
float nearbyintf(float arg);

double frexp(double arg, int * exp);
double ldexp(double arg, int exp);
double modf(double arg, double * iptr);

int isinf(double arg);
int isnan(double arg);

#ifdef __cplusplus
}
#endif

#endif