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
 * The dino game is based on a bachelor's thesis, written by Malte Sehmer.
 * The original source code can be found here: https://github.com/Malte2036/hhuOS
 */

#include "BloodEmitter.h"

#include "lib/util/math/Math.h"
#include "lib/util/game/2d/component/GravityComponent.h"
#include "lib/util/base/String.h"
#include "lib/util/game/2d/Sprite.h"
#include "lib/util/game/2d/collider/RectangleCollider.h"
#include "lib/util/game/2d/particle/Particle.h"
#include "lib/util/game/2d/event/CollisionEvent.h"
#include "lib/util/game/Collider.h"
#include "lib/util/math/Vector2.h"
#include "application/dino/entity/Block.h"
#include "lib/util/base/Panic.h"
#include "lib/util/game/2d/Entity.h"

BloodEmitter::BloodEmitter(const Util::Math::Vector2<double> &position, Type type) : Util::Game::D2::SingleTimeEmitter(TAG, PARTICLE_TAG, position), type(type) {}

void BloodEmitter::initialize() {
    SingleTimeEmitter::initialize();

    setMinEmissionRate(25);
    setMaxEmissionRate(50);
}

void BloodEmitter::draw([[maybe_unused]] Util::Game::Graphics &graphics) {}

void BloodEmitter::onTranslationEvent([[maybe_unused]] Util::Game::D2::TranslationEvent &event) {}

void BloodEmitter::onCollisionEvent([[maybe_unused]] Util::Game::D2::CollisionEvent &event) {}

void BloodEmitter::onParticleInitialization(Util::Game::D2::Particle &particle) {
    auto angle = random.getRandomNumber() * Util::Math::PI_DOUBLE;

    particle.setSprite(Util::Game::D2::Sprite(getParticleSpritePath(type), PARTICLE_SIZE, PARTICLE_SIZE));
    particle.setPosition(getPosition());
    particle.setVelocity(Util::Math::Vector2<double>(Util::Math::cosine(angle), Util::Math::sine(angle)));
    particle.setTimeToLive(10);
    particle.setCollider(Util::Game::D2::RectangleCollider(particle.getPosition(), Util::Math::Vector2<double>(PARTICLE_SIZE, PARTICLE_SIZE), Util::Game::Collider::PERMEABLE));

    particle.addComponent(new Util::Game::D2::GravityComponent(particle, 2.5, 0.0025));
}

void BloodEmitter::onParticleUpdate(Util::Game::D2::Particle &particle, double delta) {
    particle.setAlpha(particle.getAlpha() - 1 * delta);
}

void BloodEmitter::onParticleCollision(Util::Game::D2::Particle &particle, Util::Game::D2::CollisionEvent &event) {
    if (event.getCollidedWidth().getTag() == Block::GRASS || event.getCollidedWidth().getTag() == Block::DIRT || event.getCollidedWidth().getTag() == Block::WATER) {
        removeParticle(&particle);
    }
}

void BloodEmitter::onParticleDestruction([[maybe_unused]] Util::Game::D2::Particle &particle) {}

const char* BloodEmitter::getParticleSpritePath(BloodEmitter::Type type) {
    switch (type) {
        case WATER:
            return "/user/dino/particle/water.bmp";
        case ENEMY:
            return "/user/dino/particle/enemy.bmp";
        case PLAYER:
            return "/user/dino/particle/player.bmp";
        default:
            Util::Panic::fire(Util::Panic::INVALID_ARGUMENT, "Invalid particle type!");
    }
}
