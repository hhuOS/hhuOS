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

#ifndef HHUOS_RAINEMITTER_H
#define HHUOS_RAINEMITTER_H

#include "lib/util/game/2d/particle/Emitter.h"

class RainEmitter : public Util::Game::D2::Emitter {

public:
    /**
     * Default.
     */
    explicit RainEmitter(const Util::Math::Vector2D &position);

    /**
     * Copy Constructor.
     */
    RainEmitter(const RainEmitter &other) = delete;

    /**
     * Assignment operator.
     */
    RainEmitter &operator=(const RainEmitter &other) = delete;

    /**
     * Destructor.
     */
    ~RainEmitter() override = default;

    void initialize() override;

    void onUpdate(double delta) override;

    void draw(Util::Game::Graphics &graphics) override;

    void onTranslationEvent(Util::Game::D2::TranslationEvent &event) override;

    void onCollisionEvent(Util::Game::D2::CollisionEvent &event) override;

    void onParticleInitialization(Util::Game::D2::Particle &particle) override;

    void onParticleUpdate(Util::Game::D2::Particle &particle, double delta) override;

    void onParticleDestruction(Util::Game::D2::Particle &particle) override;

    static const constexpr uint32_t TAG = 0;
    static const constexpr uint32_t PARTICLE_TAG = 1;

private:

    Util::Math::Random random;
    Util::Game::Sprite cloudSprite;

    static const constexpr double SPEED = 0.25;
};

#endif
