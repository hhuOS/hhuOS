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

#include <stddef.h>
#include <stdint.h>

#include "TinyGlDemo.h"
#include "TriangleDemo.h"
#include "GearsDemo.h"
#include "CubesDemo.h"
#include "Lesson1.h"
#include "Lesson2.h"
#include "Lesson3.h"
#include "Lesson4.h"
#include "Lesson5.h"
#include "Lesson6.h"
#include "Lesson7.h"

#include <util/base/Address.h>
#include <util/base/String.h>
#include <util/base/System.h>
#include <util/base/ArgumentParser.h>
#include <util/collection/Array.h>
#include <util/math/Math.h>
#include <util/collection/ArrayQueue.h>
#include <util/io/stream/PrintStream.h>
#include <util/graphic/LinearFrameBuffer.h>
#include <util/graphic/Ansi.h>
#include <util/graphic/BitmapFile.h>
#include <util/graphic/BufferedLinearFrameBuffer.h>
#include <util/graphic/font/Terminal8x8.h>
#include <pulsar/Statistics.h>
#include <kepler/Window.h>
#include <tinygl/include/zbuffer.h>

constexpr const char *HELP_TEXT =
#include "generated/README.md"
;

/// Frame rate that the application tries to run at.
constexpr size_t TARGET_FRAME_RATE = 60;
/// Time per frame if we want to hit the target frame rate.
constexpr auto TARGET_FRAME_TIME = Util::Time::Timestamp::ofSecondsFloat(1.0f / TARGET_FRAME_RATE);

/// The main render loop runs as long as this is true.
/// Set to false when the Escape key is pressed or on a window close request
bool isRunning = true;

/// Statistics object, used to measure the full frame time, update time, draw time, and idle time.
Pulsar::Statistics statistics;
/// Contains the last FPS and average times, calculated from `statistics`.
Pulsar::Statistics::Gather gatheredStatistics;
/// Timestamp used to count up to 1 second. Each time 1 second is reached, the `gatheredStatistics` object is updated.
Util::Time::Timestamp statisticsGatherTimer;

/// The font used to draw the statistics strings.
const auto &fpsFont = Util::Graphic::Fonts::TERMINAL_8x8;

/// Store key events from the event listener.
/// Keys are retrieved from this queue in the main render loop.
Util::ArrayQueue<Util::Io::KeyEvent> keyEvents(32);

/// Handle key events from the kepler window manager
class EventListener : public Kepler::EventListener {

public:
    /// Create a new event listener instance.
    EventListener() = default;

    /// Since the event listener has no state, the default destructor is sufficient.
    ~EventListener() override = default;

    /// Handle a key event by pushing it into the global `keyEvents` queue.
    void onKeyEvent(const Util::Io::KeyEvent &key) override {
        keyEvents.offer(key);
    }

    /// Exit the application by setting `isRunning` to false.
    void onCloseButtonPressed() override {
        isRunning = false;
    }
};

/// Print OpenGL information to standard out.
void info() {
    Util::System::out << Util::Graphic::Ansi::FOREGROUND_BRIGHT_BLUE << "GL Vendor: " << Util::Graphic::Ansi::RESET
        << reinterpret_cast<const char*>(glGetString(GL_VENDOR)) << Util::Io::PrintStream::ln
        << Util::Graphic::Ansi::FOREGROUND_BRIGHT_BLUE << "GL Renderer: " << Util::Graphic::Ansi::RESET
        << reinterpret_cast<const char*>(glGetString(GL_RENDERER)) << Util::Io::PrintStream::ln
        << Util::Graphic::Ansi::FOREGROUND_BRIGHT_BLUE << "GL Version: " << Util::Graphic::Ansi::RESET
        << reinterpret_cast<const char*>(glGetString(GL_VERSION)) << Util::Io::PrintStream::ln
        << Util::Graphic::Ansi::FOREGROUND_BRIGHT_BLUE << "GL Extensions: " << Util::Graphic::Ansi::RESET
        << reinterpret_cast<const char*>(glGetString(GL_EXTENSIONS)) << Util::Io::PrintStream::lnFlush;
}

