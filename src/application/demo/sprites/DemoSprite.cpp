/*
 * Copyright (C) 2017-2025 Heinrich Heine University Düsseldorf,
 * Institute of Computer Science, Department Operating Systems
 * Main developers: Christian Gesse <christian.gesse@hhu.de>, Fabian Ruhland <ruhland@hhu.de>
 * Original development team: Burak Akguel, Christian Gesse, Fabian Ruhland, Filip Krakowski, Michael Schöttner
 * This project has been supported by several students.
 * A full list of integrated student theses can be found here: https://github.com/hhuOS/hhuOS/wiki/Student-theses
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
#include "lib/pulsar/2d/Sprite.h"
#include "lib/util/base/String.h"

DemoSprite::DemoSprite(const Util::Math::Vector2<double> &position, double size, double rotationSpeed, double scaleSpeed, bool flipX)
        : Pulsar::D2::Entity(TAG, position), initialPosition(position), size(size), rotationSpeed(rotationSpeed), scaleSpeed(scaleSpeed), flipX(flipX) {}

void DemoSprite::initialize() {
    animation = Pulsar::D2::SpriteAnimation(Util::Array<Pulsar::D2::Sprite>({
        Pulsar::D2::Sprite("/user/dino/player/run1.bmp", 0.2, 0.2267),
        Pulsar::D2::Sprite("/user/dino/player/run2.bmp", 0.2, 0.2267),
        Pulsar::D2::Sprite("/user/dino/player/run3.bmp", 0.2, 0.2267),
        Pulsar::D2::Sprite("/user/dino/player/run4.bmp", 0.2, 0.2267),
        Pulsar::D2::Sprite("/user/dino/player/run5.bmp", 0.2, 0.2267),
        Pulsar::D2::Sprite("/user/dino/player/run6.bmp", 0.2, 0.2267),
        Pulsar::D2::Sprite("/user/dino/player/dash1.bmp", 0.24, 0.2),
        Pulsar::D2::Sprite("/user/dino/player/dash2.bmp", 0.24, 0.2),
        Pulsar::D2::Sprite("/user/dino/player/dash3.bmp", 0.24, 0.2),
        Pulsar::D2::Sprite("/user/dino/player/dash4.bmp", 0.24, 0.2),
        Pulsar::D2::Sprite("/user/dino/player/dash5.bmp", 0.24, 0.2),
        Pulsar::D2::Sprite("/user/dino/player/dash6.bmp", 0.24, 0.2)}), 1.0);

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
    setPosition(initialPosition + Util::Math::Vector2<double>(positionOffset, positionOffset));
}

void DemoSprite::draw(Pulsar::Graphics &graphics) const {
    animation.draw(graphics, getPosition());
}

void DemoSprite::onTranslationEvent([[maybe_unused]] Pulsar::D2::TranslationEvent &event) {}

void DemoSprite::onCollisionEvent([[maybe_unused]] const Pulsar::D2::CollisionEvent &event) {}
