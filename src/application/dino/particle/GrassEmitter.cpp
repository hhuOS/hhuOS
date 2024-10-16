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
 * The dino game is based on a bachelor's thesis, written by Malte Sehmer.
 * The original source code can be found here: https://github.com/Malte2036/hhuOS
 */

#include "GrassEmitter.h"

#include "lib/util/math/Math.h"
#include "lib/util/game/2d/component/GravityComponent.h"
#include "lib/util/base/String.h"
#include "lib/util/game/2d/Sprite.h"
#include "lib/util/game/2d/collider/RectangleCollider.h"
#include "lib/util/game/2d/particle/Particle.h"
#include "lib/util/game/2d/event/CollisionEvent.h"
#include "lib/util/game/Collider.h"
#include "lib/util/math/Vector2D.h"
#include "application/dino/entity/Block.h"
#include "lib/util/game/2d/Entity.h"

namespace Util {
namespace Game {
class Graphics;

namespace D2 {
class TranslationEvent;
}  // namespace D2
}  // namespace Game
}  // namespace Util

GrassEmitter::GrassEmitter(const Entity &parent) : Util::Game::D2::Emitter(TAG, PARTICLE_TAG, parent.getPosition(), -1), parent(parent) {}

void GrassEmitter::initialize() {
    setMinEmissionRate(0);
    setMaxEmissionRate(1);
    setEmissionTime(-1);
}

void GrassEmitter::draw([[maybe_unused]] Util::Game::Graphics &graphics) {}

void GrassEmitter::onTranslationEvent([[maybe_unused]] Util::Game::D2::TranslationEvent &event) {}

void GrassEmitter::onCollisionEvent([[maybe_unused]] Util::Game::D2::CollisionEvent &event) {}

void GrassEmitter::onParticleInitialization(Util::Game::D2::Particle &particle) {
    auto angle = random.nextRandomNumber() * (Util::Math::PI / 4) + (Util::Math::PI / 8);
    auto velocityX = (Util::Math::cosine(angle) / 8) * (parent.getVelocity().getX() > 0 ? -1 : 1);
    auto velocityY = Util::Math::sine(angle);

    particle.setSprite(Util::Game::D2::Sprite("/user/dino/particle/grass.bmp", PARTICLE_SIZE, PARTICLE_SIZE));
    particle.setPosition(parent.getPosition());
    particle.setVelocity(Util::Math::Vector2D(velocityX, velocityY));
    particle.setTimeToLive(10);
    particle.setCollider(Util::Game::D2::RectangleCollider(particle.getPosition(), Util::Math::Vector2D(PARTICLE_SIZE, PARTICLE_SIZE), Util::Game::Collider::PERMEABLE));

    particle.addComponent(new Util::Game::D2::GravityComponent(particle, 2.5, 0.0025));
}

void GrassEmitter::onParticleUpdate(Util::Game::D2::Particle &particle, double delta) {
    particle.setAlpha(particle.getAlpha() - 1 * delta);
}

void GrassEmitter::onParticleCollision(Util::Game::D2::Particle &particle, Util::Game::D2::CollisionEvent &event) {
    if (event.getCollidedWidth().getTag() == Block::GRASS || event.getCollidedWidth().getTag() == Block::DIRT || event.getCollidedWidth().getTag() == Block::WATER) {
        removeParticle(&particle);
    }
}

void GrassEmitter::onParticleDestruction([[maybe_unused]] Util::Game::D2::Particle &particle) {}
