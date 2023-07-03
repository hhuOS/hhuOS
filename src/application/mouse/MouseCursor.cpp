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

#include "MouseCursor.h"

#include "lib/util/game/Graphics2D.h"
#include "lib/util/math/Vector2D.h"
#include "lib/util/game/entity/event/TranslationEvent.h"

namespace Util {
namespace Game {
class CollisionEvent;
}  // namespace Game
}  // namespace Util

MouseCursor::MouseCursor() : Util::Game::Entity(0, Util::Math::Vector2D(0, 0)) {}

void MouseCursor::initialize() {}

void MouseCursor::onUpdate(double delta) {}

void MouseCursor::onTranslationEvent(Util::Game::TranslationEvent &event) {
    event.cancel();
}

void MouseCursor::onCollisionEvent(Util::Game::CollisionEvent &event) {}

void MouseCursor::draw(Util::Game::Graphics2D &graphics) {
    graphics.drawImage(getPosition(), currentSprite->getImage());
}

void MouseCursor::keyPressed(Key key) {
    switch (key) {
        case Util::Game::MouseListener::LEFT:
            currentSprite = &leftClickSprite;
            break;
        case Util::Game::MouseListener::RIGHT:
            currentSprite = &rightClickSprite;
            break;
        case Util::Game::MouseListener::MIDDLE:
            currentSprite = &middleClickSprite;
            break;
        default:
            break;
    }
}

void MouseCursor::keyReleased(Key key) {
    currentSprite = &defaultSprite;
}

void MouseCursor::mouseMoved(const Util::Math::Vector2D &relativeMovement) {
    const auto &oldPosition = getPosition();
    setPositionX(oldPosition.getX() + relativeMovement.getX());
    setPositionY(oldPosition.getY() + relativeMovement.getY());

    const auto &newPosition = getPosition();
    if (newPosition.getX() < -1) setPosition({-1, newPosition.getY()});
    if (newPosition.getX() > 1) setPosition({1, newPosition.getY()});
    if (newPosition.getY() < -1 - currentSprite->getHeight()) setPosition({newPosition.getX(), -1 - currentSprite->getHeight()});
    if (newPosition.getY() > 1 - currentSprite->getHeight()) setPosition({newPosition.getX(), 1 - currentSprite->getHeight()});
}
