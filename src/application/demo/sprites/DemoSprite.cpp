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

#include "DemoSprite.h"

#include "lib/util/collection/Array.h"
#include "lib/util/game/2d/Sprite.h"
#include "lib/util/base/String.h"

namespace Util {
namespace Game {
class Graphics;

namespace D2 {
class CollisionEvent;
class TranslationEvent;
}  // namespace D2
}  // namespace Game
}  // namespace Util

DemoSprite::DemoSprite(const Util::Math::Vector2D &position, double size, double rotationSpeed, double scaleSpeed, bool flipX)
        : Util::Game::D2::Entity(TAG, position), initialPosition(position), size(size), rotationSpeed(rotationSpeed), scaleSpeed(scaleSpeed), flipX(flipX) {}

void DemoSprite::initialize() {
    animation = Util::Game::D2::SpriteAnimation(Util::Array<Util::Game::D2::Sprite>({
        Util::Game::D2::Sprite("/user/dino/player/run1.bmp", 0.2, 0.2267),
        Util::Game::D2::Sprite("/user/dino/player/run2.bmp", 0.2, 0.2267),
        Util::Game::D2::Sprite("/user/dino/player/run3.bmp", 0.2, 0.2267),
        Util::Game::D2::Sprite("/user/dino/player/run4.bmp", 0.2, 0.2267),
        Util::Game::D2::Sprite("/user/dino/player/run5.bmp", 0.2, 0.2267),
        Util::Game::D2::Sprite("/user/dino/player/run6.bmp", 0.2, 0.2267),
        Util::Game::D2::Sprite("/user/dino/player/dash1.bmp", 0.24, 0.2),
        Util::Game::D2::Sprite("/user/dino/player/dash2.bmp", 0.24, 0.2),
        Util::Game::D2::Sprite("/user/dino/player/dash3.bmp", 0.24, 0.2),
        Util::Game::D2::Sprite("/user/dino/player/dash4.bmp", 0.24, 0.2),
        Util::Game::D2::Sprite("/user/dino/player/dash5.bmp", 0.24, 0.2),
        Util::Game::D2::Sprite("/user/dino/player/dash6.bmp", 0.24, 0.2)}), 1.0);

    if (flipX) {
        animation.flipX();
    }
}

void DemoSprite::onUpdate(double delta) {
    animation.update(delta);

    if (animation.getScale().getX() >= 2) {
        scaleUp = false;
    } else if (animation.getScale().getX() <= 0.5) {
        scaleUp = true;
    }

    animation.setScale(scaleUp ? animation.getScale().getX() + delta * scaleSpeed : animation.getScale().getX() - delta * scaleSpeed);
    animation.rotate(delta * rotationSpeed);

    auto positionOffset = (animation.getOriginalSize().getX() - animation.getSize().getX()) / 2;
    setPosition(initialPosition + Util::Math::Vector2D(positionOffset, positionOffset));
}

void DemoSprite::draw(Util::Game::Graphics &graphics) {
    animation.draw(graphics, getPosition());
}

void DemoSprite::onTranslationEvent([[maybe_unused]] Util::Game::D2::TranslationEvent &event) {}

void DemoSprite::onCollisionEvent([[maybe_unused]] Util::Game::D2::CollisionEvent &event) {}
