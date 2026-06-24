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

#include "GearsDemo.h"

#include "PortableGlMath.h"

#include <util/math/Math.h>
#include <tinygl/include/GL/gl.h>

void GearsDemo::initialize(const uint16_t resX, const uint16_t resY) {
    glEnable(GL_CULL_FACE);
    glEnable(GL_DEPTH_TEST);

    GLenum smooth[3] = { PGL_SMOOTH3 };

    // Create the shader program
    const GLuint program = pglCreateProgram(vertexShader, fragmentShader, 3, smooth, GL_FALSE);

    // Enable the shaders
    glUseProgram(program);

    // set uniform pointer
    pglSetUniform(&uniforms);

    // These have to be set initially
    const auto aspect = static_cast<float>(resX) / static_cast<float>(resY);
    perspective(ProjectionMatrix, 60.0, aspect, 1.0, 1024.0);
    glViewport(0, 0, resX, resY);

    // make the gears
    gear1 = createGear(1.0, 4.0, 1.0, 20, 0.7);
    gear2 = createGear(0.5, 2.0, 2.0, 10, 0.7);
    gear3 = createGear(1.3, 2.0, 0.5, 10, 0.7);
}

void GearsDemo::update(const float delta) {
    rotationAngle += delta * 70;
    if (rotationAngle > 360.0f) {
        rotationAngle -= 360.0f;
    }
}

void GearsDemo::renderFrame() {
    constexpr GLfloat red[4] = { 0.8, 0.1, 0.0, 1.0 };
    constexpr GLfloat green[4] = { 0.0, 0.8, 0.2, 1.0 };
    constexpr GLfloat blue[4] = { 0.2, 0.2, 1.0, 1.0 };
    GLfloat transform[16];
    identity(transform);

    glClearColor(0.0, 0.0, 0.0, 0.0);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    // Translate and rotate the view
    translate(transform, 0, 0, -20);
    rotate(transform, 2 * Util::Math::PI_FLOAT * viewRotation[0] / 360.0f, 1, 0, 0);
    rotate(transform, 2 * Util::Math::PI_FLOAT * viewRotation[1] / 360.0f, 0, 1, 0);
    rotate(transform, 2 * Util::Math::PI_FLOAT * viewRotation[2] / 360.0f, 0, 0, 1);

    // Draw the gears
    renderGear(*gear1, transform, -3.0, -2.0, rotationAngle, red);
    renderGear(*gear2, transform, 3.1, -2.0, -2 * rotationAngle - 9.0f, green);
    renderGear(*gear3, transform, -3.1, 4.2, -2 * rotationAngle - 25.0f, blue);
}

GearsDemo::Gear* GearsDemo::createGear(const GLfloat innerRadius, const GLfloat outerRadius, const GLfloat width,
    const GLint teeth, const GLfloat tooth_depth)
{
    double s[5];
    double c[5];
    GLfloat normal[3];
    int cur_strip = 0;

    // Allocate memory for the gear
    auto *gear = new Gear();

    // Calculate the radii used in the gear
    const GLfloat r0 = innerRadius;
    const GLfloat r1 = outerRadius - tooth_depth / 2.0f;
    const GLfloat r2 = outerRadius + tooth_depth / 2.0f;

    const GLfloat da = 2.0f * Util::Math::PI_FLOAT / static_cast<float>(teeth) / 4.0f;

    // Allocate memory for the triangle strip information
    gear->numStrips = STRIPS_PER_TOOTH * teeth;
    gear->strips = static_cast<VertexStrip*>(calloc(gear->numStrips, sizeof(*gear->strips)));

    // Allocate memory for the vertices
    gear->vertices = static_cast<GearVertex*>(calloc(VERTICES_PER_TOOTH * teeth, sizeof(*gear->vertices)));
    GearVertex *v = gear->vertices;

    for (int i = 0; i < teeth; i++) {
        // Calculate necessary sin/cos for various angles

        const double sc_val = i * 2.0 * Util::Math::PI_DOUBLE / teeth;
        sincos(sc_val, &s[0], &c[0]);
        sincos(sc_val + da, &s[1], &c[1]);
        sincos(sc_val + da * 2, &s[2], &c[2]);
        sincos(sc_val + da * 3, &s[3], &c[3]);
        sincos(sc_val + da * 4, &s[4], &c[4]);

        // A set of macros for making the creation of the gears easier
#define GEAR_POINT(r, da) { static_cast<GLfloat>((r) * c[(da)]), static_cast<GLfloat>((r) * s[(da)]) }
#define SET_NORMAL(x, y, z) do { \
	normal[0] = (x); normal[1] = (y); normal[2] = (z); \
} while(0)

#define GEAR_VERT(v, point, sign) fillVertex((v), p[(point)].x, p[(point)].y, (sign) * width * 0.5, normal)

#define START_STRIP do { \
	gear->strips[cur_strip].first = v - gear->vertices; \
} while(0);

