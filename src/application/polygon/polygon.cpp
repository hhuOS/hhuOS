/*
 * Copyright (C) 2018-2023 Heinrich-Heine-Universitaet Duesseldorf,
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

#include <cstdint>

#include "lib/util/base/System.h"
#include "lib/util/graphic/LinearFrameBuffer.h"
#include "lib/util/game/Engine.h"
#include "lib/util/base/ArgumentParser.h"
#include "PolygonDemo.h"
#include "lib/util/collection/Array.h"
#include "lib/util/io/file/File.h"
#include "lib/util/base/String.h"
#include "lib/util/io/stream/PrintStream.h"
#include "lib/util/game/GameManager.h"
#include "lib/util/game/Game.h"

static const constexpr int32_t DEFAULT_COUNT = 10;

int32_t main(int32_t argc, char *argv[]) {
    auto argumentParser = Util::ArgumentParser();
    argumentParser.setHelpText("Demo application, displaying multiple rotating polygons.\n"
                               "The amount of polygons can be adjusted via the '+' and '-' keys.\n"
                               "Usage: polygon [COUNT]\n"
                               "Options:\n"
                               "  -h, --help: Show this help message");

    if (!argumentParser.parse(argc, argv)) {
        Util::System::error << argumentParser.getErrorString() << Util::Io::PrintStream::endl << Util::Io::PrintStream::flush;
        return -1;
    }

    auto arguments = argumentParser.getUnnamedArguments();
    auto count = arguments.length() == 0 ? DEFAULT_COUNT : Util::String::parseInt(arguments[0]);

    auto lfbFile = Util::Io::File("/device/lfb");
    auto lfb = Util::Graphic::LinearFrameBuffer(lfbFile);
    auto engine = Util::Game::Engine(lfb, 60);

    Util::Game::GameManager::getGame().pushScene(new PolygonDemo(count));
    engine.run();

    return 0;
}