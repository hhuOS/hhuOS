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

#ifndef HHUOS_MOUSECURSOR_H
#define HHUOS_MOUSECURSOR_H

#include "lib/util/game/MouseListener.h"
#include "lib/util/game/entity/Entity.h"

namespace Util {
namespace Math {
class Vector2D;
}  // namespace Math

namespace Game {
class Graphics2D;
class CollisionEvent;
class TranslationEvent;
}  // namespace Game
}  // namespace Util

class MouseCursor : public Util::Game::Entity, public Util::Game::MouseListener {

public:
    /**
     * Default Constructor.
     */
    MouseCursor();

    /**
     * Copy Constructor.
     */
    MouseCursor(const MouseCursor &other) = delete;

    /**
     * Assignment operator.
     */
    MouseCursor &operator=(const MouseCursor &other) = delete;

    /**
     * Destructor.
     */
    ~MouseCursor() override = default;

    void initialize() override;

    void onUpdate(double delta) override;

    void onTranslationEvent(Util::Game::TranslationEvent &event) override;

    void onCollisionEvent(Util::Game::CollisionEvent &event) override;

    void draw(Util::Game::Graphics2D &graphics) override;

    void keyPressed(Key key) override;

    void keyReleased(Key key) override;

    void mouseMoved(const Util::Math::Vector2D &relativeMovement) override;

private:

    bool leftPressed = false;
    bool rightPressed = false;
    bool middlePressed = false;
};

#endif
