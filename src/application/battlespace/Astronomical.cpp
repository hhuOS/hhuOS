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
 * Battle Space has been implemented during a bachelor's thesis by Richard Josef Schweitzer
 * The original source code can be found here: https://git.hhu.de/bsinfo/thesis/ba-risch114
 */

#include "Astronomical.h"

namespace Util {
namespace Graphic {
class Color;
}  // namespace Graphic
}  // namespace Util

Astronomical::Astronomical(const Util::String &modelName, const Util::Math::Vector3<double> &position, double scale, const Util::Math::Vector3<double> &rotationVector, const Util::Graphic::Color &color) : Util::Pulsar::D3::Model(TAG, Util::String::format("/user/battlespace/%s.obj", static_cast<const char*>(modelName)), position, Util::Math::Vector3<double>(0, 0, 0), Util::Math::Vector3<double>(scale, scale, scale), color), rotationVector(rotationVector) {}

void Astronomical::onUpdate(double delta) {
    rotate(rotationVector * delta);
}

void Astronomical::onCollisionEvent([[maybe_unused]] Util::Pulsar::D3::CollisionEvent &event) {}
