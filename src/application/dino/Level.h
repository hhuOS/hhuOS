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

#ifndef HHUOS_LEVEL_H
#define HHUOS_LEVEL_H

#include "lib/util/game/KeyListener.h"
#include "PlayerDino.h"
#include "lib/util/math/Random.h"
#include "lib/util/collection/ArrayList.h"
#include "lib/util/math/Vector2D.h"
#include "lib/util/game/2d/Scene.h"
#include "lib/util/collection/ArrayListBlockingQueue.h"
#include "Block.h"
#include "lib/util/collection/Pair.h"
#include "lib/util/io/file/File.h"

class Ground;
class Saw;

namespace Util {
namespace Game {
class Graphics;
}  // namespace Game

namespace Io {
class Key;
}  // namespace Io
}  // namespace Util

class Level : public Util::Game::D2::Scene, public Util::Game::KeyListener {

public:
    /**
     * Constructor.
     */
    explicit Level(const Util::Io::File &levelFile, uint32_t points);

    /**
     * Copy Constructor.
     */
    Level(const Level &other) = delete;

    /**
     * Assignment operator.
     */
    Level &operator=(const Level &other) = delete;

    /**
     * Destructor.
     */
    ~Level() override = default;

    void update(double delta) override;

    void initializeBackground(Util::Game::Graphics &graphics) override;

    void keyPressed(Util::Io::Key key) override;

    void keyReleased(Util::Io::Key key) override;

private:

    void spawnMergedBlocks(Util::ArrayList<Util::Pair<int32_t, double>> &positions, Block::Tag tag);

    PlayerDino *player = nullptr;

    bool leftPressed = false;
    bool rightPressed = false;

    double playerMinX = 0.0;
    double playerMinY = 0.0;
};

#endif
