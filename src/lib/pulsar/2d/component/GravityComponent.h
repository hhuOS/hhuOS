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
 * The game engine is based on a bachelor's thesis, written by Malte Sehmer.
 * The original source code can be found here: https://github.com/Malte2036/hhuOS
 *
 * It has been enhanced with 3D-capabilities during a bachelor's thesis by Richard Josef Schweitzer
 * The original source code can be found here: https://git.hhu.de/bsinfo/thesis/ba-risch114
 *
 * The 3D-rendering has been rewritten using OpenGL (TinyGL) during a bachelor's thesis by Kevin Weber
 * The original source code can be found here: https://git.hhu.de/bsinfo/thesis/ba-keweb100
 *
 * The 2D particle system is based on a bachelor's thesis, written by Abdulbasir Gümüs.
 * The original source code can be found here: https://git.hhu.de/bsinfo/thesis/ba-abgue101
 */

#ifndef HHUOS_LIB_PULSAR_2D_GRAVITYCOMPONENT_H
#define HHUOS_LIB_PULSAR_2D_GRAVITYCOMPONENT_H

#include "Component.h"

namespace Pulsar::D2 {

/// A component that applies a gravity effect to the entity every frame.
/// This component modifies the entity's velocity by adding a downward force each update cycle,
/// simulating the effect of gravity. Additionally, it applies a horizontal stop factor to gradually
/// reduce the entity's horizontal velocity over time, simulating friction or air resistance.
/// By adding this component to an entity, the entity will experience a constant downward acceleration,
/// making it fall towards the ground, while also slowing down its horizontal movement.
class GravityComponent final : public Component {

public:
    /// Create a new gravity component instance.
    /// The `gravityValue` parameter defines the strength of the gravitational pull applied to the entity.
    /// A higher value results in a stronger downward force.
    /// The `stopFactorX` parameter defines the factor by which the entity's horizontal velocity
    /// is reduced each update cycle. A value of 0 means no reduction, while a value of 1 means
    /// the horizontal velocity is completely stopped immediately.
    /// The default values are set to simulate a somewhat realistic gravity effect.
    explicit GravityComponent(double gravityValue = 1.25, double stopFactorX = 0.15);

protected:
    /// Update the entity's velocity and position based on gravity and horizontal stop factor.
    /// This method is called every frame by the entity that owns this component.
    void update(double delta) override;

private:

    const double gravityValue;
    const double stopFactorX;
};

}

#endif
