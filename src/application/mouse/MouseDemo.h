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

#ifndef HHUOS_MOUSEDEMO_H
#define HHUOS_MOUSEDEMO_H

#include "MouseCursor.h"
#include "lib/util/game/KeyListener.h"
#include "lib/util/game/Scene.h"

namespace Util {
namespace Game {
class Graphics2D;
}  // namespace Game

namespace Io {
class Key;
}  // namespace Io
}  // namespace Util

class MouseDemo : public Util::Game::Scene, public Util::Game::KeyListener {

public:
    /**
     * Default Constructor.
     */
    MouseDemo();

    /**
     * Copy Constructor.
     */
    MouseDemo(const MouseDemo &other) = delete;

    /**
     * Assignment operator.
     */
    MouseDemo &operator=(const MouseDemo &other) = delete;

    /**
     * Destructor.
     */
    ~MouseDemo() override = default;

    void update(double delta) override;

    void initializeBackground(Util::Game::Graphics2D &graphics) override;

    void keyPressed(Util::Io::Key key) override;

    void keyReleased(Util::Io::Key key) override;

private:

    MouseCursor *cursor = new MouseCursor();
};

#endif
