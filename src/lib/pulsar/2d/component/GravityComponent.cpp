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

#include "GravityComponent.h"

#include "lib/util/math/Math.h"
#include "lib/pulsar/2d/Entity.h"
#include "lib/pulsar/2d/component/Component.h"
#include "lib/pulsar/2d/event/TranslationEvent.h"
#include "lib/util/math/Vector2.h"

namespace Pulsar::D2 {

GravityComponent::GravityComponent(const double gravityValue, const double stopFactorX) :
    gravityValue(gravityValue), stopFactorX(stopFactorX) {}

void GravityComponent::update(const double delta) {
    auto &entity = getEntity();
    const auto gravityEffect = Util::Math::Vector2<double>(0, -Util::Math::absolute(gravityValue * delta));
    const auto velocity = entity.getVelocity() + gravityEffect;
    const auto newPosition = entity.getPosition() + velocity * delta;

    auto event = TranslationEvent(newPosition);
    entity.onTranslationEvent(event);
    
    if (!event.isCanceled()) {
        entity.setPosition(newPosition);
        entity.setVelocity(Util::Math::Vector2<double>(velocity.getX() * (1 - stopFactorX), velocity.getY()));
    }
}

}