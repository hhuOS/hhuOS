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
 */

#ifndef HHUOS_APPLICATION_PORTABLEGL_TRIANGLEDEMO_H
#define HHUOS_APPLICATION_PORTABLEGL_TRIANGLEDEMO_H

#include <stdint.h>

#include "PortableGlDemo.h"

#include <portablegl/portablegl.h>

/// TinyGL demo that renders a rotating triangle with color blending.
class TriangleDemo : public PortableGlDemo {

public:
    /// Create a new triangle demo instance.
    TriangleDemo() = default;

    /// Destroy the demo. The default destructor is sufficient.
    ~TriangleDemo() override = default;

    /// Set up shaders and VBOs for the triangle.
    void initialize(uint16_t, uint16_t) override;

    /// Update the triangle's rotation angle.
    void update(float delta) override;

    /// Render the triangle.
    void renderFrame() override;

private:

    static void vertexShader(float *output, vec4 *vertexAttr, Shader_Builtins *builtins, void *uniforms);

    static void fragmentShader(float *input, Shader_Builtins *builtins, void *uniforms);

    mat4 uniformMatrix = IDENTITY_M4();

    GLuint triangleVbo = 0;
    GLuint colorVbo = 0;

    GLfloat rotationAngle = 0.0f;
};

#endif
