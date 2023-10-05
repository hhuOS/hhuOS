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
 */

#ifndef HHUOS_EMITTER_H
#define HHUOS_EMITTER_H

#include "lib/util/game/2d/Entity.h"
#include "lib/util/game/Sprite.h"
#include "lib/util/math/Random.h"
#include "Particle.h"

namespace Util::Game::D2 {

class Emitter : public Entity {

public:
    /**
     * Constructor.
     */
    Emitter(uint32_t tag, const Util::Math::Vector2D &position, double timeToLive);

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

    virtual void setNextParticleAttributes() = 0;

    virtual void onParticleUpdate(Particle &particle, double delta) = 0;

    virtual void onParticleDestruction(Particle &particle) = 0;

    [[nodiscard]] uint32_t getMinEmissionRate() const;

    void setMinEmissionRate(uint32_t minEmissionRate);

    [[nodiscard]] uint32_t getMaxEmissionRate() const;

    void setMaxEmissionRate(uint32_t maxEmissionRate);

    [[nodiscard]] double getEmissionTime() const;

    void setEmissionTime(double emissionTime);

    [[nodiscard]] uint32_t getActiveParticles() const;

protected:

    Sprite particleSprite;
    double particleScale = 1.0;
    double particleAlpha = 1;

    uint32_t particleTag = 0xffffffff;
    double particleTimeToLive = 2.0;

    Math::Vector2D particlePosition = Math::Vector2D(0, 0);
    Math::Vector2D particleVelocity = Math::Vector2D(0.1, 0.1);
    Math::Vector2D particleAcceleration = Math::Vector2D(0, 0);
    double particleRotationVelocity = 0;

    Math::Vector2D particleColliderSize = Math::Vector2D(0, 0);
    RectangleCollider::Type particleColliderType = Collider::STATIC;

private:

    void emitParticles();

    Math::Random random;
    ArrayList<Particle*> activeParticles;

    uint32_t minEmissionRate = 2;
    uint32_t maxEmissionRate = 2;
    bool timeLimited;
    double timeToLive;

    double timeSinceLastEmission = 0;
    double emissionTime = 0.1;
};

}

#endif
