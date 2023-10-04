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

#include "DropletEmitter.h"
#include "lib/util/math/Math.h"
#include "lib/util/game/GameManager.h"
#include "lib/util/game/Scene.h"

DropletEmitter::DropletEmitter(const Util::Math::Vector2D &position) : Util::Game::D2::SingleTimeEmitter(TAG, position) {}

void DropletEmitter::initialize() {
    SingleTimeEmitter::initialize();

    setMinEmissionRate(5);
    setMaxEmissionRate(5);

    particleTag = PARTICLE_TAG;
    particleSprite = Util::Game::Sprite("/initrd/demo/raindrop.bmp", 0.005, 0.005);
    particleTimeToLive = -1;
    particlePosition = getPosition();
    particleVelocity = Util::Math::Vector2D(0.1, 0.25);
    particleAcceleration = Util::Math::Vector2D(0, -2);
    particleColliderSize = Util::Math::Vector2D(0.005, 0.005);
}

void DropletEmitter::draw(Util::Game::Graphics &graphics) {}

void DropletEmitter::onTranslationEvent(Util::Game::D2::TranslationEvent &event) {}

void DropletEmitter::onCollisionEvent(Util::Game::D2::CollisionEvent &event) {}

void DropletEmitter::setNextParticleAttributes() {
    auto angle = random.nextRandomNumber() * Util::Math::PI;
    particleVelocity = Util::Math::Vector2D(Util::Math::cosine(angle), Util::Math::sine(angle));
}

void DropletEmitter::onParticleUpdate(Util::Game::D2::Particle &particle, double delta) {}

void DropletEmitter::onParticleDestruction(Util::Game::D2::Particle &particle) {}
