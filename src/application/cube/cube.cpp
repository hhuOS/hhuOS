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

#include "lib/util/game/Engine.h"
#include "lib/util/system/System.h"
#include "CubeDemo.h"
#include "lib/util/async/Thread.h"

static const constexpr int32_t DEFAULT_SPEED = 10;

int32_t main(int32_t argc, char *argv[]) {
    auto speed = argc > 1 ? Util::Memory::String::parseInt(argv[1]) : DEFAULT_SPEED;
    if (speed < 0) {
        Util::System::error << "Speed must be greater than 0!";
        return -1;
    }

    auto game = CubeDemo(speed);
    auto lfbFile = Util::File::File("/device/lfb");
    auto lfb = Util::Graphic::LinearFrameBuffer(lfbFile);
    auto engine = Util::Game::Engine(game, lfb);

    engine.run();

    return 0;
}