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
 * Based on the TinyGL gears demo (https://github.com/C-Chads/tinygl/blob/main/Raw_Demos/gears.c)
 */

#include <stdint.h>

#include "GearsDemo.h"

#include <util/math/Math.h>
#include <tinygl/include/GL/gl.h>

void GearsDemo::initialize(const uint16_t resX, const uint16_t resY) {
    // Initialize GL
    glClearColor(0.0, 0.0, 0.0, 0.0);
    glViewport(0, 0, resX, resY);

    glEnable(GL_LIGHTING);
    glBlendEquation(GL_FUNC_ADD);

    glEnable(GL_DEPTH_TEST);
    glDisable(GL_BLEND);
    glDepthMask(GL_TRUE);

    const auto h = static_cast<GLfloat>(resY) / static_cast<GLfloat>(resX);
    glMatrixMode(GL_PROJECTION);
    glLoadIdentity();
    glFrustum(-1.0, 1.0, -h, h, 5.0, 60.0);
    glMatrixMode(GL_MODELVIEW);
    glLoadIdentity();
    glTranslatef(0.0, 0.0, -45.0);

    // Light at infinity
    glLightfv(GL_LIGHT0, GL_POSITION, lightPos);
    glLightfv(GL_LIGHT0, GL_DIFFUSE, white);
    glLightfv(GL_LIGHT0, GL_SPECULAR, white);
    glEnable(GL_CULL_FACE);
    glEnable(GL_LIGHT0);

    // Create the gears
    gears = glGenLists(3);

    // The largest gear
    glNewList(gears, GL_COMPILE);
    glMaterialfv(GL_FRONT, GL_DIFFUSE, red);
    glMaterialfv(GL_FRONT, GL_SPECULAR, white);
    glMaterialfv(GL_FRONT, GL_SHININESS, &shininess);
    glColor3fv(blue);
    gear(1.0, 4.0, 1.0, 20, 0.7);
    glEndList();

    // The small gear with the smaller hole, to the right
    glNewList(gears + 1, GL_COMPILE);
    glMaterialfv(GL_FRONT, GL_DIFFUSE, green);
    glMaterialfv(GL_FRONT, GL_SPECULAR, white);
    glColor3fv(red);
    gear(0.5, 2.0, 2.0, 10, 0.7);
    glEndList();

    // The small gear above with the large hole
    glNewList(gears + 2, GL_COMPILE);
    glMaterialfv(GL_FRONT, GL_DIFFUSE, blue);
    glMaterialfv(GL_FRONT, GL_SPECULAR, white);
    glColor3fv(green);
    gear(1.3, 2.0, 0.5, 10, 0.7);
    glEndList();
}

void GearsDemo::update(const float delta) {
    rotationAngle += delta * 75;
    if (rotationAngle > 360.0f) {
        rotationAngle -= 360.0f;
    }
}

void GearsDemo::renderFrame() const {
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    glPushMatrix();
    glRotatef(VIEW_ROTATION_X, 1.0, 0.0, 0.0);
    glRotatef(VIEW_ROTATION_Y, 0.0, 1.0, 0.0);

    glPushMatrix();
    glTranslatef(-3.0, -2.0, 0.0);
    glRotatef(rotationAngle, 0.0, 0.0, 1.0);
    glCallList(gears);
    glPopMatrix();

    glPushMatrix();
    glTranslatef(3.1, -2.0, 0.0);
    glRotatef(-2.0f * rotationAngle - 9.0f, 0.0, 0.0, 1.0);
    glCallList(gears + 1);
    glPopMatrix();

    glPushMatrix();
    glTranslatef(-3.1, 4.2, 0.0);
    glRotatef(-2.0f * rotationAngle - 25.0f, 0.0, 0.0, 1.0);
    glCallList(gears + 2);
    glPopMatrix();

    glPopMatrix();
}

void GearsDemo::gear(const GLfloat innerRadius, const GLfloat outerRadius, const GLfloat width, const GLint teeth,
    const GLfloat toothDepth)
{
    GLint i;
    GLfloat angle;

    const auto r0 = innerRadius;
    const auto r1 = outerRadius - toothDepth / 2.0f;
    const auto r2 = outerRadius + toothDepth / 2.0f;
    const auto teethFloat = static_cast<float>(teeth);

    GLfloat da = 2.0f * Util::Math::PI_FLOAT / teethFloat / 4.0f;

    glNormal3f(0.0, 0.0, 1.0);

    glBegin(GL_QUAD_STRIP);
    for (i = 0; i <= teeth; i++) {
        angle = static_cast<float>(i) * 2.0f * Util::Math::PI_FLOAT / teethFloat;
        glVertex3f(r0 * Util::Math::cosine(angle), r0 * Util::Math::sine(angle), width * 0.5f);
        glVertex3f(r1 * Util::Math::cosine(angle), r1 * Util::Math::sine(angle), width * 0.5f);
        glVertex3f(r0 * Util::Math::cosine(angle), r0 * Util::Math::sine(angle), width * 0.5f);
        glVertex3f(r1 * Util::Math::cosine(angle + 3 * da), r1 * Util::Math::sine(angle + 3 * da),
            width * 0.5f);
    }
    glEnd();

    glBegin(GL_QUADS);
    da = 2.0f * Util::Math::PI_FLOAT / teethFloat / 4.0f;
    for (i = 0; i < teeth; i++) {
        angle = static_cast<float>(i) * 2.0f * Util::Math::PI_FLOAT / teethFloat;

        glVertex3f(r1 * Util::Math::cosine(angle), r1 * Util::Math::sine(angle), width * 0.5f);
        glVertex3f(r2 * Util::Math::cosine(angle + da), r2 * Util::Math::sine(angle + da), width * 0.5f);
        glVertex3f(r2 * Util::Math::cosine(angle + 2 * da), r2 * Util::Math::sine(angle + 2 * da),
            width * 0.5f);
        glVertex3f(r1 * Util::Math::cosine(angle + 3 * da), r1 * Util::Math::sine(angle + 3 * da),
            width * 0.5f);
    }
    glEnd();

    glNormal3f(0.0, 0.0, -1.0);

    glBegin(GL_QUAD_STRIP);
    for (i = 0; i <= teeth; i++) {
        angle = static_cast<float>(i) * 2.0f * Util::Math::PI_FLOAT / teethFloat;
        glVertex3f(r1 * Util::Math::cosine(angle), r1 * Util::Math::sine(angle), -width * 0.5f);
        glVertex3f(r0 * Util::Math::cosine(angle), r0 * Util::Math::sine(angle), -width * 0.5f);
        glVertex3f(r1 * Util::Math::cosine(angle + 3 * da), r1 * Util::Math::sine(angle + 3 * da),
            -width * 0.5f);
        glVertex3f(r0 * Util::Math::cosine(angle), r0 * Util::Math::sine(angle), -width * 0.5f);
    }
    glEnd();

    glBegin(GL_QUADS);
    da = 2.0f * Util::Math::PI_FLOAT / teethFloat / 4.0f;
    for (i = 0; i < teeth; i++) {
        angle = static_cast<float>(i) * 2.0f * Util::Math::PI_FLOAT / teethFloat;

        glVertex3f(r1 * Util::Math::cosine(angle + 3 * da), r1 * Util::Math::sine(angle + 3 * da),
            -width * 0.5f);
        glVertex3f(r2 * Util::Math::cosine(angle + 2 * da), r2 * Util::Math::sine(angle + 2 * da),
            -width * 0.5f);
        glVertex3f(r2 * Util::Math::cosine(angle + da), r2 * Util::Math::sine(angle + da), -width * 0.5f);
        glVertex3f(r1 * Util::Math::cosine(angle), r1 * Util::Math::sine(angle), -width * 0.5f);
    }
    glEnd();

    glBegin(GL_QUAD_STRIP);
    for (i = 0; i < teeth; i++) {
        angle = static_cast<float>(i) * 2.0f * Util::Math::PI_FLOAT / teethFloat;

        glVertex3f(r1 * Util::Math::cosine(angle), r1 * Util::Math::sine(angle), width * 0.5f);
        glVertex3f(r1 * Util::Math::cosine(angle), r1 * Util::Math::sine(angle), -width * 0.5f);
        auto u = r2 * Util::Math::cosine(angle + da) - r1 * Util::Math::cosine(angle);
        auto v = r2 * Util::Math::sine(angle + da) - r1 * Util::Math::sine(angle);
        const GLfloat len = Util::Math::sqrt(u * u + v * v);
        u /= len;
        v /= len;
        glNormal3f(v, -u, 0.0);
        glVertex3f(r2 * Util::Math::cosine(angle + da), r2 * Util::Math::sine(angle + da), width * 0.5f);
        glVertex3f(r2 * Util::Math::cosine(angle + da), r2 * Util::Math::sine(angle + da), -width * 0.5f);
        glNormal3f(Util::Math::cosine(angle), Util::Math::sine(angle), 0.0);
        glVertex3f(r2 * Util::Math::cosine(angle + 2 * da), r2 * Util::Math::sine(angle + 2 * da),
            width * 0.5f);
        glVertex3f(r2 * Util::Math::cosine(angle + 2 * da), r2 * Util::Math::sine(angle + 2 * da),
            -width * 0.5f);
        u = r1 * Util::Math::cosine(angle + 3 * da) - r2 * Util::Math::cosine(angle + 2 * da);
        v = r1 * Util::Math::sine(angle + 3 * da) - r2 * Util::Math::sine(angle + 2 * da);
        glNormal3f(v, -u, 0.0);
        glVertex3f(r1 * Util::Math::cosine(angle + 3 * da), r1 * Util::Math::sine(angle + 3 * da),
            width * 0.5f);
        glVertex3f(r1 * Util::Math::cosine(angle + 3 * da), r1 * Util::Math::sine(angle + 3 * da),
            -width * 0.5f);
        glNormal3f(Util::Math::cosine(angle), Util::Math::sine(angle), 0.0);
    }

    glVertex3f(r1 * Util::Math::cosine(0.0f), r1 * Util::Math::sine(0.0f), width * 0.5f);
    glVertex3f(r1 * Util::Math::cosine(0.0f), r1 * Util::Math::sine(0.0f), -width * 0.5f);

    glEnd();

    glBegin(GL_QUAD_STRIP);
    for (i = 0; i <= teeth; i++) {
        angle = static_cast<float>(i) * 2.0f * Util::Math::PI_FLOAT / teethFloat;
        glNormal3f(-Util::Math::cosine(angle), -Util::Math::sine(angle), 0.0);
        glVertex3f(r0 * Util::Math::cosine(angle), r0 * Util::Math::sine(angle), -width * 0.5f);
        glVertex3f(r0 * Util::Math::cosine(angle), r0 * Util::Math::sine(angle), width * 0.5f);
    }
    glEnd();
}