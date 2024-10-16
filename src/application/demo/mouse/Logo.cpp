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

#include "Logo.h"

#include "lib/util/game/2d/event/TranslationEvent.h"
#include "lib/util/game/GameManager.h"
#include "lib/util/math/Vector2D.h"
#include "lib/util/base/String.h"

namespace Util {
namespace Game {
namespace D2 {
class CollisionEvent;
}  // namespace D2
}  // namespace Game
}  // namespace Util

Logo::Logo() : Util::Game::D2::Entity(0, Util::Math::Vector2D(-0.5, -0.313)) {}

void Logo::initialize() {
    sprite = Util::Game::D2::Sprite("user/mouse/logo.bmp", 1, 0.626);
}

void Logo::onUpdate([[maybe_unused]] double delta) {}

void Logo::onTranslationEvent(Util::Game::D2::TranslationEvent &event) {
    const auto &resolution = Util::Game::GameManager::getRelativeResolution();
    const auto &newPosition = event.getTargetPosition();

    if (newPosition.getX() < -resolution.getX() ||
        newPosition.getX() > resolution.getX() - sprite.getSize().getX() ||
        newPosition.getY() < -resolution.getY() ||
        newPosition.getY() > resolution.getY() - sprite.getSize().getY()) {
        event.cancel();
    }
}

void Logo::onCollisionEvent([[maybe_unused]] Util::Game::D2::CollisionEvent &event) {}

void Logo::draw(Util::Game::Graphics &graphics) {
    sprite.draw(graphics, getPosition());
}
