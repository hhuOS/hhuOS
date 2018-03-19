#include "math.h"
#include "errno.h"

#ifndef M_PI
#define M_PI 3.14159265358979323846
#endif

#ifndef M_E
#define M_E 2.71828182845904523536
#endif

/**
 * Calculates the square root of a given number.
 * 
 * @param x The number, whose square root shall be calculated.
 * 
 * @return The square root of x.
 */
double sqrt(double x) {
    if(x < 0) {
        errno = EDOM;
        return -1;
    }
    
    double res;
    asm("fsqrt" : "=t" (res) : "0" (x));
    return res;
}

/**
 * Calculates the sinus of a given number.
 * 
 * @param x The number, whose sinus shall be calculated.
 * 
 * @return The sinus of x.
 */
double sin(double x) {
    double res;
    asm("fsin" : "=t" (res) : "0" (x));
    return res;
}

/**
 * Calculates the cosinus of a given number.
 * 
 * @param x The number, whose cosinus shall be calculated.
 * 
 * @return The cosinus of x.
 */
double cos(double x) {
    double res;
    asm("fcos" : "=t" (res) : "0" (x));
    return res;
}

/**
 * Calculates the tangens of a given number.
 * 
 * @param x The number, whose tangens shall be calculated.
 * 
 * @return The tangens of x.
 */
double tan(double x) {
    double res;
    asm("fptan" : "=t" (res) : "0" (x));
    return res;
}

/**
 * Calculates the arctan of a given number.
 * 
 * @param x The number, whose arctan shall be calculated.
 * 
 * @return The arctan of x.
 */
double atan(double x) {
    double res;
    asm("fpatan" : "=t" (res) : "0" (x));
    return res;
}

/**
 * Calculates the arctan2 of a given number.
 * 
 * @param x The number, whose arctan2 shall be calculated.
 * 
 * @return The arctan2 of x.
 */
double atan2(double x, double y) {
    if(x == 0 && y == 0) {
        errno = EDOM;
        return -1;
    }
    
    if(x > 0) return atan(y / x);
        
    if(x < 0) {
        if(y > 0) return atan(y / x) + M_PI;
        if(y == 0) return -(M_PI / 2);
        if(y < 0) return atan(y / x) - M_PI;
    }
    
    if(x == 0) {
        if(y > 0) return M_PI / 2;
        if(y < 0) return -(M_PI / 2);
    }
    
    errno = EDOM;
    return -1;
}

/**
 * Calculates the exponential function of a given number.
 * 
 * @param x The number, whose exponential function shall be calculated.
 * 
 * @return The exponential value of x.
 */
double exp(double x) {
    if(x == 0) return 1;
    
    char neg_exp = (x < 0);
    if(neg_exp) x = -x;
        
    double res = M_E;
    int i;
    for(i = 0; i < x; i++) {
        res *= M_E;
    }
    
    if(neg_exp) res = 1 / res;
    
    return res;
}

/**
 * Breaks a floating-point number into a normalized fraction and an integral power of 2.
 * Example: x = m * (2 ^ e)
 *          1024 = 0.5 * (2 ^ 11)
 * 
 * @param x The number, whose normalized fraction shall be calculated.
 * @param e A pointer to an integer, where the calculated exponent will be saved.
 * 
 * @return The normalized fraction of x.
 */
double frexp(double x, int *e) {
    char neg = x < 0 ? -1 : 1;
    x = x * neg;
    
    int i = 1;
    int exp = 0;
    if(x >= 0.5) {
        while(i <= x) {
            i = i << 1;
            exp++;
        }
        *e = exp;
        return (x / (i)) * neg;
    } else {
        i = 2;
        while((1.0 / i) >= x) {
            i = i << 1;
            exp++;
        }
        *e = -exp;
        return x / (1.0 / (i >> 1)) * neg;
    }
}

/**
 * Calculates the power function of a given number.
 * 
 * @param x The number, that shall be raised.
 * @param y The exponent.
 * 
 * @return The value of x raised to the power of y.
 */
double pow(double x, double y) {
    if(x < 0 && y != (int)y) {
        errno = EDOM;
        return -1;
    }
    
    if(y == 0) return 1;
    
    char neg_exp = (y < 0);
    if(neg_exp) y = -y;
        
    double res = x;
    int i;
    for(i = 0; i < y; i++) {
        res *= x;
    }
    
    if(neg_exp) res = 1 / res;
    
    return res;
}
