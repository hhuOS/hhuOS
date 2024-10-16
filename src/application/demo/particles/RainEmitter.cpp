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

#include "RainEmitter.h"

#include "lib/util/game/2d/event/TranslationEvent.h"
#include "lib/util/game/2d/component/LinearMovementComponent.h"
#include "DropletEmitter.h"
#include "lib/util/game/GameManager.h"
#include "lib/util/game/Scene.h"
#include "lib/util/base/String.h"
#include "lib/util/game/2d/collider/RectangleCollider.h"
#include "lib/util/game/2d/particle/Particle.h"
#include "lib/util/game/Collider.h"
#include "lib/util/math/Vector2D.h"

namespace Util {
namespace Game {
class Graphics;
namespace D2 {
class CollisionEvent;
}  // namespace D2
}  // namespace Game
}  // namespace Util

RainEmitter::RainEmitter(const Util::Math::Vector2D &position) : Util::Game::D2::Emitter(TAG, PARTICLE_TAG, position, -1) {}

void RainEmitter::initialize() {
    Emitter::initialize();
    cloudSprite = Util::Game::D2::Sprite("/user/dino/background/cloud3.bmp", 0.6, 0.15);

    setVelocityX(SPEED);
    setEmissionTime(0.2);
    setMinEmissionRate(2);
    setMaxEmissionRate(2);

    addComponent(new Util::Game::D2::LinearMovementComponent(*this));
}

void RainEmitter::onUpdate(double delta) {
    Emitter::onUpdate(delta);
}

void RainEmitter::draw(Util::Game::Graphics &graphics) {
    cloudSprite.draw(graphics, getPosition());
}

void RainEmitter::onTranslationEvent(Util::Game::D2::TranslationEvent &event) {
    if (event.getTargetPosition().getX() < -1) {
        setPositionX(-1);
        setVelocityX(SPEED);
    } else if (event.getTargetPosition().getX() > 0.6) {
        setPositionX(0.6);
        setVelocityX(-SPEED);
    }
}

void RainEmitter::onCollisionEvent([[maybe_unused]] Util::Game::D2::CollisionEvent &event) {}

void RainEmitter::onParticleInitialization(Util::Game::D2::Particle &particle) {
    particle.setSprite(Util::Game::D2::Sprite("/user/dino/particle/water.bmp", 0.005, 0.03));
    particle.setPosition(getPosition() + Util::Math::Vector2D(random.nextRandomNumber() * 0.5, 0));
    particle.setVelocity(Util::Math::Vector2D(0, -0.8));
    particle.setTimeToLive(-1);
    particle.setCollider(Util::Game::D2::RectangleCollider(particle.getPosition(), Util::Math::Vector2D(0.005, 0.03), Util::Game::Collider::STATIC));

    particle.addComponent(new Util::Game::D2::LinearMovementComponent(particle));
}

void RainEmitter::onParticleUpdate([[maybe_unused]] Util::Game::D2::Particle &particle, [[maybe_unused]] double delta) {}

void RainEmitter::onParticleCollision(Util::Game::D2::Particle &particle, [[maybe_unused]] Util::Game::D2::CollisionEvent &event) {
    removeParticle(&particle);
}

void RainEmitter::onParticleDestruction(Util::Game::D2::Particle &particle) {
    auto *dropletEmitter = new BloodEmitter(particle.getPosition());
    Util::Game::GameManager::getCurrentScene().addObject(dropletEmitter);
}
