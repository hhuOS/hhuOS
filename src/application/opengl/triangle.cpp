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

#include "lib/tinygl/include/GL/gl.h"
#include "lib/tinygl/include/zbuffer.h"
#include "lib/util/base/Address.h"
#include "lib/util/graphic/LinearFrameBuffer.h"
#include "lib/util/async/Thread.h"
#include "lib/util/time/Timestamp.h"
#include "lib/util/graphic/Colors.h"
#include "lib/util/io/file/File.h"
#include "lib/util/base/System.h"

const uint32_t TARGET_FRAME_RATE = 60;

Util::Time::Timestamp passedTime{};
uint32_t fps = 0;

void draw() {
    auto rotationAngle = (static_cast<float>(passedTime.toMilliseconds()) / 1000) * 40;

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

    auto infoString = Util::String::format("OpenGL Version:  %s\nOpenGL Vendor:   %s\nOpenGL Renderer: %s\nFPS: %u", glGetString(GL_VERSION), glGetString(GL_VENDOR), glGetString(GL_RENDERER), fps);
    glDrawText(reinterpret_cast<const GLubyte*>(static_cast<const char*>(infoString)), 0, 0, Util::Graphic::Colors::WHITE.getRGB32());
}

void triangle(const Util::Graphic::LinearFrameBuffer &lfb) {
    const auto targetFrameTime = Util::Time::Timestamp::ofMicroseconds(static_cast<uint64_t>(1000000.0 / TARGET_FRAME_RATE));
    Util::Io::File::setAccessMode(Util::Io::STANDARD_INPUT, Util::Io::File::NON_BLOCKING);

    // Initialize frame buffer
    auto* frameBuffer = ZB_open(lfb.getResolutionX(), lfb.getResolutionY(), ZB_MODE_RGBA, nullptr);
    glInit(frameBuffer);

    // initialize GL
    glClearColor(0.0, 0.0, 0.0, 0.0);
    glViewport(0, 0, lfb.getResolutionX(), lfb.getResolutionY());

    // Draw scene
    while (true) {
        auto startTime = Util::Time::getSystemTime();

        if (Util::System::in.read() > 0) {
            break;
        }

        draw();
        ZB_copyFrameBuffer(frameBuffer, reinterpret_cast<void*>(lfb.getBuffer().get()), lfb.getPitch());

        auto frameTime = Util::Time::getSystemTime() - startTime;
        if (frameTime < targetFrameTime) {
            Util::Async::Thread::sleep(targetFrameTime - frameTime);
        }

        auto endTime = Util::Time::getSystemTime();
        passedTime += endTime - startTime;
        fps = 1000000 / (endTime.toMicroseconds() - startTime.toMicroseconds());
    }
}