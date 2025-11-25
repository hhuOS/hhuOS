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

#include "GrassEmitter.h"

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
#include "lib/pulsar/2d/Entity.h"

GrassEmitter::GrassEmitter(const Entity &parent) : Pulsar::D2::Emitter(TAG, PARTICLE_TAG, parent.getPosition(), 0, 1), parent(parent) {}

void GrassEmitter::initialize() {}

void GrassEmitter::draw([[maybe_unused]] Pulsar::Graphics &graphics) const {}

void GrassEmitter::onTranslationEvent([[maybe_unused]] Pulsar::D2::TranslationEvent &event) {}

void GrassEmitter::onCollisionEvent([[maybe_unused]] const Pulsar::D2::CollisionEvent &event) {}

void GrassEmitter::onParticleInitialization(Pulsar::D2::Particle &particle) {
    auto angle = random.getRandomNumber() * (Util::Math::PI_FLOAT / 4) + (Util::Math::PI_FLOAT / 8);
    auto velocityX = (Util::Math::cosine(angle) / 8) * (parent.getVelocity().getX() > 0 ? -1 : 1);
    auto velocityY = Util::Math::sine(angle);

    particle.setSprite(Pulsar::D2::Sprite(Util::Graphic::Color(54, 227, 119), PARTICLE_SIZE, PARTICLE_SIZE));
    particle.setPosition(parent.getPosition());
    particle.setVelocity(Util::Math::Vector2<float>(velocityX, velocityY));
    particle.setTimeToLive(Util::Time::Timestamp::ofSecondsFloat<float>(10));
    particle.setCollider(Pulsar::D2::RectangleCollider(particle.getPosition(), PARTICLE_SIZE, PARTICLE_SIZE, Pulsar::D2::RectangleCollider::PERMEABLE));

    particle.addComponent(new Pulsar::D2::GravityComponent(2.5, 0.0025));
}

void GrassEmitter::onParticleUpdate(Pulsar::D2::Particle &particle, float delta) {
    particle.setAlpha(particle.getAlpha() - 1 * delta);
}

void GrassEmitter::onParticleCollision(Pulsar::D2::Particle &particle, const Pulsar::D2::CollisionEvent &event) {
    if (event.getCollidedWidth().getTag() == Block::GRASS || event.getCollidedWidth().getTag() == Block::DIRT || event.getCollidedWidth().getTag() == Block::WATER) {
        removeParticle(&particle);
    }
}

void GrassEmitter::onParticleDestruction([[maybe_unused]] const Pulsar::D2::Particle &particle) {}
