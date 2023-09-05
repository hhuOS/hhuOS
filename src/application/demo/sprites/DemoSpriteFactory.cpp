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

#include "DemoSpriteFactory.h"

#include "application/demo/sprites/DemoSprite.h"
#include "lib/util/math/Vector2D.h"

DemoSprite *DemoSpriteFactory::createSprite() {
    const auto size = random.nextRandomNumber() * 0.15 + 0.1;
    const auto rotationSpeed = (random.nextRandomNumber() + 0.1) * (random.nextRandomNumber() < 0.5 ? -1 : 1);
    const auto scaleSpeed = random.nextRandomNumber();
    const auto position = Util::Math::Vector2D(random.nextRandomNumber() * 2 - 1 - size / 2, random.nextRandomNumber() * 2 - 1 - size / 2);
    const auto flipX = random.nextRandomNumber() < 0.5;

    return new DemoSprite(position, size, rotationSpeed, scaleSpeed, flipX);
}
