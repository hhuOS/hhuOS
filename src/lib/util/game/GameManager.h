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
 * The game engine is based on a bachelor's thesis, written by Malte Sehmer.
 * The original source code can be found here: https://github.com/Malte2036/hhuOS
 *
 * It has been enhanced with 3D-capabilities during a bachelor's thesis by Richard Josef Schweitzer
 * The original source code can be found here: https://git.hhu.de/bsinfo/thesis/ba-risch114
 */

#ifndef HHUOS_GAMEMANAGER_H
#define HHUOS_GAMEMANAGER_H

#include <stdint.h>

namespace Util {
namespace Math {
class Vector2D;
}  // namespace Math

namespace Game {
class Game;
class Scene;
}  // namespace Game
}  // namespace Util

namespace Util::Game {

class GameManager {

friend class Engine;
friend class Graphics;

public:
    /**
     * Default Constructor.
     * Deleted, as this class has only static members.
     */
    GameManager() = delete;

    /**
     * Copy Constructor.
     */
    GameManager(const GameManager &other) = delete;

    /**
     * Assignment operator.
     */
    GameManager &operator=(const GameManager &other) = delete;

    /**
     * Destructor.
     */
    ~GameManager() = default;

    [[nodiscard]] static uint16_t getTransformation();

    [[nodiscard]] static const Math::Vector2D& getAbsoluteResolution();

    [[nodiscard]] static const Math::Vector2D& getRelativeResolution();

    [[nodiscard]] static Game& getGame();

    [[nodiscard]] static Scene& getCurrentScene();

private:

    static Game *game;
    static Math::Vector2D absoluteResolution;
    static Math::Vector2D relativeResolution;
    static uint16_t transformation;
};

}

#endif
