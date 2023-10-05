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
 *
 * The particle demo is based on a bachelor's thesis, written by Abdulbasir Gümüs.
 * The original source code can be found here: https://git.hhu.de/bsinfo/thesis/ba-abgue101
 */

#include "DropletEmitter.h"
#include "lib/util/math/Math.h"
#include "lib/util/game/GameManager.h"
#include "lib/util/game/Scene.h"
#include "lib/util/game/2d/component/GravityComponent.h"

DropletEmitter::DropletEmitter(const Util::Math::Vector2D &position) : Util::Game::D2::SingleTimeEmitter(TAG, PARTICLE_TAG, position) {}

void DropletEmitter::initialize() {
    SingleTimeEmitter::initialize();

    setMinEmissionRate(5);
    setMaxEmissionRate(5);
}

void DropletEmitter::draw(Util::Game::Graphics &graphics) {}

void DropletEmitter::onTranslationEvent(Util::Game::D2::TranslationEvent &event) {}

void DropletEmitter::onCollisionEvent(Util::Game::D2::CollisionEvent &event) {}

void DropletEmitter::onParticleInitialization(Util::Game::D2::Particle &particle) {
    auto angle = random.nextRandomNumber() * Util::Math::PI;

    particle.setSprite(Util::Game::D2::Sprite("/initrd/demo/raindrop.bmp", 0.005, 0.005));
    particle.setPosition(getPosition());
    particle.setVelocity(Util::Math::Vector2D(Util::Math::cosine(angle), Util::Math::sine(angle)));
    particle.setTimeToLive(-1);
    particle.setCollider(Util::Game::D2::RectangleCollider(particle.getPosition(), Util::Math::Vector2D(0.005, 0.005), Util::Game::Collider::STATIC));

    particle.addComponent(new Util::Game::D2::GravityComponent(particle, 2.5, 0.0025));
}

void DropletEmitter::onParticleUpdate(Util::Game::D2::Particle &particle, double delta) {
    particle.setAlpha(particle.getAlpha() - 1 * delta);
}

void DropletEmitter::onParticleDestruction(Util::Game::D2::Particle &particle) {}
