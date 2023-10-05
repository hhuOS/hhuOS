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
 * The game engine is based on a bachelor's thesis, written by Malte Sehmer.
 * The original source code can be found here: https://github.com/Malte2036/hhuOS
 *
 * It has been enhanced with 3D-capabilities during a bachelor's thesis by Richard Josef Schweitzer
 * The original source code can be found here: https://git.hhu.de/bsinfo/thesis/ba-risch114
 */

#include "GravityComponent.h"

#include "lib/util/math/Math.h"
#include "lib/util/game/2d/Entity.h"
#include "lib/util/game/2d/component/Component.h"
#include "lib/util/game/2d/event/TranslationEvent.h"
#include "lib/util/math/Vector2D.h"

namespace Util::Game::D2 {

GravityComponent::GravityComponent(Entity &entity, double gravityValue, double stopFactorX) :
        Component(entity), gravityValue(gravityValue), stopFactorX(stopFactorX) {}

void GravityComponent::update(double delta) {
    auto &entity = getEntity();
    auto velocity = entity.getVelocity() - Math::Vector2D(0, Math::absolute(gravityValue * delta));
    auto newPosition = entity.getPosition() + Math::Vector2D(velocity.getX() * delta, velocity.getY() * delta);

    auto event = TranslationEvent(newPosition);
    entity.onTranslationEvent(event);
    
    if (!event.isCanceled()) {
        velocity = Math::Vector2D(velocity.getX() * (1 - stopFactorX), velocity.getY());
        entity.setPosition(newPosition);
        entity.setVelocity(velocity);
    }
}

}