/*
 * Copyright (C) 2017-2026 Heinrich Heine University Düsseldorf,
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
 * Based on PortableGL gears demo (https://github.com/rswinkle/PortableGL/blob/master/examples/classic/gears.c)
 */

#include "PortableGlMath.h"

#include <util/math/Math.h>
#include <portablegl/portablegl.h>

void sincos(const double x, double *s, double *c) {
    *s = sin(x);
    *c = cos(x);
}

void multiply(GLfloat *m, const GLfloat *n) {
    GLfloat tmp[16];

    for (int i = 0; i < 16; i++) {
        tmp[i] = 0;
        const auto d = div(i, 4);
        const auto *row = n + d.quot * 4;
        const auto *column = m + d.rem;

        for (int j = 0; j < 4; j++)
            tmp[i] += row[j] * column[j * 4];
    }

    memcpy(m, &tmp, sizeof tmp);
}

void rotate(GLfloat *m, const GLfloat angle, const GLfloat x, const GLfloat y, const GLfloat z) {
    double s, c;

    sincos(angle, &s, &c);
    const GLfloat r[16] = {
        static_cast<GLfloat>(x * x * (1 - c) + c),     static_cast<GLfloat>(y * x * (1 - c) + z * s), static_cast<GLfloat>(x * z * (1 - c) - y * s), 0,
        static_cast<GLfloat>(x * y * (1 - c) - z * s), static_cast<GLfloat>(y * y * (1 - c) + c),     static_cast<GLfloat>(y * z * (1 - c) + x * s), 0,
        static_cast<GLfloat>(x * z * (1 - c) + y * s), static_cast<GLfloat>(y * z * (1 - c) - x * s), static_cast<GLfloat>(z * z * (1 - c) + c),     0,
        0, 0, 0, 1
    };

    multiply(m, r);
}

void translate(GLfloat *m, const GLfloat x, const GLfloat y, const GLfloat z) {
    const GLfloat t[16] = { 1, 0, 0, 0,  0, 1, 0, 0,  0, 0, 1, 0,  x, y, z, 1 };

    multiply(m, t);
}

void identity(GLfloat *m) {
    constexpr GLfloat t[16] = {
        1.0, 0.0, 0.0, 0.0,
        0.0, 1.0, 0.0, 0.0,
        0.0, 0.0, 1.0, 0.0,
        0.0, 0.0, 0.0, 1.0,
    };

    memcpy(m, t, sizeof(t));
}

void transpose(GLfloat *m) {
    const GLfloat t[16] = {
        m[0], m[4], m[8],  m[12],
        m[1], m[5], m[9],  m[13],
        m[2], m[6], m[10], m[14],
        m[3], m[7], m[11], m[15]
    };

    memcpy(m, t, sizeof(t));
}

void invert(GLfloat *m) {
    GLfloat t[16];
    identity(t);

    // Extract and invert the translation part 't'. The inverse of a
    // translation matrix can be calculated by negating the translation
    // coordinates.
    t[12] = -m[12]; t[13] = -m[13]; t[14] = -m[14];

    // Invert the rotation part 'r'. The inverse of a rotation matrix is
    // equal to its transpose.
    m[12] = m[13] = m[14] = 0;
    transpose(m);

    // inv(m) = inv(r) * inv(t)
    multiply(m, t);
}

void perspective(GLfloat *m, const GLfloat fovy, const GLfloat aspect, const GLfloat zNear, const GLfloat zFar) {
    GLfloat tmp[16];
    identity(tmp);

    double sine, cosine;
    const GLfloat radians = fovy / 2 * Util::Math::PI_FLOAT / 180;

    const double deltaZ = zFar - zNear;
    sincos(radians, &sine, &cosine);

    if (deltaZ == 0 || sine == 0 || aspect == 0) {
        return;
    }

    const double cotangent = cosine / sine;

    tmp[0] = cotangent / aspect;
    tmp[5] = cotangent;
    tmp[10] = -(zFar + zNear) / deltaZ;
    tmp[11] = -1;
    tmp[14] = -2 * zNear * zFar / deltaZ;
    tmp[15] = 0;

    memcpy(m, tmp, sizeof(tmp));
}