/*
 * Copyright (C) 2017-2026 Heinrich Heine University Düsseldorf,
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

#include "Ground.h"

#include "lib/pulsar/Collider.h"
#include "lib/pulsar/2d/collider/RectangleCollider.h"
#include "lib/pulsar/2d/event/TranslationEvent.h"
#include "lib/util/math/Vector2.h"
#include "lib/util/base/String.h"

Ground::Ground(const Util::Math::Vector2<float> &position) : Pulsar::D2::Entity(TAG, position, Pulsar::D2::RectangleCollider(position, WIDTH, HEIGHT, Pulsar::D2::RectangleCollider::STATIC)) {}

void Ground::initialize() {
    sprite = Pulsar::D2::Sprite("/user/dino/block/grass.bmp", HEIGHT, HEIGHT);
}

void Ground::onUpdate([[maybe_unused]] float delta) {}

void Ground::onTranslationEvent(Pulsar::D2::TranslationEvent &event) {
    event.cancel();
}

void Ground::onCollisionEvent([[maybe_unused]] const Pulsar::D2::CollisionEvent &event) {}

void Ground::draw(Pulsar::Graphics &graphics) const {
    auto startX = getPosition().getX() - (WIDTH - 1) / 2;
    for (uint32_t i = 0; i < WIDTH / sprite.getSize().getX(); i++) {
        sprite.draw(graphics, Util::Math::Vector2<float>(startX + i * sprite.getSize().getY(), getPosition().getY()));
    }
}
