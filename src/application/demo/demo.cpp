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

#include "ant/AntDemo.h"
#include "color/AnsiColorDemo.h"
#include "fonts/FontDemo.h"
#include "keyboard/KeyboardDemo.h"
#include "opengl/OpenGlDemo.h"
#include "particles/ParticleDemo.h"
#include "polygons/PolygonDemo.h"
#include "sprites/SpriteDemo.h"
#include "widgets/WidgetDemo.h"

#include <util/base/ArgumentParser.h>
#include <util/base/String.h>
#include <util/base/System.h>
#include <util/collection/Array.h>
#include <util/graphic/Ansi.h>
#include <util/graphic/LinearFrameBuffer.h>
#include <util/io/file/File.h>
#include <util/io/stream/PrintStream.h>
#include <pulsar/Engine.h>
#include <pulsar/Game.h>

constexpr const char *HELP_TEXT =
#include "generated/README.md"
;

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
        Util::System::error << "demo: No arguments provided! Please specify a demo" << Util::Io::PrintStream::lnFlush;
        return -1;
    }

    const auto &demo = arguments[0];

    if (demo == "color") {
        ansiColorDemo();
        return 0;
    }
    if (demo == "keyboard") {
        keyboardDemo();
        return 0;
    }

    auto lfbFile = Util::Io::File("/device/lfb");
    if (argumentParser.hasArgument("resolution")) {
        const auto resolutionString = argumentParser.getArgument("resolution");
        Util::Graphic::LinearFrameBuffer::setResolution(lfbFile, resolutionString);
    }

    Util::Graphic::Ansi::prepareGraphicalApplication(true);
    Util::Graphic::LinearFrameBuffer lfb(lfbFile);
    lfb.clear();

    if (demo == "ant") {
        antDemo(lfb);
    } else if (demo == "fonts") {
        fontDemo(lfb);
    } else if (demo == "widgets") {
        WidgetDemo widgetDemo(lfb);
        widgetDemo.run();
    } else {
        const auto scaleFactor = Util::String::parseFloat<float>(
            argumentParser.getArgument("scale", "1.0f"));
         Pulsar::Engine engine(lfb, 60, scaleFactor);

        if (demo == "opengl") {
            Pulsar::Game::getInstance().pushScene(new OpenGlDemo());
        } else if (demo == "particles") {
            Pulsar::Game::getInstance().pushScene(new ParticleDemo());
        } else if (demo == "polygons") {
            Pulsar::Game::getInstance().pushScene(new PolygonDemo());
        } else if (demo == "sprites") {
            Pulsar::Game::getInstance().pushScene(new SpriteDemo());
        } else {
            Util::System::error << "demo: Invalid demo '" << demo << "'!" << Util::Io::PrintStream::lnFlush;
            return -1;
        }

        engine.run();
    }

    Util::Graphic::Ansi::cleanupGraphicalApplication();
    return 0;
}
