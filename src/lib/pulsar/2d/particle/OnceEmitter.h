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

#ifndef HHUOS_LIB_PULSAR_2D_ONCEEMITTER_H
#define HHUOS_LIB_PULSAR_2D_ONCEEMITTER_H

#include <stddef.h>
#include <stdint.h>

#include "pulsar/2d/particle/Emitter.h"

namespace Pulsar::D2 {

/// A particle emitter that emits particles only once upon creation.
/// After emitting the particles, it waits until all particles have expired before removing itself from the scene.
class OnceEmitter : public Emitter {

public:
    /// Create a new once emitter instance with its own tag and the tag for the particles it emits.
    /// The minimum and maximum emission rates define how many particles are emitted in total.
    /// The actual number of emitted particles is chosen randomly between these two values.
    OnceEmitter(size_t tag, size_t particleTag, const Util::Math::Vector2<float> &position,
		uint32_t minEmissionRate, uint32_t maxEmissionRate);

    /// Update the emitter state. This method is called automatically during the scene update cycle.
    /// On the first update, it emits the particles. Further updates just checks if all particles have expired
    /// and remove the emitter from the scene if so.
    void onUpdate(float delta) override;

private:

    bool emitted = false;
};

}

#endif
