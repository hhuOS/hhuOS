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

#ifndef HHUOS_LIB_PULSAR_2D_LINEARMOVEMENTCOMPONENT_H
#define HHUOS_LIB_PULSAR_2D_LINEARMOVEMENTCOMPONENT_H

#include "pulsar/2d/component/Component.h"

namespace Pulsar::D2 {

/// A component that updates the entity's position based on its velocity every frame.
/// Each 2D entity has a velocity vector that determines how much its position should change per second.
/// However, if the entity does not have a linear movement component, its position will not be updated automatically.
/// By adding this component to an entity, the entity's position will be updated every frame according to its velocity.
/// This allows for simple linear movement of entities in the 2D world.
class LinearMovementComponent final : public Component {

public:
    /// Create a new linear movement component instance.
    /// Since the velocity is stored in the entity itself, no parameters are needed.
    explicit LinearMovementComponent() = default;

protected:
    /// Update the entity's position based on its velocity and the elapsed time since the last update.
    /// This method is called every frame by the entity that owns this component.
    void update(double delta) override;
};

}

#endif
