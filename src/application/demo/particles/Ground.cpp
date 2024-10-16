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
 * The particle demo is based on a bachelor's thesis, written by Abdulbasir Gümüs.
 * The original source code can be found here: https://git.hhu.de/bsinfo/thesis/ba-abgue101
 */

#include "Ground.h"

#include "lib/util/game/Collider.h"
#include "lib/util/game/2d/collider/RectangleCollider.h"
#include "lib/util/game/2d/event/TranslationEvent.h"
#include "lib/util/math/Vector2D.h"
#include "lib/util/base/String.h"

namespace Util {
namespace Game {
namespace D2 {
class CollisionEvent;
}  // namespace D2
}  // namespace Game
}  // namespace Util

Ground::Ground(const Util::Math::Vector2D &position) : Util::Game::D2::Entity(TAG, position, Util::Game::D2::RectangleCollider(position, Util::Math::Vector2D(2, SIZE), Util::Game::Collider::STATIC)) {}

void Ground::initialize() {
    sprite = Util::Game::D2::Sprite("/user/dino/block/grass.bmp", SIZE, SIZE);
}

void Ground::onUpdate([[maybe_unused]] double delta) {}

void Ground::onTranslationEvent(Util::Game::D2::TranslationEvent &event) {
    event.cancel();
}

void Ground::onCollisionEvent([[maybe_unused]] Util::Game::D2::CollisionEvent &event) {}

void Ground::draw(Util::Game::Graphics &graphics) {
    auto startX = getPosition().getX() - (getCollider().getWidth() - 1) / 2;
    for (uint32_t i = 0; i < getCollider().getWidth() / sprite.getSize().getX(); i++) {
        sprite.draw(graphics, Util::Math::Vector2D(startX + i * sprite.getSize().getY(), getPosition().getY()));
    }
}
