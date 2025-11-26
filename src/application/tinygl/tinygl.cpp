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

#include <stdint.h>

#include "kepler/Window.h"
#include "kepler/WindowManagerPipe.h"
#include "lib/util/base/Address.h"
#include "lib/util/base/System.h"
#include "lib/util/base/ArgumentParser.h"
#include "lib/util/io/file/File.h"
#include "lib/util/graphic/LinearFrameBuffer.h"
#include "lib/util/graphic/Ansi.h"
#include "lib/tinygl/include/zbuffer.h"
#include "lib/util/base/String.h"
#include "lib/util/collection/Array.h"
#include "lib/util/io/stream/PrintStream.h"
#include "lib/util/graphic/BufferedLinearFrameBuffer.h"
#include "lib/tinygl/include/zbuffer.h"

extern void info();
extern void triangle(const Kepler::Window &window, const Util::Graphic::BufferedLinearFrameBuffer &lfb);
extern void gears(const Kepler::Window &window, const Util::Graphic::BufferedLinearFrameBuffer &lfb);
extern void cubes(const Kepler::Window &window, const Util::Graphic::BufferedLinearFrameBuffer &lfb);

int32_t main(int32_t argc, char *argv[]) {
    auto argumentParser = Util::ArgumentParser();
    argumentParser.setHelpText("TinyGL demo application.\n\n"
                               "Usage: tinygl <demo>\n"
                               "Demos: info, triangle, gears, cubes\n"
                               "Options:\n"
                               "  -r, --resolution: Set display resolution\n"
                               "  -s, --scale: Set display scale factor (Must be <= 1; The application will be rendered at a lower internal resolution and scaled up/centered to fill the screen)\n"
                               "  -h, --help: Show this help message");

    argumentParser.addArgument("resolution", false, "r");
    argumentParser.addArgument("scale", false, "s");

    if (!argumentParser.parse(argc, argv)) {
        Util::System::error << argumentParser.getErrorString() << Util::Io::PrintStream::ln << Util::Io::PrintStream::flush;
        return -1;
    }

    auto arguments = argumentParser.getUnnamedArguments();
    if (arguments.length() == 0) {
        Util::System::error << "tinygl: No arguments provided! Please specify a demo (info, triangle, gears, cubes)." << Util::Io::PrintStream::ln << Util::Io::PrintStream::flush;
        return -1;
    }

    const auto &demo = arguments[0];
    if (demo == "info") {
        info();
        return 0;
    }

    auto pipe = Kepler::WindowManagerPipe();
    const auto window = Kepler::Window(320, 240, Util::String::format("TinyGL (%s)", static_cast<const char*>(demo)), pipe);
    const auto &lfb = window.getFrameBuffer();

    if (lfb.getColorDepth() != TGL_FEATURE_RENDER_BITS) {
        Util::System::error << "tinygl: Color depth not supported (Required: " << TGL_FEATURE_RENDER_BITS << ", Got: " << lfb.getColorDepth() << ")!" << Util::Io::PrintStream::ln << Util::Io::PrintStream::flush;
        return -1;
    }

    auto scaleFactor = argumentParser.hasArgument("scale") ? Util::String::parseFloat<double>(argumentParser.getArgument("scale")) : 1.0;
    auto bufferedLfb = Util::Graphic::BufferedLinearFrameBuffer(lfb, scaleFactor);
    auto *glBuffer = ZB_open(bufferedLfb.getResolutionX(), bufferedLfb.getResolutionY(), ZB_MODE_RGBA, reinterpret_cast<void*>(bufferedLfb.getBuffer().get()));
    glInit(glBuffer);

    lfb.clear();

    if (demo == "triangle") {
        triangle(window, bufferedLfb);
    } else if (demo == "gears") {
        gears(window, bufferedLfb);
    } else if (demo == "cubes") {
        cubes(window, bufferedLfb);
    } else {
        Util::System::error << "opengl: Invalid demo '" << demo << "'!" << Util::Io::PrintStream::ln << Util::Io::PrintStream::flush;
        return -1;
    }

    ZB_close(glBuffer);
    glClose();

    Util::Graphic::Ansi::cleanupGraphicalApplication();
    return 0;
}