/*
 * Copyright (C) 2018-2023 Heinrich-Heine-Universitaet Duesseldorf,
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

double pow(double value, int exponent) {
    double ret = 1;
    for(int i = 1; i <= exponent; i++){
        ret *= value;
    }

    return ret;
}

}