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
 * The game engine is based on a bachelor's thesis, written by Malte Sehmer.
 * The original source code can be found here: https://github.com/Malte2036/hhuOS
 *
 * It has been enhanced with 3D-capabilities during a bachelor's thesis by Richard Josef Schweitzer
 * The original source code can be found here: https://git.hhu.de/bsinfo/thesis/ba-risch114
 *
 * The 3D-rendering has been rewritten using OpenGL (TinyGL) during a bachelor's thesis by Kevin Weber
 * The original source code can be found here: https://git.hhu.de/bsinfo/thesis/ba-keweb100
 *
 * The 2D particle system is based on a bachelor's thesis, written by Abdulbasir Gümüs.
 * The original source code can be found here: https://git.hhu.de/bsinfo/thesis/ba-abgue101
 */

#include "Particle.h"


#include "util/math/Vector2.h"
#include "pulsar/2d/particle/Emitter.h"

namespace Pulsar::D2 {

Particle::Particle(const size_t tag, Emitter &parent) : Entity(tag, parent.getPosition()), parent(parent) {}

void Particle::initialize() {
    parent.onParticleInitialization(*this);
}

void Particle::onUpdate(const float delta) {
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

void Particle::onCollisionEvent(const CollisionEvent &event) {
    parent.onParticleCollision(*this, event);
}

float Particle::getScale() const {
    return sprite.getScale().getX();
}

void Particle::setScale(const float scale) {
    sprite.setScale(scale);
}

float Particle::getAlpha() const {
    return sprite.getAlpha();
}

void Particle::setAlpha(const float alpha) {
    sprite.setAlpha(alpha);
}

float Particle::getRotationVelocity() const {
    return rotationVelocity;
}

void Particle::setRotationVelocity(const float rotationVelocity) {
    Particle::rotationVelocity = rotationVelocity;
}

void Particle::setTimeToLive(const Util::Time::Timestamp &timeToLive) {
    Particle::timeToLive = timeToLive.toSecondsFloat<float>();
    timeLimited = Particle::timeToLive > 0;
}

void Particle::setSprite(const Sprite &sprite) {
    Particle::sprite = sprite;
}

}