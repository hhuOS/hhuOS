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

#include "lib/util/game/Sprite.h"
#include "lib/util/collection/Array.h"
#include "Saw.h"
#include "lib/util/game/Graphics2D.h"
#include "lib/util/game/entity/collider/Collider.h"
#include "lib/util/game/entity/collider/RectangleCollider.h"
#include "lib/util/game/entity/event/TranslationEvent.h"
#include "lib/util/math/Vector2D.h"

namespace Util {
namespace Game {
class CollisionEvent;
}  // namespace Game
}  // namespace Util

Saw::Saw(const Util::Math::Vector2D &position) : Entity(TAG, Util::Math::Vector2D(position.getX(), position.getY()), Util::Game::RectangleCollider(position, Util::Game::Collider::STATIC, 0.2, 0.2)) {}

void Saw::initialize() {
    animation = Util::Game::SpriteAnimation(Util::Array<Util::Game::Sprite>({
        Util::Game::Sprite("/initrd/dino/saw1.bmp", getCollider().getWidth(), getCollider().getHeight()),
        Util::Game::Sprite("/initrd/dino/saw2.bmp", getCollider().getWidth(), getCollider().getHeight()),
        Util::Game::Sprite("/initrd/dino/saw3.bmp", getCollider().getWidth(), getCollider().getHeight()),
        Util::Game::Sprite("/initrd/dino/saw4.bmp", getCollider().getWidth(), getCollider().getHeight()),
        Util::Game::Sprite("/initrd/dino/saw5.bmp", getCollider().getWidth(), getCollider().getHeight()),
        Util::Game::Sprite("/initrd/dino/saw6.bmp", getCollider().getWidth(), getCollider().getHeight()),
        Util::Game::Sprite("/initrd/dino/saw7.bmp", getCollider().getWidth(), getCollider().getHeight()),
        Util::Game::Sprite("/initrd/dino/saw8.bmp", getCollider().getWidth(), getCollider().getHeight())}), 0.4);
}

void Saw::onUpdate(double delta) {
    animation.update(delta);
}

void Saw::onTranslationEvent(Util::Game::TranslationEvent &event) {
    event.cancel();
}

void Saw::onCollisionEvent(Util::Game::CollisionEvent &event) {}

void Saw::draw(Util::Game::Graphics2D &graphics) {
    graphics.drawImage(getPosition(), animation.getCurrentSprite().getImage());
}