#define END_STRIP do { \
	int _tmp = (v - gear->vertices); \
	gear->strips[cur_strip].count = _tmp - gear->strips[cur_strip].first; \
	cur_strip++; \
} while (0)

#define QUAD_WITH_NORMAL(p1, p2) do { \
	SET_NORMAL((p[(p1)].y - p[(p2)].y), -(p[(p1)].x - p[(p2)].x), 0); \
	v = GEAR_VERT(v, (p1), -1); \
	v = GEAR_VERT(v, (p1), 1); \
	v = GEAR_VERT(v, (p2), -1); \
	v = GEAR_VERT(v, (p2), 1); \
} while(0)

        struct point {
            GLfloat x;
            GLfloat y;
        };

        // Create the 7 points (only x,y coords) used to draw a tooth
        struct point p[7] = {
            GEAR_POINT(r2, 1), // 0
            GEAR_POINT(r2, 2), // 1
            GEAR_POINT(r1, 0), // 2
            GEAR_POINT(r1, 3), // 3
            GEAR_POINT(r0, 0), // 4
            GEAR_POINT(r1, 4), // 5
            GEAR_POINT(r0, 4), // 6
        };

        // Front face
        START_STRIP;
        SET_NORMAL(0, 0, 1.0);
        v = GEAR_VERT(v, 0, +1);
        v = GEAR_VERT(v, 1, +1);
        v = GEAR_VERT(v, 2, +1);
        v = GEAR_VERT(v, 3, +1);
        v = GEAR_VERT(v, 4, +1);
        v = GEAR_VERT(v, 5, +1);
        v = GEAR_VERT(v, 6, +1);
        END_STRIP;

        // Inner face
        START_STRIP;
        QUAD_WITH_NORMAL(4, 6);
        END_STRIP;

        // Back face
        START_STRIP;
        SET_NORMAL(0, 0, -1.0);
        v = GEAR_VERT(v, 6, -1);
        v = GEAR_VERT(v, 5, -1);
        v = GEAR_VERT(v, 4, -1);
        v = GEAR_VERT(v, 3, -1);
        v = GEAR_VERT(v, 2, -1);
        v = GEAR_VERT(v, 1, -1);
        v = GEAR_VERT(v, 0, -1);
        END_STRIP;

        // Outer face
        START_STRIP;
        QUAD_WITH_NORMAL(0, 2);
        END_STRIP;

        START_STRIP;
        QUAD_WITH_NORMAL(1, 0);
        END_STRIP;

        START_STRIP;
        QUAD_WITH_NORMAL(3, 1);
        END_STRIP;

        START_STRIP;
        QUAD_WITH_NORMAL(5, 3);
        END_STRIP;
    }

    gear->numVertices = v - gear->vertices;

    // Store the vertices in a vertex buffer object (VBO)
    glGenBuffers(1, &gear->vbo);
    glBindBuffer(GL_ARRAY_BUFFER, gear->vbo);
    glBufferData(GL_ARRAY_BUFFER, gear->numVertices * sizeof(GearVertex), gear->vertices,
        GL_STATIC_DRAW);

    return gear;
}

