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

#include "Dino.h"

#include "lib/util/base/String.h"
#include "lib/util/collection/Array.h"
#include "lib/util/game/2d/Sprite.h"
#include "lib/util/game/2d/collider/RectangleCollider.h"
#include "lib/util/game/Collider.h"
#include "lib/util/math/Vector2D.h"

namespace Util {
namespace Game {
class Graphics;
namespace D2 {
class CollisionEvent;
class TranslationEvent;
}  // namespace D2
}  // namespace Game
}  // namespace Util

Dino::Dino(const Util::Math::Vector2D &position, bool flipX) :
        Util::Game::D2::Entity(TAG, position, Util::Game::D2::RectangleCollider(position, Util::Math::Vector2D(SIZE, SIZE * 1.133), Util::Game::Collider::STATIC)),
        flipX(flipX) {}

void Dino::initialize() {
    animation = Util::Game::D2::SpriteAnimation(Util::Array<Util::Game::D2::Sprite>({
        Util::Game::D2::Sprite("/user/dino/player/idle1.bmp", SIZE, SIZE * 1.333),
        Util::Game::D2::Sprite("/user/dino/player/idle2.bmp", SIZE, SIZE * 1.333),
        Util::Game::D2::Sprite("/user/dino/player/idle3.bmp", SIZE, SIZE * 1.333)}), 0.5);

    if (flipX) {
        animation.flipX();
    }
}

void Dino::onUpdate(double delta) {
    animation.update(delta);
}

void Dino::onTranslationEvent([[maybe_unused]] Util::Game::D2::TranslationEvent &event) {}

void Dino::onCollisionEvent([[maybe_unused]] Util::Game::D2::CollisionEvent &event) {}

void Dino::draw(Util::Game::Graphics &graphics) {
    animation.draw(graphics, getPosition());
}
