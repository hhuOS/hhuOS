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
 * Based on https://videotutorialsrock.com/opengl_tutorial/cube/home.php
 */

#include <stddef.h>
#include <stdint.h>

#include "CubesDemo.h"

#include <util/base/String.h>
#include <util/collection/ArrayList.h>
#include <util/graphic/LinearFrameBuffer.h>
#include <util/graphic/BitmapFile.h>
#include <tinygl/include/GL/gl.h>

#include "pulsar/Game.h"

Util::Math::Random CubesDemo::random;

CubesDemo::Cube::Cube(const GLuint texture) : texture(texture) {
    displayList = glGenLists(1);
    glNewList(displayList, GL_COMPILE);

    glEnable(GL_TEXTURE_2D);
    glBindTexture(GL_TEXTURE_2D, texture);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    glColor3f(1.0f, 1.0f, 1.0f);

    glBegin(GL_QUADS);

    //Top face
    glNormal3f(0.0, 1.0f, 0.0f);
    glVertex3f(-size / 2, size / 2, -size / 2);
    glTexCoord2f(0.0f, 0.0f);
    glVertex3f(-size / 2, size / 2, size / 2);
    glTexCoord2f(1.0f, 0.0f);
    glVertex3f(size / 2, size / 2, size / 2);
    glTexCoord2f(1.0f, 1.0f);
    glVertex3f(size / 2, size / 2, -size / 2);
    glTexCoord2f(0.0f, 1.0f);

    //Bottom face
    glNormal3f(0.0, -1.0f, 0.0f);
    glVertex3f(-size / 2, -size / 2, -size / 2);
    glTexCoord2f(0.0f, 0.0f);
    glVertex3f(size / 2, -size / 2, -size / 2);
    glTexCoord2f(1.0f, 0.0f);
    glVertex3f(size / 2, -size / 2, size / 2);
    glTexCoord2f(1.0f, 1.0f);
    glVertex3f(-size / 2, -size / 2, size / 2);
    glTexCoord2f(0.0f, 1.0f);

    //Left face
    glNormal3f(-1.0, 0.0f, 0.0f);
    glVertex3f(-size / 2, -size / 2, -size / 2);
    glTexCoord2f(0.0f, 0.0f);
    glVertex3f(-size / 2, -size / 2, size / 2);
    glTexCoord2f(1.0f, 0.0f);
    glVertex3f(-size / 2, size / 2, size / 2);
    glTexCoord2f(1.0f, 1.0f);
    glVertex3f(-size / 2, size / 2, -size / 2);
    glTexCoord2f(0.0f, 1.0f);

    //Right face
    glNormal3f(1.0, 0.0f, 0.0f);
    glVertex3f(size / 2, -size / 2, -size / 2);
    glTexCoord2f(0.0f, 0.0f);
    glVertex3f(size / 2, size / 2, -size / 2);
    glTexCoord2f(1.0f, 0.0f);
    glVertex3f(size / 2, size / 2, size / 2);
    glTexCoord2f(1.0f, 1.0f);
    glVertex3f(size / 2, -size / 2, size / 2);
    glTexCoord2f(0.0f, .0f);

    glEnd();

    glBegin(GL_QUADS);

    //Front face
    glNormal3f(0.0, 0.0f, 1.0f);
    glTexCoord2f(0.0f, 0.0f);
    glVertex3f(-size / 2, -size / 2, size / 2);
    glTexCoord2f(1.0f, 0.0f);
    glVertex3f(size / 2, -size / 2, size / 2);
    glTexCoord2f(1.0f, 1.0f);
    glVertex3f(size / 2, size / 2, size / 2);
    glTexCoord2f(0.0f, 1.0f);
    glVertex3f(-size / 2, size / 2, size / 2);

    //Back face
    glNormal3f(0.0, 0.0f, -1.0f);
    glTexCoord2f(0.0f, 0.0f);
    glVertex3f(-size / 2, -size / 2, -size / 2);
    glTexCoord2f(1.0f, 0.0f);
    glVertex3f(-size / 2, size / 2, -size / 2);
    glTexCoord2f(1.0f, 1.0f);
    glVertex3f(size / 2, size / 2, -size / 2);
    glTexCoord2f(0.0f, 1.0f);
    glVertex3f(size / 2, -size / 2, -size / 2);

    glEnd();
    glDisable(GL_TEXTURE_2D);

    glEndList();
}

void CubesDemo::Cube::update(const float delta) {
    rotationAngle += delta * rotationSpeed;
    if (rotationAngle > 360.0f) {
        rotationAngle -= 360.0f;
    }
}

void CubesDemo::Cube::render() const {
    glLoadIdentity();
    glTranslatef(coordinates[0], coordinates[1], coordinates[2]);
    glRotatef(rotationAngle, rotationAxes[0], rotationAxes[1], rotationAxes[2]);
    glCallList(displayList);
}

void CubesDemo::initialize(const uint16_t resX, const uint16_t resY) {
    // Initialize GL
    glClearColor(0.0, 0.0, 0.0, 0.0);
    glViewport(0, 0, resX, resY);

    glEnable(GL_DEPTH_TEST);
    glEnable(GL_LIGHTING);
    glEnable(GL_LIGHT0);
    glEnable(GL_NORMALIZE);
    glEnable(GL_COLOR_MATERIAL);

    const auto h = static_cast<GLfloat>(resY) / static_cast<GLfloat>(resX);
    glMatrixMode(GL_PROJECTION);
    glLoadIdentity();
    glFrustum(-1.0, 1.0, -h, h, 1.0, 200.0);
    glMatrixMode(GL_MODELVIEW);

    // Set up light
    glLightModelfv(GL_LIGHT_MODEL_AMBIENT, ambientLight);
    glLightfv(GL_LIGHT0, GL_DIFFUSE, lightColor);
    glLightfv(GL_LIGHT0, GL_POSITION, lightPos);
    
    // Load texture
    cubeTexture = glLoadTexture("/user/dino/block/box.bmp");

    // Create cubes
    for (size_t i = 0; i < 5; i++) {
        cubes.add(Cube(cubeTexture));
    }
}

void CubesDemo::update(const float delta) {
    for (auto &cube : cubes) {
        cube.update(delta);
    }
}

void CubesDemo::renderFrame() const {
    // Draw the scene
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    glMatrixMode(GL_MODELVIEW);
    glLoadIdentity();

    for (auto &cube : cubes) {
        cube.render();
    }

    const auto cubesString = Util::String::format("Cubes: %u", cubes.size());
    const auto *strBytes = reinterpret_cast<const GLubyte*>(static_cast<const char*>(cubesString));
    glDrawText(strBytes, 0, 16, Util::Graphic::Colors::WHITE.getRGB32());
}

void CubesDemo::handleKeyEvent(const Util::Io::KeyEvent &keyEvent) {
    if (!keyEvent.isPressed()) {
        return;
    }

    switch (keyEvent.getScancode()) {
        case Util::Io::KeyEvent::PLUS:
        case Util::Io::KeyEvent::NUM_PLUS:
            cubes.add(Cube(cubeTexture));
            break;
        case Util::Io::KeyEvent::MINUS:
        case Util::Io::KeyEvent::NUM_MINUS:
            if (cubes.size() > 0) {
                cubes.removeIndex(cubes.size() - 1);
            }
            break;
        default:
            break;
    }
}
