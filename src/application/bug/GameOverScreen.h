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

#ifndef HHUOS_BUG_DEFENDER_GAMEOVERSCREEN_H
#define HHUOS_BUG_DEFENDER_GAMEOVERSCREEN_H

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
    explicit GameOverScreen(bool won);

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

    bool won;

    const char* LOOSE_TEXT[7] = {
            "____ ____ _  _ ____    ____ _  _ ____ ____ ",
            "| __ |__| |\\/| |___    |  | |  | |___ |__/ ",
            "|__] |  | |  | |___    |__|  \\/  |___ |  \\ ",
            "",
            "Your planet has been invaded by aliens!",
            "",
            "Press SPACE to fight the next invasion, or ESC to run like a coward..."
    };

    const char* WIN_TEXT[7] = {
            "____ ____ _  _ ____ ____ ____ ___ _  _ _    ____ ___ _ ____ _  _ ____ ",
            "|    |  | |\\ | | __ |__/ |__|  |  |  | |    |__|  |  | |  | |\\ | [__  ",
            "|___ |__| | \\| |__] |  \\ |  |  |  |__| |___ |  |  |  | |__| | \\| ___] ",
            "",
            "You have stopped the alien invasion!",
            "",
            "Press SPACE to fight the next invasion, or ESC to run like a coward..."
    };
};

#endif
