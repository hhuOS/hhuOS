/*
 * Copyright (C) 2018-2022 Heinrich-Heine-Universitaet Duesseldorf,
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

#include "lib/util/system/System.h"
#include "lib/util/graphic/LinearFrameBuffer.h"
#include "lib/util/game/Engine.h"
#include "lib/util/ArgumentParser.h"
#include "PolygonDemo.h"
#include "lib/util/data/Array.h"
#include "lib/util/file/File.h"
#include "lib/util/memory/String.h"
#include "lib/util/stream/PrintWriter.h"

static const constexpr int32_t DEFAULT_COUNT = 10;

int32_t main(int32_t argc, char *argv[]) {
    auto argumentParser = Util::ArgumentParser();
    argumentParser.setHelpText("Demo application, displaying multiple rotating polygons.\n"
                               "The amount of polygons can be adjusted via the '+' and '-' keys.\n"
                               "Usage: polygon [COUNT]\n"
                               "Options:\n"
                               "  -h, --help: Show this help message");

    if (!argumentParser.parse(argc, argv)) {
        Util::System::error << argumentParser.getErrorString() << Util::Stream::PrintWriter::endl << Util::Stream::PrintWriter::flush;
        return -1;
    }

    auto arguments = argumentParser.getUnnamedArguments();
    auto count = arguments.length() == 0 ? DEFAULT_COUNT : Util::Memory::String::parseInt(arguments[0]);

    auto game = PolygonDemo(count);
    auto lfbFile = Util::File::File("/device/lfb");
    auto lfb = Util::Graphic::LinearFrameBuffer(lfbFile);
    auto engine = Util::Game::Engine(game, lfb);

    engine.run();
    return 0;
}