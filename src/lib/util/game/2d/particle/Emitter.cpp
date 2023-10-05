/*
 * Copyright (C) 2018-2023 Heinrich-Heine-Universitaet Duesseldorf,
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

#include "Emitter.h"
#include "lib/util/game/GameManager.h"
#include "lib/util/game/Scene.h"
#include "Particle.h"

namespace Util::Game::D2 {

Emitter::Emitter(uint32_t tag, uint32_t particleTag, const Util::Math::Vector2D &position, double timeToLive) :
        Entity(tag, position), particleTag(particleTag), timeLimited(timeToLive > 0), timeToLive(timeToLive) {}

void Emitter::initialize() {}

void Emitter::onUpdate(double delta) {
    if (timeLimited) {
        timeToLive -= delta;
        if (timeToLive <= 0) {
            if (activeParticles.size() == 0) {
                GameManager::getCurrentScene().removeObject(this);
            }

            return;
        }
    }

    if (emissionTime >= 0) {
        timeSinceLastEmission += delta;
        if (timeSinceLastEmission >= emissionTime && (!timeLimited || timeToLive > 0)) {
            emitParticles();
            timeSinceLastEmission = 0;
        }
    }
}

void Emitter::removeParticle(Particle *particle) {
    onParticleDestruction(*particle);
    activeParticles.remove(particle);
    GameManager::getCurrentScene().removeObject(particle);
}

void Emitter::emitParticles() {
    uint32_t emissionRate = minEmissionRate + static_cast<uint32_t>(random.nextRandomNumber() * (maxEmissionRate - minEmissionRate));

    for (uint32_t i = 0; i < emissionRate; i++) {
        auto *particle = new Particle(particleTag, *this);
        activeParticles.add(particle);
        GameManager::getCurrentScene().addObject(particle);
    }
}

void Emitter::emitOnce() {
    emitParticles();
}

uint32_t Emitter::getMinEmissionRate() const {
    return minEmissionRate;
}

void Emitter::setMinEmissionRate(uint32_t minEmissionRate) {
    Emitter::minEmissionRate = minEmissionRate;
}

uint32_t Emitter::getMaxEmissionRate() const {
    return maxEmissionRate;
}

void Emitter::setMaxEmissionRate(uint32_t maxEmissionRate) {
    Emitter::maxEmissionRate = maxEmissionRate;
}

double Emitter::getEmissionTime() const {
    return emissionTime;
}

void Emitter::setEmissionTime(double emissionTime) {
    Emitter::emissionTime = emissionTime;
}

uint32_t Emitter::getActiveParticles() const {
    return activeParticles.size();
}

}