int32_t main(const int32_t argc, char *argv[]) {
    Util::ArgumentParser argumentParser;
    argumentParser.addArgument("resolution", false, "r");
    argumentParser.addArgument("scale", false, "s");
    argumentParser.setHelpText(HELP_TEXT);

    if (!argumentParser.parse(argc, argv)) {
        Util::System::error << argumentParser.getErrorString() << Util::Io::PrintStream::lnFlush;
        return -1;
    }

    auto arguments = argumentParser.getUnnamedArguments();
    if (arguments.length() == 0) {
        Util::System::error << "tinygl: No arguments provided! Please specify a demo (info, triangle, gears, cubes)."
            << Util::Io::PrintStream::lnFlush;
        return -1;
    }

    const auto &demoName = arguments[0];
    if (demoName == "info") {
        info();
        return 0;
    }

    TinyGlDemo *demo = nullptr;
    if (demoName == "triangle") {
        demo = new TriangleDemo();
    } else if (demoName == "gears") {
        demo = new GearsDemo();
    } else if (demoName == "cubes") {
        demo = new CubesDemo();
    } else if (demoName == "lesson1") {
        demo = new Lesson1();
    } else if (demoName == "lesson2") {
        demo = new Lesson2();
    } else if (demoName == "lesson3") {
        demo = new Lesson3();
    } else if (demoName == "lesson4") {
        demo = new Lesson4();
    } else if (demoName == "lesson5") {
        demo = new Lesson5();
    } else if (demoName == "lesson6") {
        demo = new Lesson6();
    } else if (demoName == "lesson7") {
        demo = new Lesson7();
    } else {
        Util::System::error << "tinygl: Invalid demo '" << demoName << "'!" << Util::Io::PrintStream::lnFlush;
        return -1;
    }

    const Kepler::Window window(320, 240, Util::String::format("TinyGL (%s)",
        static_cast<const char*>(demoName)));

    EventListener eventListener;
    window.registerEventListener(eventListener);

    const auto scaleFactor = Util::String::parseFloat<float>(
        argumentParser.getArgument("scale", "1.0"));

    const auto &lfb = window.getFrameBuffer();
    if (lfb.getColorDepth() != TGL_FEATURE_RENDER_BITS) {
        Util::System::error << "tinygl: Color depth not supported (Required: " << TGL_FEATURE_RENDER_BITS
            << ", Got: " << lfb.getColorDepth() << ")!" << Util::Io::PrintStream::lnFlush;
        return -1;
    }

    Util::Graphic::BufferedLinearFrameBuffer bufferedLfb(lfb, scaleFactor);
    auto *glBuffer = ZB_open(bufferedLfb.getResolutionX(), bufferedLfb.getResolutionY(),
        ZB_MODE_RGBA, reinterpret_cast<void*>(bufferedLfb.getBuffer().get()));

    glInit(glBuffer);

    lfb.clear();

    demo->initialize(bufferedLfb.getResolutionX(), bufferedLfb.getResolutionY());

    while (isRunning) {
        statistics.startFrameTime();

        // Read and process key events
        statistics.startUpdateTime();
        while (!keyEvents.isEmpty()) {
            const auto &keyEvent = keyEvents.poll();
            if (keyEvent.isPressed() && keyEvent.getScancode() == Util::Io::KeyEvent::ESC) {
                isRunning = false;
            }

            demo->handleKeyEvent(keyEvent);
        }

        // Update the demo state.
        demo->update(statistics.getLastFrameTime().toSecondsFloat<float>());
        statistics.stopUpdateTimeTime();

        statisticsGatherTimer += statistics.getLastFrameTime();
        if (statisticsGatherTimer > Util::Time::Timestamp::ofSeconds(1)) {
            gatheredStatistics = statistics.gather();
            statisticsGatherTimer = Util::Time::Timestamp();
        }

        // Render the demo scene.
        statistics.startDrawTime();
        demo->renderFrame();

        // Update FPS display
        const auto fpsString = Util::String::format("FPS: %u | Frame time: %.1fms",
            gatheredStatistics.framesPerSecond, gatheredStatistics.frameTime.toSecondsFloat<float>() * 1000);
        const auto timesString = Util::String::format("Draw: %.1fms | Update: %.1fms | Idle: %.1fms",
            gatheredStatistics.drawTime.toSecondsFloat<float>() * 1000,
            gatheredStatistics.updateTime.toSecondsFloat<float>() * 1000,
            gatheredStatistics.idleTime.toSecondsFloat<float>() * 1000);

        bufferedLfb.drawString(fpsFont, 0, 0, fpsString,
            Util::Graphic::Colors::WHITE, Util::Graphic::Colors::INVISIBLE);

        bufferedLfb.drawString(fpsFont, 0, fpsFont.getCharHeight(), timesString,
            Util::Graphic::Colors::WHITE, Util::Graphic::Colors::INVISIBLE);

        // Flush frame to display
        bufferedLfb.flush();
        window.flush();
        statistics.stopDrawTime();

        // Sleep to hit the target framerate
        statistics.startIdleTime();
        const auto renderTime = statistics.getLastDrawTime() + statistics.getLastUpdateTime();
        if (renderTime < TARGET_FRAME_TIME) {
            Util::Async::Thread::sleep(TARGET_FRAME_TIME - renderTime);
        }
        statistics.stopIdleTime();

        statistics.stopFrameTime();
    }

    window.close();

    ZB_close(glBuffer);
    glClose();

    return 0;
}

