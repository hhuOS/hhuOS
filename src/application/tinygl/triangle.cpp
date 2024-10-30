/*
 * Copyright (C) 2018-2024 Heinrich-Heine-Universitaet Duesseldorf,
 * Institute of Computer Science, Department Operating Systems
 * Burak Akguel, Christian Gesse, Fabian Ruhland, Filip Krakowski, Michael Schoettner
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

const constexpr uint32_t TARGET_FRAME_RATE = 60;

static Util::Time::Timestamp fpsTimer;
static uint32_t fpsCounter = 0;
static uint32_t fps = 0;

static GLfloat rotationAngle = 0;

void drawTriangle() {
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    glMatrixMode(GL_MODELVIEW);

    glLoadIdentity();
    glRotatef(rotationAngle, 0, 1, 0);

    glBegin(GL_TRIANGLES);
    glColor3f(0.2, 0.2, 1.0); // Blue
    glVertex3f(0.8, -0.8, 0);

    glColor3f(0.2, 1.0, 0.2); // Green
    glVertex3f(-0.8, -0.8, 0);

    glColor3f(1.0, 0.2, 0.2); // Red
    glVertex3f(0, 0.8, 0);
    glEnd();
}

void triangle(const Util::Graphic::BufferedLinearFrameBuffer &lfb) {
    const auto targetFrameTime = Util::Time::Timestamp::ofMicroseconds(static_cast<uint64_t>(1000000.0 / TARGET_FRAME_RATE));
    Util::Io::File::setAccessMode(Util::Io::STANDARD_INPUT, Util::Io::File::NON_BLOCKING);

    // Create string drawer to draw FPS
    auto pixelDrawer = Util::Graphic::PixelDrawer(lfb);
    auto stringDrawer = Util::Graphic::StringDrawer(pixelDrawer);
    auto &font = Util::Graphic::Font::getFontForResolution(lfb.getResolutionY());

    // Initialize GL
    glClearColor(0.0, 0.0, 0.0, 0.0);
    glViewport(0, 0, lfb.getResolutionX(), lfb.getResolutionY());

    // Draw scene
    while (true) {
        auto startTime = Util::Time::getSystemTime();

        if (Util::System::in.read() > 0) {
            break;
        }

        // Draw the triangle
        drawTriangle();

        // Draw the FPS string on top of the rendered OpenGL scene
        stringDrawer.drawString(font, 0, 0, static_cast<const char*>(Util::String::format("FPS: %u", fps)), Util::Graphic::Colors::WHITE, Util::Graphic::Colors::INVISIBLE);
        lfb.flush(); // Flushes the buffered frame buffer to the screen

        auto renderTime = Util::Time::getSystemTime() - startTime;
        if (renderTime < targetFrameTime) {
            Util::Async::Thread::sleep(targetFrameTime - renderTime);
        }

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
}