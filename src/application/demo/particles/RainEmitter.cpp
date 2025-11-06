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

#include "RainEmitter.h"

#include "Dino.h"
#include "lib/pulsar/2d/event/TranslationEvent.h"
#include "lib/pulsar/2d/component/LinearMovementComponent.h"
#include "DropletEmitter.h"
#include "Ground.h"
#include "pulsar/2d/event/CollisionEvent.h"
#include "lib/pulsar/Scene.h"
#include "lib/util/base/String.h"
#include "lib/pulsar/2d/collider/RectangleCollider.h"
#include "lib/pulsar/2d/particle/Particle.h"
#include "lib/pulsar/Collider.h"
#include "lib/util/math/Vector2.h"

RainEmitter::RainEmitter(const Util::Math::Vector2<double> &position) : Pulsar::D2::Emitter(TAG, PARTICLE_TAG, position, 2, 2, Util::Time::Timestamp::ofMilliseconds(200)) {
    addComponent(new Pulsar::D2::LinearMovementComponent());
}

void RainEmitter::initialize() {
    cloudSprite = Pulsar::D2::Sprite("/user/dino/background/cloud3.bmp", 0.6, 0.15);
    setVelocityX(SPEED);
}

void RainEmitter::onUpdate(double delta) {
    Emitter::onUpdate(delta);
}

void RainEmitter::draw(Pulsar::Graphics &graphics) const {
    cloudSprite.draw(graphics, getPosition());
}

void RainEmitter::onTranslationEvent(Pulsar::D2::TranslationEvent &event) {
    if (event.getTargetPosition().getX() < -1) {
        setPositionX(-1);
        setVelocityX(SPEED);
    } else if (event.getTargetPosition().getX() > 0.6) {
        setPositionX(0.6);
        setVelocityX(-SPEED);
    }
}

void RainEmitter::onCollisionEvent([[maybe_unused]] const Pulsar::D2::CollisionEvent &event) {}

void RainEmitter::onParticleInitialization(Pulsar::D2::Particle &particle) {
    particle.setSprite(Pulsar::D2::Sprite(Util::Graphic::Color(44, 197, 246), 0.005, 0.03));
    particle.setPosition(getPosition() + Util::Math::Vector2<double>(random.getRandomNumber() * 0.5, 0));
    particle.setVelocity(Util::Math::Vector2<double>(0, -0.8));
    particle.setCollider(Pulsar::D2::RectangleCollider(particle.getPosition(), 0.005, 0.03, Pulsar::Collider::STATIC));

    particle.addComponent(new Pulsar::D2::LinearMovementComponent());
}

void RainEmitter::onParticleUpdate([[maybe_unused]] Pulsar::D2::Particle &particle, [[maybe_unused]] double delta) {}

void RainEmitter::onParticleCollision(Pulsar::D2::Particle &particle, [[maybe_unused]] const Pulsar::D2::CollisionEvent &event) {
    if (event.getCollidedWidth().getTag() == Ground::TAG || event.getCollidedWidth().getTag() == Dino::TAG) {
        auto *dropletEmitter = new DropletEmitter(particle.getPosition());
        getScene().addEntity(dropletEmitter);

        removeParticle(&particle);
    }
}

void RainEmitter::onParticleDestruction([[maybe_unused]] const Pulsar::D2::Particle &particle) {}
