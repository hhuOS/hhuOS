/*
 * Copyright (C) 2017-2026 Heinrich Heine University Düsseldorf,
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
#include "lib/pulsar/2d/component/GravityComponent.h"
#include "lib/util/base/String.h"
#include "lib/pulsar/2d/Sprite.h"
#include "lib/pulsar/2d/collider/RectangleCollider.h"
#include "lib/pulsar/2d/particle/Particle.h"
#include "lib/pulsar/2d/event/CollisionEvent.h"
#include "lib/pulsar/Collider.h"
#include "lib/util/math/Vector2.h"
#include "application/dino/entity/Block.h"
#include "lib/util/base/Panic.h"
#include "lib/pulsar/2d/Entity.h"

BloodEmitter::BloodEmitter(const Util::Math::Vector2<float> &position, Type type) : Pulsar::D2::OnceEmitter(TAG, PARTICLE_TAG, position, 25, 50), type(type) {}

void BloodEmitter::initialize() {}

void BloodEmitter::draw([[maybe_unused]] Pulsar::Graphics &graphics) const {}

void BloodEmitter::onTranslationEvent([[maybe_unused]] Pulsar::D2::TranslationEvent &event) {}

void BloodEmitter::onCollisionEvent([[maybe_unused]] const Pulsar::D2::CollisionEvent &event) {}

void BloodEmitter::onParticleInitialization(Pulsar::D2::Particle &particle) {
    auto angle = random.getRandomNumber() * Util::Math::PI_FLOAT;

    switch (type) {
        case WATER:
            particle.setSprite(Pulsar::D2::Sprite(Util::Graphic::Color(44, 197, 246), PARTICLE_SIZE, PARTICLE_SIZE));
            break;
        case ENEMY:
            particle.setSprite(Pulsar::D2::Sprite(Util::Graphic::Color(114, 161, 29), PARTICLE_SIZE, PARTICLE_SIZE));
            break;
        case PLAYER:
            particle.setSprite(Pulsar::D2::Sprite(Util::Graphic::Color(49, 217, 0), PARTICLE_SIZE, PARTICLE_SIZE));
            break;
    }
    particle.setPosition(getPosition());
    particle.setVelocity(Util::Math::Vector2<float>(Util::Math::cosine(angle), Util::Math::sine(angle)));
    particle.setTimeToLive(Util::Time::Timestamp::ofSecondsFloat<float>(10));
    particle.setCollider(Pulsar::D2::RectangleCollider(particle.getPosition(), PARTICLE_SIZE, PARTICLE_SIZE, Pulsar::D2::RectangleCollider::PERMEABLE));

    particle.addComponent(new Pulsar::D2::GravityComponent(2.5, 0.0025));
}

void BloodEmitter::onParticleUpdate(Pulsar::D2::Particle &particle, float delta) {
    particle.setAlpha(particle.getAlpha() - 1 * delta);
}

void BloodEmitter::onParticleCollision(Pulsar::D2::Particle &particle, const Pulsar::D2::CollisionEvent &event) {
    if (event.getCollidedWidth().getTag() == Block::GRASS || event.getCollidedWidth().getTag() == Block::DIRT || event.getCollidedWidth().getTag() == Block::WATER) {
        removeParticle(&particle);
    }
}

void BloodEmitter::onParticleDestruction([[maybe_unused]] const Pulsar::D2::Particle &particle) {}

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
