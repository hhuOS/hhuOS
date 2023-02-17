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

#include "GravityComponent.h"
#include "lib/util/math/Math.h"

namespace Util::Game {

GravityComponent::GravityComponent(Entity &entity, double groundY, double mass, double stopFactorX, double gravityValue) :
        Component(entity), groundY(groundY), mass(mass), stopFactorX(stopFactorX), gravityValue(gravityValue) {}

void GravityComponent::update(double delta) {
    auto &entity = getEntity();
    const auto &position = entity.getPosition();
    auto velocity = entity.getVelocity();

    if (position.getY() > groundY) {
        auto force = Math::Vector2D(0, mass * gravityValue);
        auto acceleration = Math::Vector2D(force.getX() / mass, force.getY() / mass);
        velocity = velocity + Math::Vector2D(acceleration.getX() * delta, acceleration.getY() * delta);
    } else {
        entity.setPosition(Math::Vector2D(entity.getPosition().getX(), groundY));
        entity.setVelocity(Math::Vector2D(entity.getVelocity().getX(), 0));
        return;
    }

    auto newPosition = position + Math::Vector2D(velocity.getX() * delta, velocity.getY() * delta);
    auto event = TranslationEvent(newPosition);
    entity.onTranslationEvent(event);
    
    if (!event.isCanceled()) {
        velocity = Math::Vector2D(velocity.getX() * (1 - stopFactorX), velocity .getY());
        entity.setPosition(newPosition);
        entity.setVelocity(velocity);
    }
}

}