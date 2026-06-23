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
 * Based on https://videotutorialsrock.com/opengl_tutorial/color/home.php
 */

#include "Lesson3.h"

#include <tinygl/include/GL/gl.h>

void Lesson3::initialize(const uint16_t resX, const uint16_t resY) {
    // Set clear color to black
    glClearColor(0.0, 0.0, 0.0, 0.0);

    // Set up the view port to match the resolution of the frame buffer
    glViewport(0, 0, resX, resY);

    // Set the camera perspective
    glMatrixMode(GL_PROJECTION);
    glLoadIdentity();
    glPerspective(45.0, // Camera angle
        static_cast<double>(resX) / resY, // Width to height ratio
        1.0, // Near z-clipping coordinate (Object nearer than that will not be drawn)
        200.0); // Far z-clipping coordinate (Object farther than that will not be drawn)

    // Enable depth testing so that objects are occluded based on depth instead of drawing order
    glEnable(GL_DEPTH_TEST);
}

void Lesson3::update(const float delta) {
    rotationAngle += delta * 80;
    if (rotationAngle > 360) {
        rotationAngle -= 360;
    }
}

void Lesson3::renderFrame() const {
    // Clear information from the last draw
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    glMatrixMode(GL_MODELVIEW); // Switch to drawing perspective
    glLoadIdentity(); // Reset the drawing perspective

    // Set z-coordinate to -5.0 for all shapes
    glTranslatef(0.0f, 0.0f, -5.0f);

    // Trapezoid
    glPushMatrix();
    glTranslatef(0.0f, -1.0f, 0.0f); // Translate to center of trapezoid
    glRotatef(rotationAngle, 0.0f, 0.0f, 1.0f); // Rotate around the z-axis
    glBegin(GL_QUADS);

    glColor3f(0.5f, 0.0f, 0.8f);
    glVertex3f(-0.7f, -0.5f, 0.0f);
    glColor3f(0.0f, 0.9f, 0.0f);
    glVertex3f(0.7f, -0.5f, 0.0f);
    glColor3f(1.0f, 0.0f, 0.0f);
    glVertex3f(0.4f, 0.5f, 0.0f);
    glColor3f(0.0f, 0.65f, 0.65f);
    glVertex3f(-0.4f, 0.5f, 0.0f);

    glEnd();
    glPopMatrix();

    // Pentagon (Consisting of three triangles)
    glPushMatrix();
    glTranslatef(1.0f, 1.0f, 0.0f); // Translate to center of pentagon
    glRotatef(rotationAngle, 0.0f, 1.0f, 0.0f); // Rotate around the y-axis
    glBegin(GL_TRIANGLES);

    glColor3f(0.0f, 0.75f, 0.0f);

    glVertex3f(-0.5f, -0.5f, 0.0f);
    glVertex3f(0.5f, -0.5f, 0.0f);
    glVertex3f(-0.5f, 0.0f, 0.0f);

    glVertex3f(-0.5f, 0.0f, 0.0f);
    glVertex3f(0.5f, -0.5f, 0.0f);
    glVertex3f(0.5f, 0.0f, 0.0f);

    glVertex3f(-0.5f, 0.0f, 0.0f);
    glVertex3f(0.5f, 0.0f, 0.0f);
    glVertex3f(0.0f, 0.5f, 0.0f);

    glEnd();
    glPopMatrix();

    //Triangle
    glPushMatrix();
    glTranslatef(-1.0f, 1.0f, 0.0f);
    glRotatef(rotationAngle, 1.0f, 2.0f, 3.0f); // Rotate around all three axes
    glBegin(GL_TRIANGLES);

    glColor3f(1.0f, 0.7f, 0.0f);
    glVertex3f(0.5f, -0.5f, 0.0f);
    glColor3f(1.0f, 1.0f, 1.0f);
    glVertex3f(0.0f, 0.5f, 0.0f);
    glColor3f(0.0f, 0.0f, 1.0f);
    glVertex3f(-0.5f, -0.5f, 0.0f);

    glEnd();
    glPopMatrix();
}
