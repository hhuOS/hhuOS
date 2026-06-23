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


#ifndef HHUOS_APPLICATION_TINYGL_GEARSDEMO_H
#define HHUOS_APPLICATION_TINYGL_GEARSDEMO_H

#include "TinyGlDemo.h"

#include <tinygl/include/GL/gl.h>

/// TinyGL demo that renders three rotating gear wheels, similar to the famous `glxgears` application.
class GearsDemo : public TinyGlDemo {

public:
    /// Create a new gears demo instance.
    GearsDemo() = default;

    /// Destroy the demo. The default destructor is sufficient.
    ~GearsDemo() override = default;

    /// Initialize the OpenGL viewport according to the given screen resolution.
    /// Set up light and create display lists for the three gears.
    void initialize(uint16_t resX, uint16_t resY) override;

    /// Update the gears' rotation angle.
    void update(float delta) override;

    /// Render the three gears.
    void renderFrame() const override;

private:

    static void gear(GLfloat innerRadius, GLfloat outerRadius, GLfloat width, GLint teeth, GLfloat toothDepth);

    GLfloat rotationAngle = 0.0f;

    // Display lists for the three gears
    GLuint gears = 0;

    // Position of the global light
    GLfloat lightPos[4] = {5, 5, 10, 0.0};

    // Material parameters for the gears
    GLfloat red[4] = {1.0, 0.0, 0.0, 0.0};
    GLfloat green[4] = {0.0, 1.0, 0.0, 0.0};
    GLfloat blue[4] = {0.0, 0.0, 1.0, 0.0};
    GLfloat white[4] = {1.0, 1.0, 1.0, 0.0};
    GLfloat shininess = 5;

    static constexpr GLfloat VIEW_ROTATION_X = 20.0;
    static constexpr GLfloat VIEW_ROTATION_Y = 30.0;
};

#endif
