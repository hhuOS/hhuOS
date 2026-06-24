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
 */

#include "TriangleDemo.h"

#include <tinygl/include/GL/gl.h>

void TriangleDemo::update(const float delta) {
    rotationAngle += delta * 40;
    if (rotationAngle > 360.0f) {
        rotationAngle -= 360.0f;
    }
}

void TriangleDemo::renderFrame() const {
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    glMatrixMode(GL_MODELVIEW);

    glLoadIdentity();
    glRotatef(rotationAngle, 0, 1, 0);

    glBegin(GL_TRIANGLES);
    glColor3f(0.2, 0.2, 1.0); // Blue
    glVertex3f(0.8, -0.8, 0);

    glColor3f(0.2, 1.0, 0.2); // Green
    glVertex3f(-0.8, -0.8, 0);

    glColor3f(1.0, 0.2, 0.2); // Red
    glVertex3f(0, 0.8, 0);
    glEnd();
}
