/*
 * Copyright (C) 2018-2025 Heinrich-Heine-Universitaet Duesseldorf,
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

#include "Rectangle.h"
#include "lib/util/game/Graphics.h"
#include "lib/util/game/3d/event/CollisionEvent.h"

Rectangle::Rectangle(const Util::Math::Vector3<double> &position, const Util::Math::Vector3<double> &startRotation, const Util::Math::Vector3<double> &rotationAngle, const Util::Math::Vector2<double> &size, const Util::Graphic::Color &color)  :
        Entity(0, position, startRotation, Util::Math::Vector3<double>(size.getX(), size.getY(), 0)), rotationAngle(rotationAngle), color(color) {}

Rectangle::Rectangle(const Util::Math::Vector3<double> &position, const Util::Math::Vector3<double> &startRotation, const Util::Math::Vector3<double> &rotationAngle, const Util::Math::Vector2<double> &size, const Util::String &texturePath) :
        Entity(0, position, startRotation, Util::Math::Vector3<double>(size.getX(), size.getY(), 0)), rotationAngle(rotationAngle), texturePath(texturePath) {}

void Rectangle::initialize() {
    if (!texturePath.isEmpty()) {
        texture = Util::Game::D3::Texture(texturePath);
    }
}

void Rectangle::onUpdate(double delta) {
    rotate(rotationAngle * delta * 60);
}

void Rectangle::draw(Util::Game::Graphics &graphics) {
    graphics.setColor(color);
    graphics.drawRectangle3D(getPosition(), Util::Math::Vector2<double>(getScale().getX(), getScale().getY()), getOrientation(), texture);
}

void Rectangle::onCollisionEvent([[maybe_unused]] Util::Game::D3::CollisionEvent &event) {
}
