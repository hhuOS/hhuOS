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

#include "LinearMovementComponent.h"

#include "lib/util/game/entity/Entity.h"
#include "lib/util/game/entity/component/Component.h"
#include "lib/util/game/entity/event/TranslationEvent.h"
#include "lib/util/math/Vector2D.h"

namespace Util::Game {

LinearMovementComponent::LinearMovementComponent(Entity &entity) : Component(entity) {}

void LinearMovementComponent::update(double delta) {
    auto newPosition = getEntity().getPosition() + getEntity().getVelocity() * delta;
    auto event = TranslationEvent(newPosition);
    getEntity().onTranslationEvent(event);

    if (!event.isCanceled()) {
        getEntity().setPosition(newPosition);
    }
}

}