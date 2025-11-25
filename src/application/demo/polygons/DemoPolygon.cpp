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

#include "DemoPolygon.h"

#include "lib/pulsar/Graphics.h"
#include "lib/util/math/Vector2.h"

DemoPolygon::DemoPolygon() : Polygon(0, Util::Math::Vector2<float>(0, 0),
    Util::Array<Util::Math::Vector2<float>>(0)), rotationSpeed(0), scaleSpeed(0) {}

DemoPolygon::DemoPolygon(const Util::Array<Util::Math::Vector2<float>> &vertices,
    const Util::Math::Vector2<float> &position, const Util::Graphic::Color &color,
    float initialScaleFactor, float scaleSpeed, float rotationSpeed) :
    Polygon(0, position, vertices, color), rotationSpeed(rotationSpeed), scaleSpeed(scaleSpeed)
{
    scale(initialScaleFactor);
}

void DemoPolygon::onUpdate(const float delta) {
    const float rotationAngle = delta * rotationSpeed;
    const float scaleFactor = scaleUp ? 1 + (delta * scaleSpeed) : 1 - (delta * scaleSpeed);
    currentScale *= scaleFactor;
    if (currentScale >= 2) {
        scaleUp = false;
    } else if (currentScale <= 0.5) {
        scaleUp = true;
    }

    rotate(rotationAngle);
    scale(scaleFactor);
}
