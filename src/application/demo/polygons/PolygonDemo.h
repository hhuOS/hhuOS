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

#ifndef HHUOS_POLYGONDEMO_H
#define HHUOS_POLYGONDEMO_H

#include <stdint.h>

#include "lib/util/collection/ArrayListBlockingQueue.h"
#include "DemoPolygonFactory.h"
#include "lib/util/game/KeyListener.h"
#include "lib/util/game/2d/Scene.h"

class DemoPolygon;

namespace Util {
namespace Game {
class Graphics;
}  // namespace Game

namespace Io {
class Key;
}  // namespace Io
}  // namespace Util

class PolygonDemo : public Util::Game::D2::Scene, public Util::Game::KeyListener {

public:
    /**
     * Default Constructor.
     */
    explicit PolygonDemo(uint32_t initialCount);

    /**
     * Copy Constructor.
     */
    PolygonDemo(const PolygonDemo &other) = delete;

    /**
     * Assignment operator.
     */
    PolygonDemo &operator=(const PolygonDemo &other) = delete;

    /**
     * Destructor.
     */
    ~PolygonDemo() override = default;

    void initialize() override;

    void update(double delta) override;

    void initializeBackground(Util::Game::Graphics &graphics) override;

    void keyPressed(const Util::Io::Key &key) override;

    void keyReleased(const Util::Io::Key &key) override;

private:

    uint32_t initialCount;
    DemoPolygonFactory factory;
    Util::ArrayListBlockingQueue<DemoPolygon*> polygons;
};

#endif