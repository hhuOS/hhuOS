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
 * Based on https://videotutorialsrock.com/opengl_tutorial/terrain/home.php
 */

#include "Lesson7.h"

#include <util/graphic/BitmapFile.h>
#include <tinygl/include/GL/gl.h>

void Lesson7::initialize(const uint16_t resX, const uint16_t resY) {
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

    // Load terrain from a heightmap
    terrain = Terrain::loadTerrain("/user/demo/heightmap.bmp", 20);
}

void Lesson7::update(const float delta) {
    rotationAngle += delta * 40;
    if (rotationAngle > 360) {
        rotationAngle -= 360;
    }
}

void Lesson7::renderFrame() const {
    // Clear information from the last draw
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    glMatrixMode(GL_MODELVIEW); // Switch to drawing perspective
    glLoadIdentity(); // Reset the drawing perspective

    // Set translation and rotation for the terrain
    glTranslatef(0.0f, 0.0f, -10.0f);
    glRotatef(30, 1, 0, 0);
    glRotatef(rotationAngle, 0, 1, 0);

    // Add ambient light
    GLfloat ambientColor[] = { 0.4f, 0.4f, 0.4f, 1.0f };
    glLightModelfv(GL_LIGHT_MODEL_AMBIENT, ambientColor);

    // Add positioned light
    GLfloat lightColor[] = { 0.6f, 0.6f, 0.6f, 1.0f };
    GLfloat lightPosition[] = { -0.f, 0.8f, 0.1f, 1.0f };
    glLightfv(GL_LIGHT0, GL_DIFFUSE, lightColor);
    glLightfv(GL_LIGHT0, GL_POSITION, lightPosition);

    const auto scale = 5.0f / Util::Math::max(static_cast<float>(terrain->getWidth()) - 1.0f,
        static_cast<float>(terrain->getLength()) - 1.0f);
    glScalef(scale, scale, scale);
    glTranslatef(-(static_cast<float>(terrain->getWidth()) - 1.0f) / 2, 0.0f,
        -(static_cast<float>(terrain->getLength()) - 1.0f) / 2);

    glColor3f(0.3f, 0.9f, 0.0f);
    for (size_t z = 0; z < terrain->getLength() - 1; z++) {
        // Makes OpenGL draw a triangle at every three consecutive vertices
        glBegin(GL_TRIANGLE_STRIP);
        for (size_t x = 0; x < terrain->getWidth(); x++) {
            auto normal = terrain->getNormal(x, z);
            glNormal3f(normal.getX(), normal.getY(), normal.getZ());
            glVertex3f(static_cast<float>(x), terrain->getHeight(x, z), static_cast<float>(z));
            normal = terrain->getNormal(x, z + 1);
            glNormal3f(normal.getX(), normal.getY(), normal.getZ());
            glVertex3f(static_cast<float>(x), terrain->getHeight(x, z + 1), static_cast<float>(z + 1));
        }
        glEnd();
    }
}

Terrain::Terrain(const size_t width, const size_t length) : width(width), length(length) {
    heights = new float*[length];
    for (size_t i = 0; i < length; i++) {
        heights[i] = new float[width];
    }

    normals = new Util::Math::Vector3<float>*[length];
    for (size_t i = 0; i < length; i++) {
        normals[i] = new Util::Math::Vector3<float>[width];
    }
}

Terrain* Terrain::loadTerrain(const char *path, const float height) {
    const auto *image = Util::Graphic::BitmapFile::open(path);
    const auto *pixels = image->getPixelBuffer();
    auto *terrain = new Terrain(image->getWidth(), image->getHeight());

    for (uint16_t y = 0; y < image->getHeight(); y++) {
        for (uint16_t x = 0; x < image->getWidth(); x++) {
            // Get the color of the pixel at (x, y)
            auto color = pixels[y * image->getWidth() + x];
            // Image is black & white, so all color channels are the same
            const auto z = height * (static_cast<float>(color.getRed()) / 255.0f - 0.5f);
            // Set the height of the terrain at (x, y) to z
            terrain->setHeight(x, y, z);
        }
    }

    delete image;
    terrain->computeNormals();

    return terrain;
}

Terrain::~Terrain() {
    for (size_t i = 0; i < length; i++) {
        delete[] heights[i];
        delete[] normals[i];
    }

    delete[] heights;
    delete[] normals;
}

void Terrain::computeNormals() {
    auto **normals2 = new Util::Math::Vector3<float>*[length];
    for (size_t i = 0; i < length; i++) {
        normals2[i] = new Util::Math::Vector3<float>[width];
    }

    for (size_t z = 0; z < length; z++) {
        for (size_t x = 0; x < width; x++) {
            Util::Math::Vector3<float> sum(0.0f, 0.0f, 0.0f);

            Util::Math::Vector3<float> out;
            if (z > 0) {
                out = Util::Math::Vector3<float>(0.0f, heights[z - 1][x] - heights[z][x], -1.0f);
            }

            Util::Math::Vector3<float> in;
            if (z < length - 1) {
                in = Util::Math::Vector3<float>(0.0f, heights[z + 1][x] - heights[z][x], 1.0f);
            }

            Util::Math::Vector3<float> left;
            if (x > 0) {
                left = Util::Math::Vector3<float>(-1.0f, heights[z][x - 1] - heights[z][x], 0.0f);
            }

            Util::Math::Vector3<float> right;
            if (x < width - 1) {
                right = Util::Math::Vector3<float>(1.0f, heights[z][x + 1] - heights[z][x], 0.0f);
            }

            if (x > 0 && z > 0) {
                sum = sum + out.crossProduct(left).normalize();
            }

            if (x > 0 && z < length - 1) {
                sum = sum + left.crossProduct(in).normalize();
            }

            if (x < width - 1 && z < length - 1) {
                sum = sum + in.crossProduct(right).normalize();
            }

            if (x < width - 1 && z > 0) {
                sum = sum + right.crossProduct(out).normalize();
            }

            normals2[z][x] = sum;
        }
    }

    for (size_t z = 0; z < length; z++) {
        for (size_t x = 0; x < width; x++) {
            auto sum = normals2[z][x];

            if (x > 0) {
                sum = sum + normals2[z][x - 1] * FALLOUT_RATIO;
            }
            if (x < width - 1) {
                sum = sum + normals2[z][x + 1] * FALLOUT_RATIO;
            }
            if (z > 0) {
                sum = sum + normals2[z - 1][x] * FALLOUT_RATIO;
            }
            if (z < length - 1) {
                sum = sum + normals2[z + 1][x] * FALLOUT_RATIO;
            }

            if (sum.length() == 0) {
                sum = Util::Math::Vector3<float>(0.0f, 1.0f, 0.0f);
            }
            normals[z][x] = sum;
        }
    }

    for (size_t i = 0; i < length; i++) {
        delete[] normals2[i];
    }
    delete[] normals2;

    computedNormals = true;
}
