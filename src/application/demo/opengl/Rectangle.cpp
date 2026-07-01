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

#include "Rectangle.h"

#include <pulsar/Graphics.h>
#include <pulsar/3d/Orientation.h>
#include <pulsar/3d/Texture.h>
#include <util/math/Vector2.h>

Rectangle::Rectangle(const Util::Math::Vector3<float> &position, const Util::Math::Vector3<float> &startRotation,
    const Util::Math::Vector3<float> &rotationAngle, const Util::Math::Vector2<float> &size,
    const Util::Graphic::Color &color) :
    Entity(0, position, startRotation, Util::Math::Vector2<float>(size.getX(), size.getY())),
    rotationAngle(rotationAngle), color(color) {}

Rectangle::Rectangle(const Util::Math::Vector3<float> &position, const Util::Math::Vector3<float> &startRotation,
    const Util::Math::Vector3<float> &rotationAngle, const Util::Math::Vector2<float> &size,
    const Util::String &texturePath) :
    Entity(0, position, startRotation, Util::Math::Vector2<float>(size.getX(), size.getY())),
    rotationAngle(rotationAngle), texturePath(texturePath) {}

void Rectangle::initialize() {
    if (!texturePath.isEmpty()) {
        texture = Pulsar::D3::Texture(texturePath);
    }
}

void Rectangle::onUpdate(const float delta) {
    rotate(rotationAngle * delta * 60);
}

void Rectangle::draw(Pulsar::Graphics &graphics) const {
    graphics.setColor(color);
    graphics.drawRectangle3D(getPosition(), static_cast<Util::Math::Vector2<float>>(getScale()),
        getOrientation().getRotation(), texture);
}
