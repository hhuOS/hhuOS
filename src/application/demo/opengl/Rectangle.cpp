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

#include "Rectangle.h"

#include "lib/pulsar/Graphics.h"
#include "lib/pulsar/3d/Orientation.h"
#include "lib/pulsar/3d/Texture.h"
#include "lib/util/math/Vector2.h"

Rectangle::Rectangle(const Util::Math::Vector3<float> &position, const Util::Math::Vector3<float> &startRotation, const Util::Math::Vector3<float> &rotationAngle, const Util::Math::Vector2<float> &size, const Util::Graphic::Color &color)  :
        Entity(0, position, startRotation, Util::Math::Vector3<float>(size.getX(), size.getY(), 0)), rotationAngle(rotationAngle), color(color) {}

Rectangle::Rectangle(const Util::Math::Vector3<float> &position, const Util::Math::Vector3<float> &startRotation, const Util::Math::Vector3<float> &rotationAngle, const Util::Math::Vector2<float> &size, const Util::String &texturePath) :
        Entity(0, position, startRotation, Util::Math::Vector3<float>(size.getX(), size.getY(), 0)), rotationAngle(rotationAngle), texturePath(texturePath) {}

void Rectangle::initialize() {
    if (!texturePath.isEmpty()) {
        texture = Pulsar::D3::Texture(texturePath);
    }
}

void Rectangle::onUpdate(float delta) {
    rotate(rotationAngle * delta * 60);
}

void Rectangle::draw(Pulsar::Graphics &graphics) const {
    graphics.setColor(color);
    graphics.drawRectangle3D(getPosition(), Util::Math::Vector2<float>(getScale().getX(), getScale().getY()), getOrientation().getRotation(), texture);
}

void Rectangle::onCollisionEvent([[maybe_unused]] const Pulsar::D3::CollisionEvent &event) {
}
