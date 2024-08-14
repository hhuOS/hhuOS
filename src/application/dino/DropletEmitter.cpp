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

#include "DropletEmitter.h"

#include "lib/util/math/Math.h"
#include "lib/util/game/2d/component/GravityComponent.h"
#include "lib/util/base/String.h"
#include "lib/util/game/2d/Sprite.h"
#include "lib/util/game/2d/collider/RectangleCollider.h"
#include "lib/util/game/2d/particle/Particle.h"
#include "lib/util/game/2d/event/CollisionEvent.h"
#include "lib/util/game/Collider.h"
#include "lib/util/math/Vector2D.h"
#include "Block.h"

namespace Util {
namespace Game {
class Graphics;
namespace D2 {
class CollisionEvent;
class TranslationEvent;
}  // namespace D2
}  // namespace Game
}  // namespace Util

DropletEmitter::DropletEmitter(const Util::Math::Vector2D &position, Type type) : Util::Game::D2::SingleTimeEmitter(TAG, PARTICLE_TAG, position), type(type) {}

void DropletEmitter::initialize() {
    SingleTimeEmitter::initialize();

    setMinEmissionRate(25);
    setMaxEmissionRate(50);
}

void DropletEmitter::draw(Util::Game::Graphics &graphics) {}

void DropletEmitter::onTranslationEvent(Util::Game::D2::TranslationEvent &event) {}

void DropletEmitter::onCollisionEvent(Util::Game::D2::CollisionEvent &event) {}

void DropletEmitter::onParticleInitialization(Util::Game::D2::Particle &particle) {
    auto angle = random.nextRandomNumber() * Util::Math::PI;

    particle.setSprite(Util::Game::D2::Sprite(type == WATER ? "/user/dino/particle/water.bmp" : "/user/dino/particle/blood.bmp", SIZE, SIZE));
    particle.setPosition(getPosition());
    particle.setVelocity(Util::Math::Vector2D(Util::Math::cosine(angle), Util::Math::sine(angle)));
    particle.setTimeToLive(-1);
    particle.setCollider(Util::Game::D2::RectangleCollider(particle.getPosition(), Util::Math::Vector2D(SIZE, SIZE), Util::Game::Collider::PERMEABLE));

    particle.addComponent(new Util::Game::D2::GravityComponent(particle, 2.5, 0.0025));
}

void DropletEmitter::onParticleUpdate(Util::Game::D2::Particle &particle, double delta) {
    particle.setAlpha(particle.getAlpha() - 1 * delta);
}

void DropletEmitter::onParticleCollision(Util::Game::D2::Particle &particle, Util::Game::D2::CollisionEvent &event) {
    if (event.getCollidedWidth().getTag() == Block::GRASS || event.getCollidedWidth().getTag() == Block::DIRT || event.getCollidedWidth().getTag() == Block::WATER) {
        removeParticle(&particle);
    }
}

void DropletEmitter::onParticleDestruction(Util::Game::D2::Particle &particle) {}
