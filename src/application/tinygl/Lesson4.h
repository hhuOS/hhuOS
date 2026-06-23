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
 * Based on https://videotutorialsrock.com/opengl_tutorial/lighting/home.php
 */

#ifndef HHUOS_APPLICATION_TINYGL_LESSON4_H
#define HHUOS_APPLICATION_TINYGL_LESSON4_H

#include "TinyGlDemo.h"

/// TinyGL demo that renders a rotating 3D box with lighting enabled.
/// It was created by following lesson 4 of the OpenGL Tutorial on https://videotutorialsrock.com.
class Lesson4 : public TinyGlDemo {

public:
    /// Create a new demo instance.
    Lesson4() = default;

    /// Destroy the demo. The default destructor is sufficient.
    ~Lesson4() override = default;

    /// Initialize the OpenGL viewport according to the given screen resolution.
    void initialize(uint16_t resX, uint16_t resY) override;

    /// Update the box's rotation angle.
    void update(float delta) override;

    /// Render the box.
    void renderFrame() const override;

private:

    GLfloat rotationAngle = 0.0f;
};

#endif
