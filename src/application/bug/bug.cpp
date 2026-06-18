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

#include "bug.h"

#include "BugDefender.h"

#include <util/base/System.h>
#include <util/base/ArgumentParser.h>
#include <util/io/stream/PrintStream.h>
#include <util/io/file/File.h>
#include <util/graphic/LinearFrameBuffer.h>
#include <util/base/String.h>
#include <pulsar/TextScreen.h>
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

    const Util::Io::File lfbFile("/device/lfb");

    if (argumentParser.hasArgument("resolution")) {
        const auto resolutionString = argumentParser.getArgument("resolution");
        Util::Graphic::LinearFrameBuffer::setResolution(lfbFile, resolutionString);
    }

    const auto scaleFactor = Util::String::parseFloat<float>(
        argumentParser.getArgument("scale", "1.0f"));

    const Util::Graphic::LinearFrameBuffer lfb(lfbFile);
    Pulsar::Engine engine(lfb, 60, scaleFactor);

    Pulsar::Game::getInstance().pushScene(
        new Pulsar::TextScreen(INTRO_TEXT, handleKeyPressOnTextScreen, Util::Graphic::Colors::GREEN));

    engine.run();
    return 0;
}

void handleKeyPressOnTextScreen(const Util::Io::KeyEvent &key) {
    switch (key.getScancode()) {
        case Util::Io::KeyEvent::ESC:
            Pulsar::Game::getInstance().stop();
            break;
        case Util::Io::KeyEvent::SPACE:
            Pulsar::Game::getInstance().pushScene(new BugDefender());
            Pulsar::Game::getInstance().switchToNextScene();
            break;
        default:
            break;
    }
}
