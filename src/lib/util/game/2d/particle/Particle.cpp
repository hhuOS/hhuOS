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

Particle::Particle(uint32_t tag, const Math::Vector2D &position, const Math::Vector2D &velocity, const Math::Vector2D &acceleration,
                   double rotationVelocity, double scale, double alpha, double timeToLive, const Sprite &sprite, Emitter &parent) :
        Entity(tag, position), acceleration(acceleration), rotationVelocity(rotationVelocity), timeLimited(timeToLive > 0), timeToLive(timeToLive), sprite(sprite), parent(parent) {
    setVelocity(velocity);
    Particle::sprite.setAlpha(alpha);
    Particle::sprite.setScale(scale);
}

Particle::Particle(uint32_t tag, const Math::Vector2D &position, const RectangleCollider &collider, const Math::Vector2D &velocity,
                   const Math::Vector2D &acceleration, double rotationVelocity, double scale, double alpha, double timeToLive, const Sprite &sprite, Emitter &parent) :
        Entity(tag, position, collider), acceleration(acceleration), rotationVelocity(rotationVelocity), timeLimited(timeToLive > 0), timeToLive(timeToLive), sprite(sprite), parent(parent) {
    setVelocity(velocity);
    Particle::sprite.setScale(scale);
}

void Particle::initialize() {}

void Particle::onUpdate(double delta) {
    if (timeLimited) {
        timeToLive -= delta;
        if (timeToLive <= 0) {
            parent.removeParticle(this);
            return;
        }
    }

    parent.onParticleUpdate(*this, delta);

    sprite.rotate(rotationVelocity * delta);
    setVelocity(getVelocity() + acceleration * delta);
    setPosition(getPosition() + getVelocity() * delta);
}

void Particle::draw(Graphics &graphics) {
    sprite.draw(graphics, getPosition());
}

void Particle::onTranslationEvent(TranslationEvent &event) {}

void Particle::onCollisionEvent(CollisionEvent &event) {
    parent.removeParticle(this);
}

bool Particle::isParticle() const {
    return true;
}

double Particle::getScale() const {
    return sprite.getScale().getX();
}

void Particle::setScale(double scale) {
    sprite.setScale(scale);
}

double Particle::getAlpha() const {
    return sprite.getAlpha();
}

void Particle::setAlpha(double alpha) {
    sprite.setAlpha(alpha);
}

}