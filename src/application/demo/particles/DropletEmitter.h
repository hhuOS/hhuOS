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
 * The particle demo is based on a bachelor's thesis, written by Abdulbasir Gümüs.
 * The original source code can be found here: https://git.hhu.de/bsinfo/thesis/ba-abgue101
 */

#ifndef HHUOS_APPLICAITON_DEMO_DROPLETEMITTER_H
#define HHUOS_APPLICAITON_DEMO_DROPLETEMITTER_H

#include <stddef.h>

#include <util/math/Random.h>
#include <util/math/Vector2.h>
#include <pulsar/2d/Sprite.h>
#include <pulsar/2d/particle/OnceEmitter.h>

/// An emitter that emits small rain droplets only one time.
/// It removes itself automatically once none of its particles are remaining in the scene.
/// This emitter is used to simulate larger raindrops splashing into multiple small droplets when they hit the ground.
class DropletEmitter : public Pulsar::D2::OnceEmitter {

public:
    /// Create a new droplet emitter instance at the given position.
    explicit DropletEmitter(const Util::Math::Vector2<float> &position);

    /// Initialize the emitter instance by loading the droplet sprite.
    void initialize() override;

    /// Initialize a droplet particle.
    void onParticleInitialization(Pulsar::D2::Particle &particle) override;

    /// Decrease the particle's alpha value, so that slowly fades over time.
    void onParticleUpdate(Pulsar::D2::Particle &particle, float delta) override;

    /// Handle droplet particle collisions.
    /// If a droplet collides with the ground, it is removed from the scene.
    void onParticleCollision(Pulsar::D2::Particle &particle, const Pulsar::D2::CollisionEvent &event) override;

    /// Unique tag to distinguish the droplet emitter from other object types in collision events.
    static constexpr size_t TAG = 2;
    /// Unique tag to distinguish droplets from other object types in collision events.
    static constexpr size_t PARTICLE_TAG = 3;

private:

    bool emitted = false;
    Util::Math::Random random;

    Pulsar::D2::Sprite dropletSprite;
};

#endif
