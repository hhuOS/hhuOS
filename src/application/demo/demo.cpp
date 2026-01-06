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

#include "lib/util/base/System.h"
#include "lib/util/graphic/LinearFrameBuffer.h"
#include "lib/pulsar/Engine.h"
#include "lib/util/base/ArgumentParser.h"
#include "lib/util/collection/Array.h"
#include "lib/util/io/file/File.h"
#include "lib/util/base/String.h"
#include "lib/util/io/stream/PrintStream.h"
#include "lib/pulsar/Game.h"
#include "application/demo/polygons/PolygonDemo.h"
#include "application/demo/sprites/SpriteDemo.h"
#include "application/demo/ant/Ant.h"
#include "application/demo/particles/ParticleDemo.h"
#include "application/demo/color/AnsiColorDemo.h"
#include "application/demo/fonts/FontDemo.h"
#include "opengl/OpenGlDemo.h"
#include "widget/WidgetDemo.h"

int32_t main(int32_t argc, char *argv[]) {
    auto argumentParser = Util::ArgumentParser();
    argumentParser.setHelpText("Demo applications, showing off the systems graphical capabilities.\n"
                               "Usage: demo [DEMO] [OPTIONS]...\n"
                               "Demos: ant, color, fonts, opengl, particles, polygons, sprites\n"
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
        Util::System::error << "demo: No arguments provided! Please specify a demo (ant, color, fonts, mouse, opengl, particles, polygons, sprites)." << Util::Io::PrintStream::ln << Util::Io::PrintStream::flush;
        return -1;
    }

    auto demo = arguments[0];

    if (demo == "color") {
        ansiColorDemo();
        return 0;
    }

    auto lfbFile = Util::Io::File("/device/lfb");

    if (argumentParser.hasArgument("resolution")) {
        auto split1 = argumentParser.getArgument("resolution").split("x");
        auto split2 = split1[1].split("@");

        auto resolutionX = Util::String::parseNumber<uint16_t>(split1[0]);
        auto resolutionY = Util::String::parseNumber<uint16_t>(split2[0]);
        uint8_t colorDepth = split2.length() > 1 ? Util::String::parseNumber<uint8_t>(split2[1]) : 32;

        lfbFile.controlFile(Util::Graphic::LinearFrameBuffer::SET_RESOLUTION, Util::Array<uint32_t>({resolutionX, resolutionY, colorDepth}));
    }

    auto lfb = Util::Graphic::LinearFrameBuffer(lfbFile);

    if (demo == "ant") {
        auto sleepInterval = arguments.length() <= 1 ? 0 : Util::String::parseNumber<uint32_t>(arguments[0]);
        antDemo(lfb, sleepInterval);
    } else if (demo == "fonts") {
        fontDemo(lfb);
    } else if (demo == "widgets") {
        auto widgetDemo = WidgetDemo(lfb);
        widgetDemo.run();
    } else {
        auto scaleFactor = argumentParser.hasArgument("scale") ? Util::String::parseFloat<double>(argumentParser.getArgument("scale")) : 1.0;
        auto engine = Pulsar::Engine(lfb, 60, scaleFactor);

        if (demo == "opengl") {
            Pulsar::Game::getInstance().pushScene(new OpenGlDemo());
        } else if (demo == "particles") {
            Pulsar::Game::getInstance().pushScene(new ParticleDemo());
        } else if (demo == "polygons") {
            auto initialCount = arguments.length() > 1 ? Util::String::parseNumber<uint32_t>(arguments[1]) : 10;
            Pulsar::Game::getInstance().pushScene(new PolygonDemo(initialCount));
        } else if (demo == "sprites") {
            auto initialCount = arguments.length() > 1 ? Util::String::parseNumber<uint32_t>(arguments[1]) : 10;
            Pulsar::Game::getInstance().pushScene(new SpriteDemo(initialCount));
        } else {
            Util::System::error << "demo: Invalid demo '" << demo << "'!" << Util::Io::PrintStream::ln << Util::Io::PrintStream::flush;
            return -1;
        }

        engine.run();
    }

    return 0;
}
