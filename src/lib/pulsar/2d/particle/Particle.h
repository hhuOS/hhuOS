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

#ifndef HHUOS_LIB_PULSAR_2D_PARTICLE_H
#define HHUOS_LIB_PULSAR_2D_PARTICLE_H

#include <stddef.h>

#include "util/time/Timestamp.h"
#include "pulsar/2d/Entity.h"
#include "pulsar/2d/Sprite.h"

namespace Pulsar::D2 {

class Emitter;

/// Represents a single particle in a particle system.
/// Particles are created and managed by an `Emitter`. A particle has the following properties:
/// - Position and Velocity: Inherited from Entity, determines where the particle is and how it moves.
/// - Sprite: Visual representation of the particle.
/// - Scale: A factor that is applied to the size of the particle's sprite at rendering time.
/// - Alpha: Transparency level of the particle's sprite.
/// - Rotation Velocity: The speed at which the particle rotates over time (can be positive or negative).
/// - Time to Live: Optional lifespan of the particle, after which it is removed from the scene.
///
/// This class is not inheritable; all custom behavior should be implemented in the `Emitter`.
/// At creation, each particle is initialized with default properties and an invisible sprite.
/// The `Emitter` should set these properties in the `onParticleInitialization()` callback.
/// It can also update them over time in `onParticleUpdate()`, and handle collisions in `onParticleCollision()`.
class Particle final : public Entity {

public:
    /// Create a new particle instance with the given tag and reference to its parent emitter.
    Particle(size_t tag, Emitter &parent);

    /// Initialize the particle.
    /// method calls the emitter's `onParticleInitialization()` method to set up the particle's initial properties.
    void initialize() override;

    /// Update the particle's state. This method is called automatically each frame.
    /// It decreases the particle's time to live (if applicable) and calls the emitter's
    /// `onParticleUpdate()` method to allow custom behavior.
    void onUpdate(float delta) override;

    /// Draw the particle using its sprite at its current position.
    /// The particle's current rotation angle and alpha transparency are applied during rendering.
    void draw(Graphics &graphics) const override;

    /// Handle collision events. This method forwards the event to the emitter's
    /// `onParticleCollision()` method for custom collision handling.
    void onCollisionEvent(const CollisionEvent &event) override;

    /// Get the current scale of the particle's sprite.
    float getScale() const;

    /// Set the scale of the particle's sprite.
    void setScale(float scale);

    /// Get the current alpha transparency of the particle's sprite.
    float getAlpha() const;

    /// Set the alpha transparency of the particle's sprite.
    void setAlpha(float alpha);

    /// Get the rotation velocity of the particle (in degrees per second).
    float getRotationVelocity() const;

    /// Set the rotation velocity of the particle (in degrees per second).
    /// A negative value will rotate the particle counter-clockwise.
    void setRotationVelocity(float rotationVelocity);

    /// Set the time to live for the particle.
    /// If the time is greater than zero, the particle will be removed from the scene after that duration.
    /// If the time is zero, the particle will live indefinitely until removed by other means.
    void setTimeToLive(const Util::Time::Timestamp &timeToLive);

    /// Set the sprite used to render the particle.
    /// The `Sprite` class has a constructor that allows creating a square sprite from a color and size.
    /// This allows easily creating simple particle sprites (i.e. colored square).
    void setSprite(const Sprite &sprite);

private:

    float rotationVelocity = 0;
    bool timeLimited = false;
    float timeToLive = 0;

    Sprite sprite;
    Emitter &parent;
};

}

#endif