/// Taken from https://stackoverflow.com/questions/12943164/replacement-for-gluperspective-with-glfrustrum
void glPerspective(const GLdouble fovY, const GLdouble aspect, const GLdouble zNear, const GLdouble zFar) {
    const auto fH = Util::Math::tangent(fovY / 360 * Util::Math::PI_DOUBLE) * zNear;
    const auto fW = fH * aspect;
    glFrustum(-fW, fW, -fH, fH, zNear, zFar);
}

GLuint glLoadTexture(const char *path) {
    const auto *image = Util::Graphic::BitmapFile::open(path);
    const auto *imagePixels = image->getPixelBuffer();

    // The Image class stores pixels as an array of the struct `Color`.
    // We need to convert this to an array of bytes in the RGB format (24-bit).
    auto *textureData = new uint8_t[image->getWidth() * image->getHeight() * 3];
    for (uint32_t i = 0; i < image->getWidth() * image->getHeight(); i++) {
        auto color = imagePixels[i];
        textureData[i * 3] = color.getRed();
        textureData[i * 3 + 1] = color.getGreen();
        textureData[i * 3 + 2] = color.getBlue();
    }

    // Generate the OpenGL texture
    GLuint textureId;
    glGenTextures(1, &textureId); // Generate a texture ID
    glBindTexture(GL_TEXTURE_2D, static_cast<GLint>(textureId)); // Tell OpenGL which texture to edit
    glTexImage2D(GL_TEXTURE_2D, // Type of texture
        0,                      // Mipmap level, 0 for base
        3,                      // Number of color components in texture
        image->getWidth(),      // Width of the texture
        image->getHeight(),     // Height of the texture
        0,                      // Border width in pixels
        GL_RGB,                 // Format of pixel data
        GL_UNSIGNED_BYTE,       // Type of pixel data
        textureData);           // Pointer to the image data

    // OpenGL now stores the texture data, we can delete our copy
    delete image;
    delete[] textureData;

    // This ID can be used to refer to the texture
    return textureId;
}
