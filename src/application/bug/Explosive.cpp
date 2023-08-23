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

#include "Explosive.h"

#include "lib/util/collection/Array.h"
#include "lib/util/game/Graphics.h"
#include "lib/util/game/Sprite.h"
#include "lib/util/game/2d/collider/RectangleCollider.h"

namespace Util {
namespace Math {
class Vector2D;
}  // namespace Math
}  // namespace Util

Explosive::Explosive(uint32_t tag, const Util::Math::Vector2D &position, const Util::Game::D2::RectangleCollider &collider) : Entity(tag, position, collider) {}

void Explosive::initialize() {
    auto size = getCollider().getHeight() > getCollider().getWidth() ? getCollider().getHeight() : getCollider().getWidth();
    animation = Util::Game::SpriteAnimation(Util::Array<Util::Game::Sprite>({
        Util::Game::Sprite("/initrd/bug/explosion1.bmp", size, size),
        Util::Game::Sprite("/initrd/bug/explosion2.bmp", size, size),
        Util::Game::Sprite("/initrd/bug/explosion3.bmp", size, size),
        Util::Game::Sprite("/initrd/bug/explosion4.bmp", size, size),
        Util::Game::Sprite("/initrd/bug/explosion5.bmp", size, size),
        Util::Game::Sprite("/initrd/bug/explosion6.bmp", size, size),
        Util::Game::Sprite("/initrd/bug/explosion7.bmp", size, size),
        Util::Game::Sprite("/initrd/bug/explosion8.bmp", size, size)}), 0.5);
}

void Explosive::onUpdate(double delta) {
    if (shouldExplode) {
        exploding = true;
        shouldExplode = false;
    } else if (exploding) {
        explosionTimer += delta;
        if (explosionTimer <= animation.getAnimationTime()) {
            animation.update(delta);
        }
    }
}

void Explosive::draw(Util::Game::Graphics &graphics) {
    graphics.drawImage2D(getPosition(), animation.getCurrentSprite().getImage());
}

void Explosive::explode() {
    if (!exploding) {
        shouldExplode = true;
    }
}

bool Explosive::isExploding() const {
    return exploding;
}

bool Explosive::hasExploded() const {
    return explosionTimer >= animation.getAnimationTime();
}
