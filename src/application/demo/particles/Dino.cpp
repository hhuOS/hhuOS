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
 *
 * The particle demo is based on a bachelor's thesis, written by Abdulbasir Gümüs.
 * The original source code can be found here: https://git.hhu.de/bsinfo/thesis/ba-abgue101
 */

#include "Dino.h"

#include "lib/util/base/String.h"
#include "lib/util/collection/Array.h"
#include "lib/pulsar/2d/Sprite.h"
#include "lib/pulsar/2d/collider/RectangleCollider.h"
#include "lib/pulsar/Collider.h"
#include "lib/util/math/Vector2.h"

Dino::Dino(const Util::Math::Vector2<double> &position, bool flipX) :
        Pulsar::D2::Entity(TAG, position, Pulsar::D2::RectangleCollider(position, SIZE, SIZE * 1.133, Pulsar::Collider::STATIC)),
        flipX(flipX) {}

void Dino::initialize() {
    animation = Pulsar::D2::SpriteAnimation(Util::Array<Pulsar::D2::Sprite>({
        Pulsar::D2::Sprite("/user/dino/player/idle1.bmp", SIZE, SIZE * 1.333),
        Pulsar::D2::Sprite("/user/dino/player/idle2.bmp", SIZE, SIZE * 1.333),
        Pulsar::D2::Sprite("/user/dino/player/idle3.bmp", SIZE, SIZE * 1.333)}), 0.5);

    if (flipX) {
        animation.flipX();
    }
}

void Dino::onUpdate(double delta) {
    animation.update(delta);
}

void Dino::onTranslationEvent([[maybe_unused]] Pulsar::D2::TranslationEvent &event) {}

void Dino::onCollisionEvent([[maybe_unused]] const Pulsar::D2::CollisionEvent &event) {}

void Dino::draw(Pulsar::Graphics &graphics) const {
    animation.draw(graphics, getPosition());
}
