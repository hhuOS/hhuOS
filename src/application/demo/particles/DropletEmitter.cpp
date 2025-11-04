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
 * You should have received a copy of the GNU General Public License
 * along with this program.  If not, see <http://www.gnu.org/licenses/>
 *
 * The particle demo is based on a bachelor's thesis, written by Abdulbasir Gümüs.
 * The original source code can be found here: https://git.hhu.de/bsinfo/thesis/ba-abgue101
 */

#include "DropletEmitter.h"

#include "lib/util/math/Math.h"
#include "lib/pulsar/2d/component/GravityComponent.h"
#include "lib/util/base/String.h"
#include "lib/pulsar/2d/Sprite.h"
#include "lib/pulsar/2d/collider/RectangleCollider.h"
#include "lib/pulsar/2d/event/CollisionEvent.h"
#include "lib/pulsar/2d/particle/Particle.h"
#include "lib/pulsar/Collider.h"
#include "lib/util/math/Vector2.h"
#include "Ground.h"
#include "lib/pulsar/2d/Entity.h"

DropletEmitter::DropletEmitter(const Util::Math::Vector2<double> &position) : Pulsar::D2::SingleTimeEmitter(TAG, PARTICLE_TAG, position) {}

void DropletEmitter::initialize() {
    SingleTimeEmitter::initialize();

    setMinEmissionRate(5);
    setMaxEmissionRate(5);
}

void DropletEmitter::draw([[maybe_unused]] Pulsar::Graphics &graphics) const {}

void DropletEmitter::onTranslationEvent([[maybe_unused]] Pulsar::D2::TranslationEvent &event) {}

void DropletEmitter::onCollisionEvent([[maybe_unused]] Pulsar::D2::CollisionEvent &event) {}

void DropletEmitter::onParticleInitialization(Pulsar::D2::Particle &particle) {
    auto angle = random.getRandomNumber() * Util::Math::PI_DOUBLE;

    particle.setSprite(Pulsar::D2::Sprite("/user/dino/particle/water.bmp", 0.005, 0.005));
    particle.setPosition(getPosition());
    particle.setVelocity(Util::Math::Vector2<double>(Util::Math::cosine(angle), Util::Math::sine(angle)));
    particle.setTimeToLive(-1);
    particle.setCollider(Pulsar::D2::RectangleCollider(particle.getPosition(), Util::Math::Vector2<double>(0.005, 0.005), Pulsar::Collider::STATIC));

    particle.addComponent(new Pulsar::D2::GravityComponent(particle, 2.5, 0.0025));
}

void DropletEmitter::onParticleUpdate(Pulsar::D2::Particle &particle, double delta) {
    particle.setAlpha(particle.getAlpha() - 1 * delta);
}

void DropletEmitter::onParticleCollision(Pulsar::D2::Particle &particle, Pulsar::D2::CollisionEvent &event) {
    if (event.getCollidedWidth().getTag() == Ground::TAG) {
        removeParticle(&particle);
    }
}

void DropletEmitter::onParticleDestruction([[maybe_unused]] Pulsar::D2::Particle &particle) {}
