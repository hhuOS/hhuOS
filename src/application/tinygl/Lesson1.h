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
 * Based on https://videotutorialsrock.com/opengl_tutorial/basic_shapes/home.php
 */

#ifndef HHUOS_APPLICATION_TINYGL_LESSON1_H
#define HHUOS_APPLICATION_TINYGL_LESSON1_H

#include "TinyGlDemo.h"

/// TinyGL demo that renders static 2D shapes.
/// It was created by following lesson 1 of the OpenGL Tutorial on https://videotutorialsrock.com.
class Lesson1 : public TinyGlDemo {

public:
    /// Create a new demo instance.
    Lesson1() = default;

    /// Destroy the demo. The default destructor is sufficient.
    ~Lesson1() override = default;

    /// Initialize the OpenGL viewport according to the given screen resolution.
    void initialize(uint16_t resX, uint16_t resY) override;

    /// Do nothing, as the shapes are static and do not move.
    void update(float) override {}

    /// Render the shapes.
    void renderFrame() const override;
};

#endif
