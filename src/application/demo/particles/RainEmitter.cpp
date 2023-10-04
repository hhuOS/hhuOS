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

#include "RainEmitter.h"
#include "lib/util/game/2d/event/TranslationEvent.h"
#include "lib/util/game/2d/component/LinearMovementComponent.h"
#include "DropletEmitter.h"
#include "lib/util/game/GameManager.h"
#include "lib/util/game/Scene.h"

RainEmitter::RainEmitter(const Util::Math::Vector2D &position) : Util::Game::D2::Emitter(TAG, position, 0) {
    addComponent(new Util::Game::D2::LinearMovementComponent(*this));
    setVelocityX(SPEED);
}

void RainEmitter::initialize() {
    Emitter::initialize();
    cloudSprite = Util::Game::Sprite("/initrd/dino/cloud3.bmp", 0.6, 0.15);

    setEmissionTime(0.2);
    setMinEmissionRate(2);
    setMaxEmissionRate(2);

    particleTag = PARTICLE_TAG;
    particleSprite = Util::Game::Sprite("/initrd/demo/raindrop.bmp", 0.005, 0.03);
    particleVelocity = Util::Math::Vector2D(0, -0.8);
    particleTimeToLive = 5;
    particleColliderSize = Util::Math::Vector2D(0.005, 0.03);
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
    } else if (event.getTargetPosition().getX() > 0.6){
        setPositionX(0.6);
        setVelocityX(-SPEED);
    }
}

void RainEmitter::onCollisionEvent(Util::Game::D2::CollisionEvent &event) {}

void RainEmitter::setNextParticleAttributes() {
    particlePosition = getPosition() + Util::Math::Vector2D(random.nextRandomNumber() * 0.5, 0);
}

void RainEmitter::onParticleUpdate(Util::Game::D2::Particle &particle, double delta) {}

void RainEmitter::onParticleDestruction(Util::Game::D2::Particle &particle) {
    auto *dropletEmitter = new DropletEmitter(particle.getPosition());
    Util::Game::GameManager::getCurrentScene().addObject(dropletEmitter);
}
