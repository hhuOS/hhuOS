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
 * Based on the PortableGL gears demo (https://github.com/rswinkle/PortableGL/blob/master/examples/classic/gears.c)
 */

#ifndef HHUOS_APPLICATION_PORTABLEGL_GEARSDEMO_H
#define HHUOS_APPLICATION_PORTABLEGL_GEARSDEMO_H

#include <stdint.h>

#include "PortableGlDemo.h"

#include <portablegl/portablegl.h>

/// TinyGL demo that renders a rotating triangle with color blending.
class GearsDemo : public PortableGlDemo {

public:
    /// Create a new triangle demo instance.
    GearsDemo() = default;

    /// Destroy the demo. The default destructor is sufficient.
    ~GearsDemo() override = default;

    /// Set up the three gears.
    void initialize(uint16_t resX, uint16_t resY) override;

    /// Update the gears' rotation angle.
    void update(float delta) override;

    /// Render the three gears.
    void renderFrame() override;

private:

    static constexpr GLint STRIPS_PER_TOOTH = 7;
    static constexpr GLint VERTICES_PER_TOOTH = 34;
    static constexpr GLint GEAR_VERTEX_STRIDE = 6;

    typedef GLfloat GearVertex[GEAR_VERTEX_STRIDE];

    struct VertexStrip {
        GLint first;
        GLint count;
    };

    struct Gear {
        GearVertex *vertices;
        GLint numVertices;
        VertexStrip *strips;
        GLint numStrips;
        GLuint vbo;
    };

    struct Uniforms {
        mat4 mvpMat;
        mat4 normalMat;
        vec3 materialColor;
    };

    static Gear* createGear(GLfloat innerRadius, GLfloat outerRadius, GLfloat width, GLint teeth, GLfloat tooth_depth);

    static GearVertex* fillVertex(GearVertex *v, GLfloat x, GLfloat y, GLfloat z, const GLfloat n[3]);

    static void vertexShader(float *output, vec4 *vertexAttr, Shader_Builtins *builtins, void *uniforms);

    static void fragmentShader(float *input, Shader_Builtins *builtins, void *uniforms);

    void renderGear(const Gear &gear, const GLfloat *transform, GLfloat x, GLfloat y, GLfloat angle,
        const GLfloat color[4]);

    Uniforms uniforms{};
    GLfloat ProjectionMatrix[16]{};
    GLfloat viewRotation[3] = { 20.0, 30.0, 0.0 };

    Gear *gear1 = nullptr;
    Gear *gear2 = nullptr;
    Gear *gear3 = nullptr;
    GLfloat rotationAngle = 0.0f;
};

#endif
