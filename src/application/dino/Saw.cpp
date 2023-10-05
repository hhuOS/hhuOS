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

#include "lib/util/game/2d/Sprite.h"
#include "lib/util/collection/Array.h"
#include "Saw.h"
#include "lib/util/game/Collider.h"
#include "lib/util/game/2d/collider/RectangleCollider.h"
#include "lib/util/game/2d/event/TranslationEvent.h"
#include "lib/util/math/Vector2D.h"

namespace Util {
namespace Game {
namespace D2 {
class CollisionEvent;
}  // namespace D2
}  // namespace Game
}  // namespace Util

Saw::Saw(const Util::Math::Vector2D &position) : Entity(TAG, Util::Math::Vector2D(position.getX(), position.getY()), Util::Game::D2::RectangleCollider(position, Util::Math::Vector2D(0.2, 0.2), Util::Game::Collider::STATIC)) {}

void Saw::initialize() {
    animation = Util::Game::D2::SpriteAnimation(Util::Array<Util::Game::D2::Sprite>({
        Util::Game::D2::Sprite("/initrd/dino/saw1.bmp", getCollider().getWidth(), getCollider().getHeight()),
        Util::Game::D2::Sprite("/initrd/dino/saw2.bmp", getCollider().getWidth(), getCollider().getHeight()),
        Util::Game::D2::Sprite("/initrd/dino/saw3.bmp", getCollider().getWidth(), getCollider().getHeight()),
        Util::Game::D2::Sprite("/initrd/dino/saw4.bmp", getCollider().getWidth(), getCollider().getHeight()),
        Util::Game::D2::Sprite("/initrd/dino/saw5.bmp", getCollider().getWidth(), getCollider().getHeight()),
        Util::Game::D2::Sprite("/initrd/dino/saw6.bmp", getCollider().getWidth(), getCollider().getHeight()),
        Util::Game::D2::Sprite("/initrd/dino/saw7.bmp", getCollider().getWidth(), getCollider().getHeight()),
        Util::Game::D2::Sprite("/initrd/dino/saw8.bmp", getCollider().getWidth(), getCollider().getHeight())}), 0.4);
}

void Saw::onUpdate(double delta) {
    animation.update(delta);
}

void Saw::onTranslationEvent(Util::Game::D2::TranslationEvent &event) {
    event.cancel();
}

void Saw::onCollisionEvent(Util::Game::D2::CollisionEvent &event) {}

void Saw::draw(Util::Game::Graphics &graphics) {
    animation.draw(graphics, getPosition());
}
