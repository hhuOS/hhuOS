/*
 * Copyright (C) 2017-2026 Heinrich Heine University Düsseldorf,
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

#include "Emitter.h"

#include "pulsar/2d/particle/Particle.h"

namespace Pulsar {
namespace D2 {

void Emitter::onUpdate(const float delta) {
    if (timeLimited) {
        timeToLive -= delta;
        if (timeToLive <= 0) {
            if (activeParticles.size() == 0) {
                removeFromScene();
            }
        }
    } else if (emissionInterval > 0) {
        timeSinceLastEmission += delta;
        if (timeSinceLastEmission >= emissionInterval && (!timeLimited || timeToLive > 0)) {
            emitParticles();
            timeSinceLastEmission = 0;
        }
    }
}

void Emitter::removeParticle(const Particle *particle) {
    onParticleDestruction(*particle);
    if (activeParticles.remove(const_cast<Particle*>(particle))) {
        getScene().removeEntity(particle);
    }
}

void Emitter::emitParticles() {
    const auto emissionRate = random.getRandomNumber(minEmissionRate, maxEmissionRate);

    for (uint32_t i = 0; i < emissionRate; i++) {
        auto *particle = new Particle(particleTag, *this);
        activeParticles.add(particle);
        getScene().addEntity(particle);
    }
}

void Emitter::destroy(const bool immediate) {
    timeToLive = 0;
    timeLimited = true;

    if (immediate) {
        for (const auto *particle : activeParticles) {
            getScene().removeEntity(particle);
        }

        removeFromScene();
    }
}

}
}