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

#include "DropletEmitter.h"

#include "lib/util/math/Math.h"
#include "lib/util/game/2d/component/GravityComponent.h"
#include "lib/util/base/String.h"
#include "lib/util/game/2d/Sprite.h"
#include "lib/util/game/2d/collider/RectangleCollider.h"
#include "lib/util/game/2d/event/CollisionEvent.h"
#include "lib/util/game/2d/particle/Particle.h"
#include "lib/util/game/Collider.h"
#include "lib/util/math/Vector2D.h"
#include "Ground.h"
#include "lib/util/game/2d/Entity.h"

namespace Util {
namespace Game {
class Graphics;

namespace D2 {
class TranslationEvent;
}  // namespace D2
}  // namespace Game
}  // namespace Util

BloodEmitter::BloodEmitter(const Util::Math::Vector2D &position) : Util::Game::D2::SingleTimeEmitter(TAG, PARTICLE_TAG, position) {}

void BloodEmitter::initialize() {
    SingleTimeEmitter::initialize();

    setMinEmissionRate(5);
    setMaxEmissionRate(5);
}

void BloodEmitter::draw([[maybe_unused]] Util::Game::Graphics &graphics) {}

void BloodEmitter::onTranslationEvent([[maybe_unused]] Util::Game::D2::TranslationEvent &event) {}

void BloodEmitter::onCollisionEvent([[maybe_unused]] Util::Game::D2::CollisionEvent &event) {}

void BloodEmitter::onParticleInitialization(Util::Game::D2::Particle &particle) {
    auto angle = random.nextRandomNumber() * Util::Math::PI;

    particle.setSprite(Util::Game::D2::Sprite("/user/dino/particle/water.bmp", 0.005, 0.005));
    particle.setPosition(getPosition());
    particle.setVelocity(Util::Math::Vector2D(Util::Math::cosine(angle), Util::Math::sine(angle)));
    particle.setTimeToLive(-1);
    particle.setCollider(Util::Game::D2::RectangleCollider(particle.getPosition(), Util::Math::Vector2D(0.005, 0.005), Util::Game::Collider::STATIC));

    particle.addComponent(new Util::Game::D2::GravityComponent(particle, 2.5, 0.0025));
}

void BloodEmitter::onParticleUpdate(Util::Game::D2::Particle &particle, double delta) {
    particle.setAlpha(particle.getAlpha() - 1 * delta);
}

void BloodEmitter::onParticleCollision(Util::Game::D2::Particle &particle, Util::Game::D2::CollisionEvent &event) {
    if (event.getCollidedWidth().getTag() == Ground::TAG) {
        removeParticle(&particle);
    }
}

void BloodEmitter::onParticleDestruction([[maybe_unused]] Util::Game::D2::Particle &particle) {}
