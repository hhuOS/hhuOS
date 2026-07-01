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

#ifndef HHUOS_APPLICATION_DEMO_RAINEMITTER_H
#define HHUOS_APPLICATION_DEMO_RAINEMITTER_H

#include <stddef.h>

#include <util/math/Random.h>
#include <util/math/Vector2.h>
#include <pulsar/2d/particle/Emitter.h>
#include <pulsar/2d/Sprite.h>

/// A cloud that moves horizontally across the screen and emits rain particles.
class RainEmitter : public Pulsar::D2::Emitter {

public:
    /// Create a new rain emitter instance at the given position.
    explicit RainEmitter(const Util::Math::Vector2<float> &position);

    /// Initialize the rain emitter by loading its sprite and the raindrop sprite.
    void initialize() override;

    /// Draw the cloud using its sprite.
    void draw(Pulsar::Graphics &graphics) const override;

    /// Check if the cloud hits a left or right boundary with the new position.
    /// If so, change the movement direction.
    void onTranslationEvent(Pulsar::D2::TranslationEvent &event) override;

    /// Initialize a raindrop particle.
    void onParticleInitialization(Pulsar::D2::Particle &particle) override;

    /// Handle raindrop particle collisions.
    /// If a raindrop collides with a dinosaur or the ground,
    /// it spawns new smaller water droplets and is removed from the scene.
    void onParticleCollision(Pulsar::D2::Particle &particle, const Pulsar::D2::CollisionEvent &event) override;

    /// Unique tag to distinguish the cloud from other object types in collision events.
    static constexpr size_t TAG = 0;
    /// Unique tag to distinguish raindrops from other object types in collision events.
    static constexpr size_t PARTICLE_TAG = 1;

private:

    Util::Math::Random random;
    Pulsar::D2::Sprite cloudSprite;
    Pulsar::D2::Sprite raindropSprite;

    static constexpr float SPEED = 0.25;
};

#endif
