/*
 * Copyright (C) 2018-2024 Heinrich-Heine-Universitaet Duesseldorf,
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
 * The particle demo is based on a bachelor's thesis, written by Abdulbasir Gümüs.
 * The original source code can be found here: https://git.hhu.de/bsinfo/thesis/ba-abgue101
 */

#ifndef HHUOS_DROPLETEMITTER_H
#define HHUOS_DROPLETEMITTER_H

#include <cstdint>

#include "lib/util/game/2d/particle/SingleTimeEmitter.h"
#include "lib/util/math/Random.h"

namespace Util {
namespace Game {
class Graphics;
namespace D2 {
class CollisionEvent;
class Particle;
class TranslationEvent;
}  // namespace D2
}  // namespace Game
namespace Math {
class Vector2D;
}  // namespace Math
}  // namespace Util

class DropletEmitter : public Util::Game::D2::SingleTimeEmitter {

public:
    /**
     * Default.
     */
    explicit DropletEmitter(const Util::Math::Vector2D &position);

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

    void draw(Util::Game::Graphics &graphics) override;

    void onTranslationEvent(Util::Game::D2::TranslationEvent &event) override;

    void onCollisionEvent(Util::Game::D2::CollisionEvent &event) override;

    void onParticleInitialization(Util::Game::D2::Particle &particle) override;

    void onParticleUpdate(Util::Game::D2::Particle &particle, double delta) override;

    void onParticleDestruction(Util::Game::D2::Particle &particle) override;

    static const constexpr uint32_t TAG = 2;
    static const constexpr uint32_t PARTICLE_TAG = 3;

private:

    bool emitted = false;
    Util::Math::Random random;
};

#endif
