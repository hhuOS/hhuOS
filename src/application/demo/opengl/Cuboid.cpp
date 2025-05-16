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
 * The OpenGL demo has been created during a bachelor's thesis by Kevin Weber
 * The original source code can be found here: https://git.hhu.de/bsinfo/thesis/ba-keweb100
 */

#include "Cuboid.h"

#include "lib/util/game/Graphics.h"
#include "lib/util/game/3d/Orientation.h"
#include "lib/util/game/3d/Texture.h"

Cuboid::Cuboid(const Util::Math::Vector3<double> &position, const Util::Math::Vector3<double> &startRotation, const Util::Math::Vector3<double> &rotationAngle, const Util::Math::Vector3<double> &size, const Util::Graphic::Color &color) :
        Entity(0, position, startRotation, size), rotationAngle(rotationAngle), color(color) {}

Cuboid::Cuboid(const Util::Math::Vector3<double> &position, const Util::Math::Vector3<double> &startRotation, const Util::Math::Vector3<double> &rotationAngle, const Util::Math::Vector3<double> &size, const Util::String &texturePath) :
        Entity(0, position, startRotation, size), rotationAngle(rotationAngle), texturePath(texturePath) {}

void Cuboid::initialize() {
    if (!texturePath.isEmpty()) {
        texture = Util::Game::D3::Texture(texturePath);
    }
}

void Cuboid::onUpdate(double delta) {
    rotate(rotationAngle * delta * 60);
}

void Cuboid::draw(Util::Game::Graphics &graphics) {
    graphics.setColor(color);
    graphics.drawCuboid3D(getPosition(), getScale(), getOrientation().getRotation(), texture);
}

void Cuboid::onCollisionEvent([[maybe_unused]] Util::Game::D3::CollisionEvent &event) {}
