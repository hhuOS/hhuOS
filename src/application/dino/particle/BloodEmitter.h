/*
 * Copyright (C) 2018-2025 Heinrich-Heine-Universitaet Duesseldorf,
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
 * The dino game is based on a bachelor's thesis, written by Malte Sehmer.
 * The original source code can be found here: https://github.com/Malte2036/hhuOS
 */

#ifndef HHUOS_BLOODEMITTER_H
#define HHUOS_BLOODEMITTER_H

#include <stdint.h>

#include "lib/util/game/2d/particle/SingleTimeEmitter.h"
#include "lib/util/math/Random.h"

namespace Util {
namespace Math {
template <typename T> class Vector2;
}  // namespace Math
}  // namespace Util

class BloodEmitter : public Util::Game::D2::SingleTimeEmitter {

public:

    enum Type {
        WATER, ENEMY, PLAYER
    };

    /**
     * Constructor.
     */
    explicit BloodEmitter(const Util::Math::Vector2<double> &position, Type type);

    /**
     * Copy Constructor.
     */
    BloodEmitter(const BloodEmitter &other) = delete;

    /**
     * Assignment operator.
     */
    BloodEmitter &operator=(const BloodEmitter &other) = delete;

    /**
     * Destructor.
     */
    ~BloodEmitter() override = default;

    void initialize() override;

    void draw(Util::Game::Graphics &graphics) override;

    void onTranslationEvent(Util::Game::D2::TranslationEvent &event) override;

    void onCollisionEvent(Util::Game::D2::CollisionEvent &event) override;

    void onParticleInitialization(Util::Game::D2::Particle &particle) override;

    void onParticleUpdate(Util::Game::D2::Particle &particle, double delta) override;

    void onParticleCollision(Util::Game::D2::Particle &particle, Util::Game::D2::CollisionEvent &event) override;

    void onParticleDestruction(Util::Game::D2::Particle &particle) override;

    static const constexpr uint32_t TAG = 8;
    static const constexpr uint32_t PARTICLE_TAG = 9;

private:

    static const char* getParticleSpritePath(Type type);

    Type type;
    Util::Math::Random random;

    static const constexpr double PARTICLE_SIZE = 0.01;
};

#endif
