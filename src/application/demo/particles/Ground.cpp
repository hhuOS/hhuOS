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

#include <util/base/String.h>
#include <util/math/Vector2.h>
#include <pulsar/2d/collider/RectangleCollider.h>

Ground::Ground(const Util::Math::Vector2<float> &position) : Entity(TAG, position,
    Pulsar::D2::RectangleCollider(position, WIDTH, HEIGHT, Pulsar::D2::RectangleCollider::STATIC)) {}

void Ground::initialize() {
    sprite = Pulsar::D2::Sprite("/user/dino/block/grass.bmp", HEIGHT, HEIGHT);
}

void Ground::draw(Pulsar::Graphics &graphics) const {
    const auto startX = getPosition().getX() - (WIDTH - 1) / 2;
    for (size_t i = 0; static_cast<float>(i) < WIDTH / sprite.getSize().getX(); i++) {
        sprite.draw(graphics, Util::Math::Vector2<float>(
            startX + static_cast<float>(i) * sprite.getSize().getY(), getPosition().getY()));
    }
}
