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
 * The network stack is based on a bachelor's thesis, written by Malte Sehmer.
 * The original source code can be found here: https://git.hhu.de/bsinfo/thesis/ba-maseh100
 */

#include "GravityComponent.h"

#include "lib/util/math/Math.h"
#include "lib/util/game/entity/Entity.h"
#include "lib/util/game/entity/component/Component.h"
#include "lib/util/game/entity/event/TranslationEvent.h"
#include "lib/util/math/Vector2D.h"

namespace Util::Game {

GravityComponent::GravityComponent(Entity &entity, double mass, double stopFactorX, double gravityValue) :
        Component(entity), mass(mass), stopFactorX(stopFactorX), gravityValue(gravityValue) {}

void GravityComponent::update(double delta) {
    auto &entity = getEntity();
    const auto &position = entity.getPosition();
    auto velocity = entity.getVelocity();

    auto force = Math::Vector2D(0, mass * gravityValue);
    auto acceleration = Math::Vector2D(0, force.getY() / mass);
    velocity = velocity + Math::Vector2D(acceleration.getX() * delta, -Math::absolute(acceleration.getY() * delta));

    auto newPosition = position + Math::Vector2D(velocity.getX() * delta, velocity.getY() * delta);
    auto event = TranslationEvent(newPosition);
    entity.onTranslationEvent(event);
    
    if (!event.isCanceled()) {
        velocity = Math::Vector2D(velocity.getX() * (1 - stopFactorX), velocity.getY());
        entity.setPosition(newPosition);
        entity.setVelocity(velocity);
    }
}

}