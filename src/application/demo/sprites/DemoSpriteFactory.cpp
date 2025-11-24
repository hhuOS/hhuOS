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

#include "DemoSpriteFactory.h"

#include "application/demo/sprites/DemoSprite.h"
#include "lib/util/math/Vector2.h"

DemoSprite *DemoSpriteFactory::createSprite() {
    const auto size = random.getRandomNumber() * 0.15 + 0.1;
    const auto rotationSpeed = static_cast<int32_t>(random.getRandomNumber(0, 60)) - 30;
    const auto scaleSpeed = random.getRandomNumber();
    const auto position = Util::Math::Vector2<double>(random.getRandomNumber() * 2 - 1 - size / 2, random.getRandomNumber() * 2 - 1 - size / 2);
    const auto flipX = random.getRandomNumber() < 0.5;

    return new DemoSprite(position, size, rotationSpeed, scaleSpeed, flipX);
}
