/*
 * Copyright (C) 2018-2024 Heinrich-Heine-Universitaet Duesseldorf,
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
 *
 * Battle Space has been implemented during a bachelor's thesis by Richard Josef Schweitzer
 * The original source code can be found here: https://git.hhu.de/bsinfo/thesis/ba-risch114
 */

#include "Astronomical.h"

namespace Util {
namespace Game {
namespace D3 {
class CollisionEvent;
}  // namespace D3
}  // namespace Game
namespace Graphic {
class Color;
}  // namespace Graphic
}  // namespace Util

Astronomical::Astronomical(const Util::String &modelName, const Util::Math::Vector3D &position, double scale, const Util::Math::Vector3D &rotationVector, const Util::Graphic::Color &color) : Util::Game::D3::Model(TAG, Util::String::format("/user/battlespace/%s.obj", static_cast<const char*>(modelName)), position, Util::Math::Vector3D(0, 0, 0), Util::Math::Vector3D(scale, scale, scale), color), rotationVector(rotationVector) {}

void Astronomical::onUpdate(double delta) {
    rotate(rotationVector * delta);
}

void Astronomical::onCollisionEvent([[maybe_unused]] Util::Game::D3::CollisionEvent &event) {}
