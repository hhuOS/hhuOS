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

#include "TriangleDemo.h"

#include <tinygl/include/GL/gl.h>

void TriangleDemo::initialize(uint16_t, uint16_t) {
    // Set up shaders
    GLenum smooth[4] = { PGL_SMOOTH4 };
    const GLuint shaderProgram = pglCreateProgram(vertexShader, fragmentShader, 4, smooth, GL_FALSE);
    glUseProgram(shaderProgram);

    // Set up uniform matrix
    pglSetUniform(&uniformMatrix);

    // Set up vertex and color data for triangle
    constexpr float vertices[9] = {
        0.8f, -0.8f, 0.0f,  // Bottom right
        -0.8f, -0.8f, 0.0f, // Bottom left
        0.0f,  0.8f, 0.0f   // Top
    };

    constexpr float color[12] = {
        0.0, 0.0, 1.0, 1.0, // Bottom right
        0.0, 1.0, 0.0, 1.0, // Bottom left
        1.0, 0.0, 0.0, 1.0  // Top
    };

    // Create vertex and color buffer objects for the triangle
    glGenBuffers(1, &triangleVbo);
    glBindBuffer(GL_ARRAY_BUFFER, triangleVbo);
    glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);
    glEnableVertexAttribArray(0);
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 0, nullptr);

    glGenBuffers(1, &colorVbo);
    glBindBuffer(GL_ARRAY_BUFFER, colorVbo);
    glBufferData(GL_ARRAY_BUFFER, sizeof(color), color, GL_STATIC_DRAW);
    glEnableVertexAttribArray(4);
    glVertexAttribPointer(4, 4, GL_FLOAT, GL_FALSE, 0, nullptr);
}

void TriangleDemo::update(const float delta) {
    rotationAngle += delta * 40;
}

void TriangleDemo::renderFrame() {
    // Clear screen
    glClearColor(0.0f, 0.0f, 0.0f, 0.0f);
    glClear(GL_COLOR_BUFFER_BIT);

    // Create rotation matrix
    mat4 rotationMatrix = IDENTITY_M4();
    load_rotation_m4(rotationMatrix, vec3{0.0f, 1.0f, 0.0f}, DEG_TO_RAD(rotationAngle));

    mat4 rotatedUniformMatrix = IDENTITY_M4();
    mult_m4_m4(uniformMatrix, rotatedUniformMatrix, rotationMatrix);

    // Bind triangle vertex array object and draw triangle
    glDrawArrays(GL_TRIANGLES, 0, 3);
}

void TriangleDemo::vertexShader(float *output, vec4 *vertexAttr, Shader_Builtins *builtins, void *uniforms) {
    reinterpret_cast<vec4*>(output)[0] = vertexAttr[4]; // Color
    builtins->gl_Position = mult_m4_v4(static_cast<float*>(uniforms), vertexAttr[0]);
}

void TriangleDemo::fragmentShader(float *input, Shader_Builtins *builtins, void*) {
    builtins->gl_FragColor = reinterpret_cast<vec4*>(input)[0];
}
