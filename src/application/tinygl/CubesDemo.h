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

#ifndef HHUOS_APPLICATION_TINYGL_CUBESDEMO_H
#define HHUOS_APPLICATION_TINYGL_CUBESDEMO_H

#include <stdint.h>

#include "TinyGlDemo.h"

#include <util/collection/ArrayList.h>
#include <util/graphic/Image.h>
#include <util/math/Random.h>
#include <tinygl/include/GL/gl.h>

/// TinyGL demo that renders three rotating gear wheels, similar to the famous `glxgears` application.
class CubesDemo : public TinyGlDemo {

public:
    /// Create a new cubes demo instance.
    CubesDemo() = default;

    /// Destroy the demo. The default destructor is sufficient.
    ~CubesDemo() override = default;

    /// Initialize the OpenGL viewport according to the given screen resolution.
    /// Set up light, load the cube texture, and set up a display list for the cubes.
    void initialize(uint16_t resX, uint16_t resY) override;

    /// Update the cubes' rotation angles.
    void update(float delta) override;

    /// Render the cubes.
    void renderFrame() const override;

    /// Handle key events:
    /// - `+`: Add a cube.
    /// - `-`: Remove the last cube.
    void handleKeyEvent(const Util::Io::KeyEvent&) override;

private:

    struct Cube {

        Cube() = default;

        explicit Cube(GLuint texture);

        bool operator!=(const Cube &other) const {
            return displayList != other.displayList;
        }

        void update(float delta);

        void render() const;

        GLuint texture = 0;
        GLuint displayList = 0;

        GLfloat size = random.getRandomNumber<float>() * 10 + 1;
        GLfloat rotationSpeed = random.getRandomNumber<float>() * 100 + 10;
        GLfloat rotationAngle = 0;

        GLfloat coordinates[3] = {
            random.getRandomNumber<float>() * 50 - 25,
            random.getRandomNumber<float>() * 50 - 25,
            -(random.getRandomNumber<float>() * 100 + 20)
        };

        GLfloat rotationAxes[3] = {
            random.getRandomNumber<float>() * 200 - 100,
            random.getRandomNumber<float>() * 200 - 100,
            random.getRandomNumber<float>() * 200 - 100
        };
    };

    static GLuint loadTexture(const Util::Graphic::Image &image);

    static Util::Math::Random random;

    GLuint cubeTexture = 0;
    Util::ArrayList<Cube> cubes;

    GLfloat ambientLight[4] = {0.3f, 0.3f, 0.3f, 1.0f};

    GLfloat lightColor[4] = {0.7f, 0.7f, 0.7f, 1.0f};
    GLfloat lightPos[4] = {-20, 10, 40, 1.0f};
};

#endif
