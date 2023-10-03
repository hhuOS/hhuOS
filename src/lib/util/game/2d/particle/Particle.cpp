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

#include "Particle.h"
#include "Emitter.h"
#include "lib/util/math/Math.h"
#include "lib/util/game/GameManager.h"
#include "lib/util/game/Game.h"

namespace Util::Game::D2 {

Particle::Particle(uint32_t tag, const Math::Vector2D &position, const Math::Vector2D &velocity, const Math::Vector2D &acceleration, double scale, double timeToLive, const Sprite &sprite, Emitter &parent) :
        Entity(tag, position), acceleration(acceleration), timeLimited(timeToLive > 0), timeToLive(timeToLive), scale(scale), sprite(sprite), parent(parent) {
    setVelocity(velocity);
}

Particle::Particle(uint32_t tag, const Math::Vector2D &position, const RectangleCollider &collider, const Math::Vector2D &velocity, const Math::Vector2D &acceleration, double scale, double timeToLive, const Sprite &sprite, Emitter &parent) :
        Entity(tag, position, collider), acceleration(acceleration), timeLimited(timeToLive > 0), timeToLive(timeToLive), scale(scale), sprite(sprite), parent(parent) {
    setVelocity(velocity);
}

void Particle::initialize() {}

void Particle::onUpdate(double delta) {
    if (timeLimited) {
        timeToLive -= delta;
        if (timeToLive <= 0) {
            parent.removeParticle(this);
        }
    }

    parent.updateParticle(*this, delta);

    rotation += rotationVelocity * delta;
    setVelocity(getVelocity() + acceleration * delta);
    setPosition(getPosition() + getVelocity() * delta);
}

void Particle::draw(Graphics &graphics) {
    graphics.drawImage2D(getPosition(), sprite.getImage(), false, Math::Vector2D(scale, scale), rotation);
}

void Particle::onTranslationEvent(TranslationEvent &event) {}

void Particle::onCollisionEvent(CollisionEvent &event) {
    parent.removeParticle(this);
}

bool Particle::isParticle() const {
    return true;
}

}