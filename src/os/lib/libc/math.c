/*
 * Copyright (C) 2018 Burak Akguel, Christian Gesse, Fabian Ruhland, Filip Krakowski, Michael Schoettner
 * Heinrich-Heine University
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

#include "math.h"
#include "errno.h"

#ifndef M_PI
#define M_PI 3.14159265358979323846
#endif

#ifndef M_E
#define M_E 2.71828182845904523536
#endif

double sqrt(double x) {
    if(x < 0) {
        errno = EDOM;
        return -1;
    }
    
    double res;
    asm("fsqrt" : "=t" (res) : "0" (x));
    return res;
}

double sin(double x) {
    double res;
    asm("fsin" : "=t" (res) : "0" (x));
    return res;
}

double cos(double x) {
    double res;
    asm("fcos" : "=t" (res) : "0" (x));
    return res;
}

double tan(double x) {
    double res;
    asm("fptan" : "=t" (res) : "0" (x));
    return res;
}

double atan(double x) {
    double res;
    asm("fpatan" : "=t" (res) : "0" (x));
    return res;
}

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

double frexp(double x, int *e) {
    char neg = (char) (x < 0 ? -1 : 1);
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
