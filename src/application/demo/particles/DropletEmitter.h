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
 * The particle demo is based on a bachelor's thesis, written by Abdulbasir Gümüs.
 * The original source code can be found here: https://git.hhu.de/bsinfo/thesis/ba-abgue101
 */

#ifndef HHUOS_DROPLETEMITTER_H
#define HHUOS_DROPLETEMITTER_H

#include <stdint.h>

#include "lib/pulsar/2d/particle/OnceEmitter.h"
#include "lib/util/math/Random.h"

namespace Util {
namespace Math {
template <typename T> class Vector2;
}  // namespace Math
}  // namespace Util

class DropletEmitter : public Pulsar::D2::OnceEmitter {

public:
    /**
     * Default.
     */
    explicit DropletEmitter(const Util::Math::Vector2<float> &position);

    /**
     * Copy Constructor.
     */
    DropletEmitter(const DropletEmitter &other) = delete;

    /**
     * Assignment operator.
     */
    DropletEmitter &operator=(const DropletEmitter &other) = delete;

    /**
     * Destructor.
     */
    ~DropletEmitter() override = default;

    void initialize() override;

    void draw(Pulsar::Graphics &graphics) const override;

    void onTranslationEvent(Pulsar::D2::TranslationEvent &event) override;

    void onCollisionEvent(const Pulsar::D2::CollisionEvent &event) override;

    void onParticleInitialization(Pulsar::D2::Particle &particle) override;

    void onParticleUpdate(Pulsar::D2::Particle &particle, float delta) override;

    void onParticleCollision(Pulsar::D2::Particle &particle, const Pulsar::D2::CollisionEvent &event) override;

    void onParticleDestruction(const Pulsar::D2::Particle &particle) override;

    static const constexpr uint32_t TAG = 2;
    static const constexpr uint32_t PARTICLE_TAG = 3;

private:

    bool emitted = false;
    Util::Math::Random random;
};

#endif
