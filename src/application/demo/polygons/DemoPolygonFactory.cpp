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
 */

#include "DemoPolygonFactory.h"
#include "DemoPolygon.h"

#include <util/collection/Array.h>
#include <util/graphic/Color.h>
#include <util/math/Vector2.h>

const Util::Array<Util::Array<Util::Math::Vector2<float>>> DemoPolygonFactory::SHAPES = {
    // Triangle
    {{-1, -1}, {0, 1.0}, {1, -1}},
    // Square
    {{0, 1}, {1, 0}, {0, -1}, {-1, 0}},
    // Pentagon
    {{0, 1}, {1, 0.25}, {0.75, -1}, {-0.75, -1}, {-1, 0.25}},
    // Hexagon
    {{-0.5, 1}, {0.5, 1}, {1, 0}, {0.5, -1}, {-0.5, -1}, {-1, 0}},
    // Septagon
    {{0, 1}, {0.75, 0.75}, {1, -0.25}, {0.5, -1}, {-0.5, -1}, {-1, -0.25},
    {-0.75, 0.75}}
};

DemoPolygon* DemoPolygonFactory::createPolygon() {
    const auto shape = random.getRandomNumber(0, SHAPES.length() - 1);
    const auto initialScaleFactor = random.getRandomNumber<float>() + 0.1f;
    const auto rotationSpeed = random.getRandomNumber<float>() * 360 - 180;
    const auto scaleSpeed = random.getRandomNumber<float>();
    const auto position = Util::Math::Vector2<float>(random.getRandomNumber<float>() * 2 - 1.0f,
        random.getRandomNumber<float>() * 2 - 1.0f);
    const auto color = Util::Graphic::Color(random.getRandomNumber(0, 255),
        random.getRandomNumber(0, 255), random.getRandomNumber(0, 255));

    return new DemoPolygon(SHAPES[shape], position, color, initialScaleFactor, scaleSpeed, rotationSpeed);
}