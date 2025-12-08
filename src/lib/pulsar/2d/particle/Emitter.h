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

#ifndef HHUOS_LIB_PULSAR_2D_EMITTER_H
#define HHUOS_LIB_PULSAR_2D_EMITTER_H

#include <stddef.h>
#include <stdint.h>

#include "util/collection/ArrayList.h"
#include "util/math/Random.h"
#include "pulsar/2d/Entity.h"

namespace Pulsar {
namespace D2 {
class Particle;
} // namespace D2
} // namespace Pulsar

namespace Pulsar {
namespace D2 {

/// An emitter is an entity that emits particles over time. It works as a normal 2D entity and can be placed in a scene.
/// Emitters can be time-limited, meaning they will stop emitting particles after a certain time
/// and remove themselves from the scene once all particles are gone.
/// All emitters have a minimum and maximum emission rate, defining how many particles are emitted per emission cycle.
/// The number of particles emitted per cycle is chosen randomly between these two values.
/// Emissions happen at a fixed rate, determined by the emission interval property.
/// Particles be configured by overriding the provided virtual methods.
/// Particle emission happens automatically during the update cycle of the emitter, but can also be triggered manually
/// by calling `emitOnce()`.
class Emitter : public Entity {

public:
    /// Create a new emitter instance with its own tag and the tag for the particles it emits.
    /// The emission interval defines the time between emission cycles.
    /// A value of 0 means particles are not emitted automatically and must be triggered manually via `emitOnce()`.
    /// The time to live parameter defines how long the emitter will emit particles.
    /// A time to live of 0 means the emitter will emit particles indefinitely.
    Emitter(const size_t tag, const size_t particleTag, const Util::Math::Vector2<float> &position,
        const uint32_t minEmissionRate, const uint32_t maxEmissionRate,
        const Util::Time::Timestamp &emissionInterval = Util::Time::Timestamp::ofSeconds(0),
        const Util::Time::Timestamp &timeToLive = Util::Time::Timestamp::ofSeconds(0)) :
        Entity(tag, position), particleTag(particleTag), timeLimited(timeToLive.toNanoseconds() > 0),
        timeToLive(timeToLive.toSecondsFloat<float>()), minEmissionRate(minEmissionRate),
        maxEmissionRate(maxEmissionRate), emissionInterval(emissionInterval.toSecondsFloat<float>()) {}

    /// Update the emitter and its particles. This method is called once per frame.
    /// It handles particle emission and updates all active particles.
    /// If a subclass needs to perform additional update logic, it should override this method
    /// and call the base class implementation.
    void onUpdate(float delta) override;

    /// Remove a particle from the emitter's active particle list.
    /// This method is intended to be called by subclasses when a particle needs to be removed.
    /// For example, it can be called during or `onParticleCollision()` to remove particles that have collided.
    /// It is also called by the particle itself when its lifetime expires.
    void removeParticle(const Particle *particle);

    /// Emit particles immediately based on the current emission rate settings.
    /// Particle emitters automatically emit particles during their update cycle,
    /// but this method allows for manual triggering of particle emission.
    void emitOnce() {
        emitParticles();
    }

    /// Destroy the emitter.
    /// This method causes the emitter to stop emitting particles and removes it from the scene
    /// once none of its active particles remain. If `immediate` is set to true, the emitter is removed
    /// from the scene immediately together with all its active particles.
    void destroy(bool immediate = false);

    /// Called when a particle is initialized. Subclasses should override this method to configure
    /// the particle's initial properties (e.g. position, velocity, lifetime, color, size, etc.) and
    /// attach any necessary components.
    virtual void onParticleInitialization(Particle &particle) = 0;

    /// Called during each update cycle for every active particle. Subclasses should override this method to update
    /// the particle's properties over time (e.g. position, velocity, size, etc.)
    /// based on the elapsed time since the last update.
    virtual void onParticleUpdate(Particle &particle, float delta) = 0;

    /// Called when a particle collides with another entity. Subclasses should override this method to define
    /// custom collision behavior (e.g. bounce, destroy, etc.).
    virtual void onParticleCollision(Particle &particle, const CollisionEvent &event) = 0;

    /// Called when a particle is destroyed/removed. Subclasses should override this method to perform any necessary
    /// cleanup or effects (e.g. spawn new particles, play sound, etc.) when a particle is removed.
    virtual void onParticleDestruction(const Particle &particle) = 0;

    /// Get the minimum emission rate (particles per emission cycle).
    uint32_t getMinEmissionRate() const {
        return minEmissionRate;
    }

    /// Set the minimum emission rate (particles per emission cycle).
    void setMinEmissionRate(const uint32_t minEmissionRate) {
        Emitter::minEmissionRate = minEmissionRate;
    }

    /// Get the maximum emission rate (particles per emission cycle).
    uint32_t getMaxEmissionRate() const {
        return maxEmissionRate;
    }

    /// Set the maximum emission rate (particles per emission cycle).
    void setMaxEmissionRate(const uint32_t maxEmissionRate) {
        Emitter::maxEmissionRate = maxEmissionRate;
    }

    /// Get the emission interval (time between emission cycles in seconds).
    float getEmissionInterval() const {
        return emissionInterval;
    }

    /// Set the emission interval (time between emission cycles in seconds).
    void setEmissionTime(const float emissionInterval) {
        Emitter::emissionInterval = emissionInterval;
    }

    /// Get the number of currently active particles emitted by this emitter.
    size_t getActiveParticles() const {
        return activeParticles.size();
    }

private:

    void emitParticles();

    Util::Math::Random random;
    Util::ArrayList<Particle*> activeParticles;

    uint32_t particleTag;
    bool timeLimited;
    float timeToLive;

    uint32_t minEmissionRate;
    uint32_t maxEmissionRate;

    float emissionInterval;
    float timeSinceLastEmission = 0;
};

}
}

#endif
