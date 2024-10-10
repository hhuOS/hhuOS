/*
 * Lesson 1: Basic Shapes (https://videotutorialsrock.com/opengl_tutorial/basic_shapes/home.php)
 *
 * Permission is hereby granted, free of charge, to any person obtaining a copy
 * of this software and associated documentation files (the "Software"), to deal
 * in the Software without restriction, including without limitation the rights
 * to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
 * copies of the Software, and to permit persons to whom the Software is
 * furnished to do so, subject to the following conditions:
 *
 * The above notice and this permission notice shall be included in all copies
 * or substantial portions of the Software.
 *
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 * IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
 * FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
 * AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
 * LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
 * OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
 * SOFTWARE.
 *
 * Ported to hhuOS by Fabian Ruhland, 09.10.2024
 */

#include <stdint.h>
#include <GL/gl.h>

#include "lib/util/graphic/LinearFrameBuffer.h"
#include "lib/util/async/Thread.h"
#include "lib/util/time/Timestamp.h"
#include "lib/util/io/file/File.h"
#include "lib/util/base/System.h"
#include "lib/util/graphic/PixelDrawer.h"
#include "lib/util/graphic/StringDrawer.h"
#include "lib/util/graphic/font/Terminal8x8.h"
#include "lib/util/graphic/Colors.h"
#include "lib/util/base/String.h"
#include "lib/util/io/stream/InputStream.h"
#include "lib/util/graphic/BufferedLinearFrameBuffer.h"
#include "lib/util/io/key/Key.h"
#include "lib/util/async/Process.h"
#include "lib/util/io/key/KeyDecoder.h"
#include "lib/util/io/key/layout/DeLayout.h"
#include "application/tinygl/util.h"

/**
 * The target frame rate of the application.
 * If the application achieves this frame rate, it will sleep for the remaining time of the frame.
 */
const constexpr uint32_t TARGET_FRAME_RATE = 60;

/**
 * Called before the main loop to initialize the OpenGL scene.
 */
static void initRendering(const Util::Graphic::BufferedLinearFrameBuffer &lfb) {
    const auto width = static_cast<GLdouble>(lfb.getResolutionX());
    const auto height = static_cast<GLdouble>(lfb.getResolutionY());

    // Set clear color to black
    glClearColor(0.0f, 0.0f, 0.0f, 1.0f);

    // Set up the view port to match the resolution of the frame buffer
    glViewport(0, 0, lfb.getResolutionX(), lfb.getResolutionY());

    // Set the camera perspective
    glMatrixMode(GL_PROJECTION);
    glLoadIdentity();
    gluPerspective(45.0,             // Camera angle
                   width / height,   // Width to height ratio
                   1.0,              // Near z-clipping coordinate (Object nearer than that will not be drawn)
                   200.0);           // Far z-clipping coordinate (Object farther than that will not be drawn)

    // Enable required OpenGL features
    glEnable(GL_DEPTH_TEST); // Depth testing to make sure the shapes are drawn in the correct order
}

/**
 * Called by the main loop before updating the scene, when a key is pressed.
 */
static void handleKeypress(const Util::Io::Key &key) {
    switch (key.getScancode()) {
        case Util::Io::Key::ESC:
            Util::Async::Process::exit(0);
    }
}

/**
 * Called in each iteration of the main loop to draw the scene.
 */
static void drawScene() {
    // Clear information from the last draw
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    glMatrixMode(GL_MODELVIEW); // Switch to drawing perspective
    glLoadIdentity(); // Reset the drawing perspective

    // Trapezoid
    glBegin(GL_QUADS); // Begin quadrilateral coordinates

    glVertex3f(-0.7f, -1.5f, -5.0f);
    glVertex3f(0.7f, -1.5f, -5.0f);
    glVertex3f(0.4f, -0.5f, -5.0f);
    glVertex3f(-0.4f, -0.5f, -5.0f);

    glEnd(); // End quadrilateral coordinates

    // Pentagon (Consisting of three triangles)
    glBegin(GL_TRIANGLES); // Begin triangle coordinates

    glVertex3f(0.5f, 0.5f, -5.0f);
    glVertex3f(1.5f, 0.5f, -5.0f);
    glVertex3f(0.5f, 1.0f, -5.0f);

    glVertex3f(0.5f, 1.0f, -5.0f);
    glVertex3f(1.5f, 0.5f, -5.0f);
    glVertex3f(1.5f, 1.0f, -5.0f);

    glVertex3f(0.5f, 1.0f, -5.0f);
    glVertex3f(1.5f, 1.0f, -5.0f);
    glVertex3f(1.0f, 1.5f, -5.0f);

    //Triangle
    glVertex3f(-0.5f, 0.5f, -5.0f);
    glVertex3f(-1.0f, 1.5f, -5.0f);
    glVertex3f(-1.5f, 0.5f, -5.0f);

    glEnd(); // End triangle coordinates
}

/**
 * Lesson 1: Basic Shapes (https://videotutorialsrock.com/opengl_tutorial/basic_shapes/home.php)
 */
void lesson1(const Util::Graphic::BufferedLinearFrameBuffer &lfb) {
    const auto targetFrameTime = Util::Time::Timestamp::ofMicroseconds(static_cast<uint64_t>(1000000.0 / TARGET_FRAME_RATE));
    Util::Io::File::setAccessMode(Util::Io::STANDARD_INPUT, Util::Io::File::NON_BLOCKING);

    // Create string drawer to draw FPS
    auto pixelDrawer = Util::Graphic::PixelDrawer(lfb);
    auto stringDrawer = Util::Graphic::StringDrawer(pixelDrawer);

    // Variable needed for FPS calculation
    Util::Time::Timestamp fpsTimer;
    uint32_t fpsCounter = 0;
    uint32_t fps = 0;

    // Needed to parse key presses
    auto keyDecoder = new Util::Io::KeyDecoder(new Util::Io::DeLayout());

    // Initialize GL
    initRendering(lfb);

    // Draw scene
    while (true) {
        // Get current time at the start of drawing the current frame (needed for FPS calculation)
        auto startTime = Util::Time::getSystemTime();

        // Check for key presses
        auto c = Util::System::in.read();
        if (c > 0 && keyDecoder->parseScancode(c)) {
            auto key = keyDecoder->getCurrentKey();
            if (key.isPressed()) {
                handleKeypress(key);
            }
        }

        // Draw the scene
        drawScene();

        // Draw the FPS string on top of the rendered OpenGL scene
        stringDrawer.drawString(Util::Graphic::Fonts::TERMINAL_8x8, 0, 0, static_cast<const char*>(Util::String::format("FPS: %u", fps)), Util::Graphic::Colors::WHITE, Util::Graphic::Colors::INVISIBLE);
        lfb.flush(); // Flushes the buffered frame buffer to the screen

        // Calculate the time the rendering took and sleep if necessary to achieve the target frame rate
        auto renderTime = Util::Time::getSystemTime() - startTime;
        if (renderTime < targetFrameTime) {
            Util::Async::Thread::sleep(targetFrameTime - renderTime);
        }

        // Calculate the FPS
        fpsCounter++;
        auto frameTime = Util::Time::getSystemTime() - startTime;
        fpsTimer += frameTime;

        if (fpsTimer >= Util::Time::Timestamp::ofSeconds(1)) {
            fps = fpsCounter;
            fpsCounter = 0;
            fpsTimer.reset();
        }
    }
}