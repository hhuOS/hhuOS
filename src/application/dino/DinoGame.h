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

#ifndef HHUOS_DINOGAME_H
#define HHUOS_DINOGAME_H

#include "lib/util/game/Game.h"
#include "lib/util/game/KeyListener.h"
#include "lib/util/math/Vector2D.h"
#include "Ground.h"
#include "lib/util/math/Random.h"
#include "Tree.h"
#include "lib/util/game/Text.h"

class Dino;

namespace Util {
namespace Io {
class Key;
}  // namespace Io
}  // namespace Util

class DinoGame : public Util::Game::Game, public Util::Game::KeyListener {

public:
    /**
     * Default Constructor.
     */
    DinoGame() = default;

    /**
     * Copy Constructor.
     */
    DinoGame(const DinoGame &other) = delete;

    /**
     * Assignment operator.
     */
    DinoGame &operator=(const DinoGame &other) = delete;

    /**
     * Destructor.
     */
    ~DinoGame() override = default;

    void update(double delta) override;

    void keyPressed(Util::Io::Key key) override;

    void keyReleased(Util::Io::Key key) override;

private:

    Dino *dino = nullptr;
    Ground *ground = nullptr;
    Util::Game::Text *pointText = nullptr;

    Util::ArrayList<Tree*> trees;
    Util::Math::Random treeRandom;
    double treeCooldown = 0;
};

#endif
