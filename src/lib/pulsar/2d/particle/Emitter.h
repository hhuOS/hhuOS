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
 * The particle system is based on a bachelor's thesis, written by Abdulbasir Gümüs.
 * The original source code can be found here: https://git.hhu.de/bsinfo/thesis/ba-abgue101
 */

#ifndef HHUOS_EMITTER_H
#define HHUOS_EMITTER_H

#include <stdint.h>

#include "Particle.h"
#include "lib/pulsar/2d/Entity.h"
#include "lib/util/math/Random.h"
#include "lib/util/collection/ArrayList.h"

namespace Util {
namespace Math {
template <typename T> class Vector2;
}  // namespace Math

namespace Pulsar {
namespace D2 {
class CollisionEvent;
}  // namespace D2
}  // namespace Pulsar

}  // namespace Util

namespace Pulsar::D2 {

class Emitter : public Entity {

public:
    /**
     * Constructor.
     */
    Emitter(uint32_t tag, uint32_t particleTag, const Util::Math::Vector2<double> &position, double timeToLive);

    /**
     * Copy Constructor.
     */
    Emitter(const Emitter &other) = delete;

    /**
     * Assignment operator.
     */
    Emitter &operator=(const Emitter &other) = delete;

    /**
     * Destructor.
     */
    ~Emitter() override = default;

    void initialize() override;

    void onUpdate(double delta) override;

    void removeParticle(Particle *particle);

    void emitOnce();

    virtual void onParticleInitialization(Particle &particle) = 0;

    virtual void onParticleUpdate(Particle &particle, double delta) = 0;

    virtual void onParticleCollision(Particle &particle, CollisionEvent &event) = 0;

    virtual void onParticleDestruction(Particle &particle) = 0;

    [[nodiscard]] uint32_t getMinEmissionRate() const;

    void setMinEmissionRate(uint32_t minEmissionRate);

    [[nodiscard]] uint32_t getMaxEmissionRate() const;

    void setMaxEmissionRate(uint32_t maxEmissionRate);

    [[nodiscard]] double getEmissionTime() const;

    void setEmissionTime(double emissionTime);

    [[nodiscard]] uint32_t getActiveParticles() const;

private:

    void emitParticles();

    Util::Math::Random random;
    Util::ArrayList<Particle*> activeParticles;

    uint32_t particleTag;
    bool timeLimited;
    double timeToLive;

    uint32_t minEmissionRate = 2;
    uint32_t maxEmissionRate = 2;

    double timeSinceLastEmission = 0;
    double emissionTime = 0.1;
};

}

#endif
