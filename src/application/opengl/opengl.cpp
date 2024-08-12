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

#include "opengl.h"
#include "lib/util/base/Address.h"
#include "lib/util/base/System.h"
#include "lib/util/base/ArgumentParser.h"
#include "lib/util/io/file/File.h"
#include "lib/util/graphic/LinearFrameBuffer.h"
#include "lib/util/graphic/Ansi.h"
#include "lib/tinygl/include/zbuffer.h"
#include "lib/tinygl/include/GL/gl.h"
#include "lib/util/base/String.h"
#include "lib/util/collection/Array.h"
#include "lib/util/io/stream/PrintStream.h"

extern void info();
extern void triangle(void *frameBuffer, const Util::Graphic::LinearFrameBuffer &lfb);
extern void gears(void *frameBuffer, const Util::Graphic::LinearFrameBuffer &lfb);

int32_t main(int32_t argc, char *argv[]) {
    auto argumentParser = Util::ArgumentParser();
    argumentParser.setHelpText("OpenGL demo application.\n\n"
                               "Usage: opengl <demo>\n"
                               "Demos: info, triangle, gears\n"
                               "Options:\n"
                               "  -r, --resolution: Set display resolution"
                               "  -h, --help: Show this help message");

    argumentParser.addArgument("resolution", false, "r");

    if (!argumentParser.parse(argc, argv)) {
        Util::System::error << argumentParser.getErrorString() << Util::Io::PrintStream::endl << Util::Io::PrintStream::flush;
        return -1;
    }

    auto arguments = argumentParser.getUnnamedArguments();
    if (arguments.length() == 0) {
        Util::System::error << "opengl: No arguments provided! Please specify a demo (info, triangle, gears)." << Util::Io::PrintStream::endl << Util::Io::PrintStream::flush;
        return -1;
    }

    auto demo = arguments[0];
    if (demo == "info") {
        info();
        return 0;
    }

    Util::Graphic::Ansi::prepareGraphicalApplication(true);
    auto lfbFile = Util::Io::File("/device/lfb");

    if (argumentParser.hasArgument("resolution")) {
        auto split1 = argumentParser.getArgument("resolution").split("x");
        auto split2 = split1[1].split("@");

        uint32_t resolutionX = Util::String::parseInt(split1[0]);
        uint32_t resolutionY = Util::String::parseInt(split2[0]);
        uint32_t colorDepth = split2.length() > 1 ? Util::String::parseInt(split2[1]) : 32;

        lfbFile.controlFile(Util::Graphic::LinearFrameBuffer::SET_RESOLUTION, Util::Array<uint32_t>({resolutionX, resolutionY, colorDepth}));
    }

    auto lfb = Util::Graphic::LinearFrameBuffer(lfbFile);
    auto *glBuffer = ZB_open(lfb.getResolutionX(), lfb.getResolutionY(), ZB_MODE_RGBA, nullptr);
    glInit(glBuffer);

    if (demo == "triangle") {
        triangle(glBuffer, lfb);
    } else if (demo == "gears") {
        gears(glBuffer, lfb);
    } else {
        Util::System::error << "opengl: Invalid demo '" << demo << "'!" << Util::Io::PrintStream::endl << Util::Io::PrintStream::flush;
        return -1;
    }

    Util::Graphic::Ansi::cleanupGraphicalApplication();
    return 0;
}

void flush(void *glBuffer, const Util::Graphic::LinearFrameBuffer &lfb) {
    ZB_copyFrameBuffer(reinterpret_cast<ZBuffer*>(glBuffer), reinterpret_cast<void*>(lfb.getBuffer().get()), lfb.getPitch());
}