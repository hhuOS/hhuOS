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
 * Based on https://videotutorialsrock.com/opengl_tutorial/textures/home.php
 */

#include "Lesson5.h"

#include <tinygl/include/GL/gl.h>

void Lesson5::initialize(const uint16_t resX, const uint16_t resY) {
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

    // Enable required OpenGL features
    glEnable(GL_DEPTH_TEST); // Depth testing to make sure the shapes are drawn in the correct order
    glEnable(GL_LIGHTING); // Enable lighting
    glEnable(GL_LIGHT0); // Enable light #0
    glEnable(GL_NORMALIZE); // Automatically normalize normals
    glEnable(GL_COLOR_MATERIAL); // Enable color

    // Load the texture
    textureId = glLoadTexture("/user/demo/vtr.bmp");
}

void Lesson5::renderFrame() const {
    // Clear information from the last draw
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    glMatrixMode(GL_MODELVIEW); // Switch to drawing perspective
    glLoadIdentity(); // Reset the drawing perspective

    glTranslatef(0.0f, 1.0f, -6.0f);

    // Add ambient light
    GLfloat ambientColor[] = { 0.2f, 0.2f, 0.2f, 1.0f };
    glLightModelfv(GL_LIGHT_MODEL_AMBIENT, ambientColor);

    // Add directed light
    GLfloat lightColor[] = { 0.7f, 0.7f, 0.7f, 1.0f };
    GLfloat lightPosition[] = { -10.0f, 15.0f, 20.0f, 1.0f };
    glLightfv(GL_LIGHT0, GL_DIFFUSE, lightColor);
    glLightfv(GL_LIGHT0, GL_POSITION, lightPosition);

    // Bind texture
    glEnable(GL_TEXTURE_2D);
    glBindTexture(GL_TEXTURE_2D, static_cast<GLint>(textureId));

    // Bottom
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
    glColor3f(1.0f, 0.2f, 0.2f); // Texture will be blended with this red color
    glBegin(GL_QUADS);

    glNormal3f(0.0f, 1.0f, 0.0f);
    glTexCoord2f(0.0f, 0.0f);
    glVertex3f(-2.5f, -2.5f, 2.5f);
    glTexCoord2f(1.0f, 0.0f);
    glVertex3f(2.5f, -2.5f, 2.5f);
    glTexCoord2f(1.0f, 1.0f);
    glVertex3f(2.5f, -2.5f, -2.5f);
    glTexCoord2f(0.0f, 1.0f);
    glVertex3f(-2.5f, -2.5f, -2.5f);

    glEnd();

    // Back
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    glColor3f(1.0f, 1.0f, 1.0f); // White, so that the texture will appear as it is
    glBegin(GL_TRIANGLES);

    glNormal3f(0.0f, 0.0f, 1.0f);
    glTexCoord2f(0.0f, 0.0f);
    glVertex3f(-2.5f, -2.5f, -2.5f);
    glTexCoord2f(5.0f, 5.0f);
    glVertex3f(0.0f, 2.5f, -2.5f);
    glTexCoord2f(10.0f, 0.0f);
    glVertex3f(2.5f, -2.5f, -2.5f);

    glEnd();

    // Left
    glDisable(GL_TEXTURE_2D); // No texture for this shape
    glColor3f(1.0f, 0.7f, 0.3f);
    glBegin(GL_QUADS);

    glNormal3f(1.0f, 0.0f, 0.0f);
    glVertex3f(-2.5f, -2.5f, 2.5f);
    glVertex3f(-2.5f, -2.5f, -2.5f);
    glVertex3f(-2.5f, 2.5f, -2.5f);
    glVertex3f(-2.5f, 2.5f, 2.5f);

    glEnd();
}
