/*
 * Based on the PortableGL triangle example (https://github.com/rswinkle/PortableGL/blob/master/examples/ex3.cpp) and the Shaders tutorial from Lean OpenGL (https://learnopengl.com/Getting-started/Shaders).
 */

#include <assert.h>

#define PGL_ASSERT(x) assert(x)
#include "lib/portablegl/portablegl.h"

#include "lib/util/graphic/LinearFrameBuffer.h"
#include "lib/util/base/Address.h"
#include "lib/util/base/System.h"
#include "lib/util/time/Timestamp.h"
#include "lib/util/io/file/File.h"
#include "lib/util/graphic/PixelDrawer.h"
#include "lib/util/graphic/StringDrawer.h"
#include "lib/util/async/Thread.h"
#include "lib/util/graphic/Colors.h"
#include "lib/util/graphic/font/Terminal8x8.h"
#include "lib/util/graphic/BufferedLinearFrameBuffer.h"

const constexpr uint32_t TARGET_FRAME_RATE = 60;

static Util::Time::Timestamp fpsTimer;
static uint32_t fpsCounter = 0;
static uint32_t fps = 0;

static GLfloat rotationAngle = 0;

void vertexShader(float *output, vec4 *vertexAttributes, Shader_Builtins *builtins, void *uniforms) {
    reinterpret_cast<vec4*>(output)[0] = vertexAttributes[4]; // Color
    builtins->gl_Position = mult_mat4_vec4(static_cast<float*>(uniforms), vertexAttributes[0]);
}

void fragmentShader(float *input, Shader_Builtins *builtins, void *uniforms) {
    builtins->gl_FragColor = reinterpret_cast<vec4*>(input)[0];
}

void triangle(const Util::Graphic::BufferedLinearFrameBuffer &lfb) {
    const auto targetFrameTime = Util::Time::Timestamp::ofMicroseconds(static_cast<uint64_t>(1000000.0 / TARGET_FRAME_RATE));
    Util::Io::File::setAccessMode(Util::Io::STANDARD_INPUT, Util::Io::File::NON_BLOCKING);

    // Create string drawer to draw FPS
    auto pixelDrawer = Util::Graphic::PixelDrawer(lfb);
    auto stringDrawer = Util::Graphic::StringDrawer(pixelDrawer);
    auto &font = Util::Graphic::Font::getFontForResolution(lfb.getResolutionY());

    // Set up shaders
    GLenum smooth[4] = { PGL_SMOOTH4 };
    GLuint shaderProgram = pglCreateProgram(vertexShader, fragmentShader, 4, smooth, GL_FALSE);
    glUseProgram(shaderProgram);

    // Set up uniform matrix
    mat4 uniformMatrix = IDENTITY_MAT4();
    pglSetUniform(&uniformMatrix);

    // Set up vertex and color data for triangle
    float vertices[] = {
            0.8f, -0.8f, 0.0f,  // Bottom right
            -0.8f, -0.8f, 0.0f, // Bottom left
            0.0f,  0.8f, 0.0f   // Top
    };

    float color[] = {
            0.0, 0.0, 1.0, 1.0, // Bottom right
            0.0, 1.0, 0.0, 1.0, // Bottom left
            1.0, 0.0, 0.0, 1.0  // Top
    };

    // Create vertex and color buffer objects for the triangle
    GLuint triangleVbo, colorVbo;
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

    while (true) {
        auto startTime = Util::Time::getSystemTime();

        // Check for keyboard input and exit if any key is pressed
        if (Util::System::in.read() > 0) {
            break;
        }

        // Clear screen
        glClearColor(0.0f, 0.0f, 0.0f, 0.0f);
        glClear(GL_COLOR_BUFFER_BIT);

        // Create rotation matrix
        mat4 rotationMatrix = IDENTITY_MAT4();
        load_rotation_mat4(rotationMatrix, vec3(0.0f, 1.0f, 0.0f), DEG_TO_RAD(rotationAngle));

        mat4 rotatedUniformMatrix = IDENTITY_MAT4();
        mult_mat4_mat4(uniformMatrix, rotatedUniformMatrix, rotationMatrix);

        // Bind triangle vertex array object and draw triangle
        glDrawArrays(GL_TRIANGLES, 0, 3);

        // Draw FPS string
        stringDrawer.drawString(font, 0, 0, static_cast<const char*>(Util::String::format("FPS: %u", fps)), Util::Graphic::Colors::WHITE, Util::Graphic::Colors::INVISIBLE);

        // Flush double buffer to screen
        lfb.flush();

        // Measure frame time and sleep to achieve target frame rate
        auto renderTime = Util::Time::getSystemTime() - startTime;
        if (renderTime < targetFrameTime) {
            Util::Async::Thread::sleep(targetFrameTime - renderTime);
        }

        // Calculate FPS by incrementing fpsCounter and resetting it after one second
        fpsCounter++;
        auto frameTime = Util::Time::getSystemTime() - startTime;
        fpsTimer += frameTime;

        if (fpsTimer >= Util::Time::Timestamp::ofSeconds(1)) {
            fps = fpsCounter;
            fpsCounter = 0;
            fpsTimer.reset();
        }

        rotationAngle += (static_cast<GLfloat>(frameTime.toMicroseconds()) / 1000000) * 40;
    }

    glDeleteBuffers(1, &triangleVbo);
    glDeleteBuffers(1, &colorVbo);
    glDeleteProgram(shaderProgram);
}