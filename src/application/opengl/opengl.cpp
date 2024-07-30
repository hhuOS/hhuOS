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

#include "lib/util/base/System.h"
#include "lib/util/base/ArgumentParser.h"
#include "lib/util/io/file/File.h"
#include "lib/util/graphic/LinearFrameBuffer.h"
#include "lib/tinygl/include/GL/gl.h"
#include "lib/util/graphic/Ansi.h"

extern void triangle(const Util::Graphic::LinearFrameBuffer &lfb);

void info() {
    Util::System::out << Util::Graphic::Ansi::FOREGROUND_BRIGHT_BLUE << "GL Version: " << Util::Graphic::Ansi::RESET << reinterpret_cast<const char*>(glGetString(GL_VERSION)) << Util::Io::PrintStream::endl
            << Util::Graphic::Ansi::FOREGROUND_BRIGHT_BLUE << "GL Vendor: " << Util::Graphic::Ansi::RESET << reinterpret_cast<const char*>(glGetString(GL_VENDOR)) << Util::Io::PrintStream::endl
            << Util::Graphic::Ansi::FOREGROUND_BRIGHT_BLUE << "GL Renderer: " << Util::Graphic::Ansi::RESET << reinterpret_cast<const char*>(glGetString(GL_RENDERER)) << Util::Io::PrintStream::endl
            << Util::Graphic::Ansi::FOREGROUND_BRIGHT_BLUE << "GL Extensions: " << Util::Graphic::Ansi::RESET << reinterpret_cast<const char*>(glGetString(GL_EXTENSIONS)) << Util::Io::PrintStream::endl << Util::Io::PrintStream::flush;
}

int32_t main(int32_t argc, char *argv[]) {
    auto argumentParser = Util::ArgumentParser();
    argumentParser.setHelpText("OpenGL demo application.\n\n"
                               "Usage: opengl <demo>\n"
                               "Demos: info\n"
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
        Util::System::error << "opengl: No arguments provided! Please specify a demo (info)." << Util::Io::PrintStream::endl << Util::Io::PrintStream::flush;
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

    if (demo == "triangle") {
        triangle(lfb);
    }

    Util::Graphic::Ansi::cleanupGraphicalApplication();
    return 0;
}