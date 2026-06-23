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

#ifndef HHUOS_APPLICATION_TINYGL_LESSON7_H
#define HHUOS_APPLICATION_TINYGL_LESSON7_H

#include <stddef.h>

#include <util/math/Vector3.h>

#include "TinyGlDemo.h"

/// A terrain, consisting of a 2D-array of height values and the corresponding normals.
/// Terrains are read from heightmaps (i.e., monochrome bitmap files).
class Terrain {

public:
    /// Open the bitmap file at the given path, interpret it as a heightmap, and return the resulting terrain.
    /// The terrain instance lies on the heap, and the caller is responsible for deleting the instance.
    /// The given height parameter specifies the maximum height (i.e., a white pixel in the heightmap).
    static Terrain* loadTerrain(const char *path, float height);

    /// Destroy the terrain, freeing the memory occupied by the 2D-arrays for height values and normals.
    ~Terrain();

    /// Get the width of the terrain (same the pixel width of the heightmap).
    size_t getWidth() const {
        return width;
    }

    /// Get the length of the terrain (same as the pixel height of the heightmap).
    size_t getLength() const {
        return length;
    }

    /// Set the height value at the given (x, y) coordinate.
    /// The maximum (x, y) value can be calculated by (`getWidth()` - 1, `getLength()` - 1).
    void setHeight(const size_t x, const size_t y, const float height) {
        heights[x][y] = height;
        computedNormals = false;
    }

    /// Get the height value at the given (x, y) coordinate.
    /// The maximum (x, y) value can be calculated by (`getWidth()` - 1, `getLength()` - 1).
    float getHeight(const size_t x, const size_t y) const {
        return heights[x][y];
    }

    /// Get the normal for the given (x, y) coordinate.
    /// The maximum (x, y) value can be calculated by (`getWidth()` - 1, `getLength()` - 1).
    Util::Math::Vector3<float> getNormal(const size_t x, const size_t y) const {
        return normals[x][y];
    }

    /// Recalculate the normal values of the terrain.
    /// This has to be called manually after height values have been changed.
    void computeNormals();

private:

    Terrain(size_t width, size_t length);

    size_t width;
    size_t length;
    float **heights = nullptr;
    Util::Math::Vector3<float> **normals = nullptr;
    bool computedNormals = false;

    static constexpr float FALLOUT_RATIO = 0.5f;
};

/// TinyGL demo renders a terrain based on a heightmap.
/// It was created by following lesson 7 of the OpenGL Tutorial on https://videotutorialsrock.com
class Lesson7 : public TinyGlDemo {

public:
    /// Create a new demo instance.
    Lesson7() = default;

    /// Destroy the demo, freeing the memory occupied by the terrain.
    ~Lesson7() override {
        delete terrain;
    }

    /// Initialize the OpenGL viewport according to the given screen resolution.
    void initialize(uint16_t resX, uint16_t resY) override;

    /// Update the terrain's rotation angle.
    void update(float delta) override;

    /// Render the terrain.
    void renderFrame() const override;

private:

    Terrain *terrain = nullptr;
    GLfloat rotationAngle = 0.0f;
};

#endif
