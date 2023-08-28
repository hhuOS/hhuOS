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

    void update(double delta) override;

    void initializeBackground(Util::Game::Graphics &graphics) override;

    void keyPressed(Util::Io::Key key) override;

    void keyReleased(Util::Io::Key key) override;

private:

    const bool won;

    static const constexpr char *CONGRATULATIONS = "Congratulations!";
    static const constexpr char *INVASION_STOPPED = "You have stopped the alien invasion!";
    static const constexpr char *LOST = "Oh noooooo!";
    static const constexpr char *PLANET_INVADED = "Your planet has been invaded by aliens!";
    static const constexpr char *NEW_GAME = "Press SPACE to fight the next invasion, or ESC to run like a coward...";
};

#endif
