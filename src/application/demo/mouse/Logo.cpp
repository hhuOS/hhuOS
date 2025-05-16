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
 */

#include "Logo.h"

#include "lib/util/game/2d/event/TranslationEvent.h"
#include "lib/util/game/GameManager.h"
#include "lib/util/math/Vector2.h"
#include "lib/util/base/String.h"

Logo::Logo() : Util::Game::D2::Entity(0, Util::Math::Vector2<double>(-0.5, -0.313)) {}

void Logo::initialize() {
    sprite = Util::Game::D2::Sprite("user/mouse/logo.bmp", 1, 0.626);
}

void Logo::onUpdate([[maybe_unused]] double delta) {}

void Logo::onTranslationEvent(Util::Game::D2::TranslationEvent &event) {
    const auto &dimensions = Util::Game::GameManager::getDimensions();
    const auto &newPosition = event.getTargetPosition();

    if (newPosition.getX() < -dimensions.getX() ||
        newPosition.getX() > dimensions.getX() - sprite.getSize().getX() ||
        newPosition.getY() < -dimensions.getY() ||
        newPosition.getY() > dimensions.getY() - sprite.getSize().getY()) {
        event.cancel();
    }
}

void Logo::onCollisionEvent([[maybe_unused]] Util::Game::D2::CollisionEvent &event) {}

void Logo::draw(Util::Game::Graphics &graphics) {
    sprite.draw(graphics, getPosition());
}
