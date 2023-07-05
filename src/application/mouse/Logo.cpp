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

#include "Logo.h"

#include "lib/util/game/entity/event/TranslationEvent.h"
#include "lib/util/game/GameManager.h"
#include "lib/util/game/Graphics2D.h"
#include "lib/util/math/Vector2D.h"

namespace Util {
namespace Game {
class CollisionEvent;
}  // namespace Game
}  // namespace Util

Logo::Logo() : Util::Game::Entity(0, Util::Math::Vector2D(-0.5, -0.313)) {}

void Logo::initialize() {
    sprite = Util::Game::Sprite("initrd/mouse/logo.bmp", 1, 0.626);
}

void Logo::onUpdate(double delta) {}

void Logo::onTranslationEvent(Util::Game::TranslationEvent &event) {
    const auto &resolution = Util::Game::GameManager::getRelativeResolution();
    const auto &newPosition = event.getTargetPosition();

    if (newPosition.getX() < -resolution.getX() ||
        newPosition.getX() > resolution.getX() - sprite.getWidth() ||
        newPosition.getY() < -resolution.getY() ||
        newPosition.getY() > resolution.getY() - sprite.getHeight()) {
        event.cancel();
    }
}

void Logo::onCollisionEvent(Util::Game::CollisionEvent &event) {}

void Logo::draw(Util::Game::Graphics2D &graphics) {
    graphics.drawImage(getPosition(), sprite.getImage());
}
