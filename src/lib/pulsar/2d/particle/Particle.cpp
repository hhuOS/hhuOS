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
 * The particle system is based on a bachelor's thesis, written by Abdulbasir Gümüs.
 * The original source code can be found here: https://git.hhu.de/bsinfo/thesis/ba-abgue101
 */

#include "Particle.h"

#include "Emitter.h"
#include "lib/util/math/Vector2.h"

namespace Pulsar::D2 {

Particle::Particle(uint32_t tag, Emitter &parent) : Entity(tag, parent.getPosition()), parent(parent) {}

void Particle::initialize() {
    parent.onParticleInitialization(*this);
}

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
}

void Particle::draw(Graphics &graphics) const {
    sprite.draw(graphics, getPosition());
}

void Particle::onTranslationEvent([[maybe_unused]] TranslationEvent &event) {}

void Particle::onCollisionEvent(CollisionEvent &event) {
    parent.onParticleCollision(*this, event);
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

void Particle::setTimeToLive(double timeToLive) {
    Particle::timeToLive = timeToLive;
    timeLimited = timeToLive > 0;
}

void Particle::setSprite(const Sprite &sprite) {
    Particle::sprite = sprite;
}

double Particle::getRotationVelocity() const {
    return rotationVelocity;
}

void Particle::setRotationVelocity(double rotationVelocity) {
    Particle::rotationVelocity = rotationVelocity;
}

}