/*
 * Copyright (C) 2018-2024 Heinrich-Heine-Universitaet Duesseldorf,
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

#ifndef HHUOS_TINYGL_H
#define HHUOS_TINYGL_H

#include <GL/gl.h>

/**
 * Taken from https://stackoverflow.com/questions/12943164/replacement-for-gluperspective-with-glfrustrum
 */
void gluPerspective(GLdouble fovY, GLdouble aspect, GLdouble zNear, GLdouble zFar);

/**
 * Loads a texture from a bitmap file and returns the texture ID.
 */
GLuint gluLoadTexture(const char *path);

#endif