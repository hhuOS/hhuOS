/*
 * Copyright (C) 2017-2025 Heinrich Heine University Düsseldorf,
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
 *
 * The dino game is based on a bachelor's thesis, written by Malte Sehmer.
 * The original source code can be found here: https://github.com/Malte2036/hhuOS
 */

#include <stdint.h>

#include "dino.h"
#include "lib/pulsar/Engine.h"
#include "lib/util/base/System.h"
#include "lib/util/base/ArgumentParser.h"
#include "lib/util/io/stream/PrintStream.h"
#include "lib/util/io/file/File.h"
#include "lib/util/graphic/LinearFrameBuffer.h"
#include "lib/pulsar/Game.h"
#include "Level.h"
#include "lib/util/base/String.h"
#include "lib/util/collection/Array.h"
#include "pulsar/TextScreen.h"

int32_t main(int32_t argc, char *argv[]) {
    auto argumentParser = Util::ArgumentParser();
    argumentParser.setHelpText("Endless runner game.\n"
                               "Usage: dino\n"
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

    auto lfbFile = Util::Io::File("/device/lfb");

    if (argumentParser.hasArgument("resolution")) {
        auto split1 = argumentParser.getArgument("resolution").split("x");
        auto split2 = split1[1].split("@");

        auto resolutionX = Util::String::parseNumber<uint16_t>(split1[0]);
        auto resolutionY = Util::String::parseNumber<uint16_t>(split2[0]);
        uint8_t colorDepth = split2.length() > 1 ? Util::String::parseNumber<uint8_t>(split2[1]) : 32;

        lfbFile.controlFile(Util::Graphic::LinearFrameBuffer::SET_RESOLUTION, Util::Array<uint32_t>({resolutionX, resolutionY, colorDepth}));
    }

    auto scaleFactor = argumentParser.hasArgument("scale") ? Util::String::parseFloat<float>(argumentParser.getArgument("scale")) : 1.0;
    auto lfb = Util::Graphic::LinearFrameBuffer::open(lfbFile);
    auto engine = Pulsar::Engine(lfb, 60, scaleFactor);

    Pulsar::Game::getInstance().pushScene(new Pulsar::TextScreen(INTRO_TEXT, handleKeyPressOnTextScreen, Util::Graphic::Colors::GREEN));
    engine.run();

    return 0;
}

void handleKeyPressOnTextScreen(const Util::Io::Key &key) {
    switch (key.getScancode()) {
        case Util::Io::Key::ESC:
            Pulsar::Game::getInstance().stop();
            break;
        case Util::Io::Key::SPACE:
            Pulsar::Game::getInstance().pushScene(new Level(Util::Io::File("/user/dino/level/level1.txt"), 0));
            Pulsar::Game::getInstance().switchToNextScene();
            break;
        default:
            break;
    }
}