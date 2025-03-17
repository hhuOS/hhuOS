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
 * The particle system is based on a bachelor's thesis, written by Abdulbasir Gümüs.
 * The original source code can be found here: https://git.hhu.de/bsinfo/thesis/ba-abgue101
 */

#include "SingleTimeEmitter.h"

#include "lib/util/game/GameManager.h"
#include "lib/util/game/Scene.h"
#include "lib/util/game/2d/particle/Emitter.h"

namespace Util {
namespace Math {
template <typename T> class Vector2;
}  // namespace Math
}  // namespace Util

namespace Util::Game::D2 {

SingleTimeEmitter::SingleTimeEmitter(uint32_t tag, uint32_t particleTag, const Math::Vector2<double> &position) : Emitter(tag, particleTag, position, -1) {}

void SingleTimeEmitter::initialize() {
    Emitter::initialize();
    setEmissionTime(-1);
}

void SingleTimeEmitter::onUpdate(double delta) {
    Emitter::onUpdate(delta);

    if (!emitted) {
        emitOnce();
        emitted = true;
    } else if (getActiveParticles() == 0) {
        GameManager::getCurrentScene().removeObject(this);
    }
}

}