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

#ifndef HHUOS_APPLICATION_PORTABLEGL_PORTABLEGLMATH_H
#define HHUOS_APPLICATION_PORTABLEGL_PORTABLEGLMATH_H

#include <portablegl/portablegl.h>

/// Calculate sine and cosine of `x` simultaneously.
void sincos(double x, double *s, double *c);

/// Multiply two 4x4 matrices.
/// THe result is stored in matrix m.
void multiply(GLfloat *m, const GLfloat *n);

/// Rotate a 4x4 matrix.
/// The given matrix `m` is directly overwritten with the new values.
void rotate(GLfloat *m, GLfloat angle, GLfloat x, GLfloat y, GLfloat z);

/// Translate a 4x4 matrix.
/// The given matrix `m` is directly overwritten with the new values.
void translate(GLfloat *m, GLfloat x, GLfloat y, GLfloat z);

/// Create an identity 4x4 matrix in `m`.
void identity(GLfloat *m);

/// Transpose a 4x4 matrix.
/// The given matrix `m` is directly overwritten with the new values.
void transpose(GLfloat *m);

/// Invert a 4x4 matrix.
/// The given matrix `m` is directly overwritten with the new values.
///
/// This function can currently handle only pure translation-rotation matrices.
/// Read http://www.gamedev.net/community/forums/topic.asp?topic_id=425118 for an explanation.
void invert(GLfloat *m);

/// Calculate a perspective projection transformation in `m`.
void perspective(GLfloat *m, GLfloat fovy, GLfloat aspect, GLfloat zNear, GLfloat zFar);

#endif
