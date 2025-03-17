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
 *
 * Battle Space has been implemented during a bachelor's thesis by Richard Josef Schweitzer
 * The original source code can be found here: https://git.hhu.de/bsinfo/thesis/ba-risch114
 */

#ifndef HHUOS_ASTRONOMICAL_H
#define HHUOS_ASTRONOMICAL_H

#include <stdint.h>

#include "lib/util/game/3d/Model.h"
#include "lib/util/base/String.h"
#include "lib/util/math/Vector3.h"

namespace Util {
namespace Graphic {
class Color;
}  // namespace Graphic
}  // namespace Util

class Astronomical : public Util::Game::D3::Model {

public:
    /**
     * Constructor.
     */
    Astronomical(const Util::String &modelName, const Util::Math::Vector3<double> &position, double scale, const Util::Math::Vector3<double> &rotationVector, const Util::Graphic::Color &color);

    /**
     * Copy Constructor.
     */
    Astronomical(const Astronomical &other) = delete;

    /**
     * Assignment operator.
     */
    Astronomical &operator=(const Astronomical &other) = delete;

    /**
     * Destructor.
     */
    ~Astronomical() override = default;

    void onUpdate(double delta) override;

    void onCollisionEvent(Util::Game::D3::CollisionEvent &event) override;

    static const constexpr uint32_t TAG = 1;

private:

    Util::Math::Vector3<double> rotationVector;
};

#endif
