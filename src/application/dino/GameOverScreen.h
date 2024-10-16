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
 *
 * The dino game is based on a bachelor's thesis, written by Malte Sehmer.
 * The original source code can be found here: https://github.com/Malte2036/hhuOS
 */

#ifndef HHUOS_BUG_DEFENDER_GAMEOVERSCREEN_H
#define HHUOS_BUG_DEFENDER_GAMEOVERSCREEN_H

#include <stdint.h>

#include "lib/util/game/2d/Scene.h"
#include "lib/util/game/KeyListener.h"

namespace Util {
namespace Game {
class Graphics;
}  // namespace Game
namespace Io {
class Key;
}  // namespace Io
}  // namespace Util

class GameOverScreen : public Util::Game::D2::Scene, public Util::Game::KeyListener {

public:
    /**
     * Constructor.
     */
    explicit GameOverScreen(uint32_t score);

    /**
     * Copy Constructor.
     */
    GameOverScreen(const GameOverScreen &other) = delete;

    /**
     * Assignment operator.
     */
    GameOverScreen &operator=(const GameOverScreen &other) = delete;

    /**
     * Destructor.
     */
    ~GameOverScreen() override = default;

    void initialize() override;

    void update(double delta) override;

    void initializeBackground(Util::Game::Graphics &graphics) override;

    void keyPressed(const Util::Io::Key &key) override;

    void keyReleased(const Util::Io::Key &key) override;

private:

    uint32_t score;

    const char* TEXT[7] = {
            "____ ____ _  _ ____    ____ _  _ ____ ____ ",
            "| __ |__| |\\/| |___    |  | |  | |___ |__/ ",
            "|__] |  | |  | |___    |__|  \\/  |___ |  \\ ",
            "",
            "Score :%d",
            "",
            "Press SPACE to play again or ESC to exit!"
    };
};

#endif