GearsDemo::GearVertex* GearsDemo::fillVertex(GearVertex *v, const GLfloat x, const GLfloat y, const GLfloat z,
    const GLfloat n[3])
{
    v[0][0] = x;
    v[0][1] = y;
    v[0][2] = z;
    v[0][3] = n[0];
    v[0][4] = n[1];
    v[0][5] = n[2];

    return v + 1;
}

void GearsDemo::renderGear(const Gear &gear, const GLfloat *transform, const GLfloat x, const GLfloat y,
    const GLfloat angle, const GLfloat color[4])
{
    GLfloat modelView[16];
    GLfloat normalMatrix[16];
    GLfloat mvp[16];

    // Translate and rotate the gear
    memcpy(modelView, transform, sizeof (modelView));
    translate(modelView, x, y, 0);
    rotate(modelView, 2.0f * Util::Math::PI_FLOAT * angle / 360.0f, 0, 0, 1);

    // Create and set the ModelViewProjectionMatrix
    memcpy(mvp, ProjectionMatrix, sizeof(mvp));
    multiply(mvp, modelView);

    memcpy(&uniforms.mvpMat, mvp, sizeof(mat4));

    // Create and set the NormalMatrix. It's the inverse transpose of the ModelView matrix.
    memcpy(normalMatrix, modelView, sizeof (normalMatrix));
    invert(normalMatrix);
    transpose(normalMatrix);
    memcpy(&uniforms.normalMat, normalMatrix, sizeof(mat4));

    // Set the gear color, only copying rgb since alpha is 1
    memcpy(&uniforms.materialColor, color, sizeof(vec3));

    // Set the vertex buffer object to use
    glBindBuffer(GL_ARRAY_BUFFER, gear.vbo);

    // Set up the position of the attributes in the vertex buffer object
    // Note, not using the macro pglVertexAttribPointer here because this is a demo
    // of porting existing code with minimal changes.
    // I will never understand why Khronos decided an offset parameter should be a pointer.
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 6 * sizeof(GLfloat), nullptr);
    glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 6 * sizeof(GLfloat),
        reinterpret_cast<void*>(0 + 3 * sizeof(GLfloat)));

    // Enable the attributes
    glEnableVertexAttribArray(0);
    glEnableVertexAttribArray(1);

    // Draw the triangle strips that comprise the gear
    for (int n = 0; n < gear.numStrips; n++) {
        glDrawArrays(GL_TRIANGLE_STRIP, gear.strips[n].first, gear.strips[n].count);
    }

    // Disable the attributes
    glDisableVertexAttribArray(1);
    glDisableVertexAttribArray(0);
}

void GearsDemo::vertexShader(float *output, vec4 *vertexAttr, Shader_Builtins *builtins, void *uniforms) {
    // Convenience
    auto *vsOut = reinterpret_cast<vec3*>(output);
    auto *u = static_cast<Uniforms*>(uniforms);

    const vec4 v4 = mult_m4_v4(u->normalMat, vertexAttr[1]);
    const vec3 v3 = { v4.x, v4.y, v4.z };
    const vec3 N = norm_v3(v3);

    constexpr vec3 lightPos = { 5.0, 5.0, 10.0 };
    const vec3 L = norm_v3(lightPos);

    // Prevent double dot calc using macro
    const float tmp = dot_v3s(N, L);
    const float diffIntensity = MAX(tmp, 0.0);

    vsOut[0] = scale_v3(u->materialColor, diffIntensity);

    builtins->gl_Position = mult_m4_v4(u->mvpMat, vertexAttr[0]);
}

void GearsDemo::fragmentShader(float *input, Shader_Builtins *builtins, void*) {
    const vec3 color = reinterpret_cast<vec3*>(input)[0];

    builtins->gl_FragColor.x = color.x;
    builtins->gl_FragColor.y = color.y;
    builtins->gl_FragColor.z = color.z;
    builtins->gl_FragColor.w = 1;
}
