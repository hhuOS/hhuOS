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

#ifndef HHUOS_ENGINE_H
#define HHUOS_ENGINE_H


#include <cstdint>
#include "lib/util/async/Runnable.h"
#include "lib/util/data/ArrayList.h"
#include "Drawable.h"
#include "Game.h"

namespace Util::Game {

class Engine : public Util::Async::Runnable {

public:
    /**
     * Default Constructor.
     */
    explicit Engine(Game &game, const Util::Graphic::LinearFrameBuffer &lfb, uint8_t targetFrameRate = 60);

    /**
     * Copy Constructor.
     */
    Engine(const Engine &other) = delete;

    /**
     * Assignment operator.
     */
    Engine &operator=(const Engine &other) = delete;

    /**
     * Destructor.
     */
    ~Engine() override = default;

    void run() override;

private:

    void drawStatus();

    Game &game;
    Graphics2D graphics;
    uint32_t frameTime;
    const uint8_t targetFrameRate;
};

}

#endif
