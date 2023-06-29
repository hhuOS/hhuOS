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

#include "BugDefender.h"
#include "lib/util/game/Sprite.h"
#include "lib/util/math/Random.h"
#include "lib/util/game/entity/component/LinearMovementComponent.h"
#include "lib/util/game/GameManager.h"
#include "lib/util/game/Game.h"
#include "Bug.h"

BugDefender::BugDefender() {
    addObject(ship);

    addObject(new Bug(Util::Math::Vector2D(-0.3, 0.8)));
    addObject(new Bug(Util::Math::Vector2D(0, 0.8)));
    addObject(new Bug(Util::Math::Vector2D(0.3, 0.8)));

    setKeyListener(*this);
}

void BugDefender::update(double delta) {

}

void BugDefender::initializeBackground(Util::Game::Graphics2D &graphics) {
    auto sprites = Util::Array<Util::Game::Sprite>(BACKGROUND_TILE_COUNT);
    for (uint32_t i = 0; i < BACKGROUND_TILE_COUNT; i++) {
        sprites[i] = Util::Game::Sprite(Util::String::format("/initrd/bug/background%u.bmp", i + 1), BACKGROUND_TILE_SIZE, BACKGROUND_TILE_SIZE);
    }

    auto random = Util::Math::Random();
    auto resolution = graphics.getAbsoluteResolution();
    auto defaultTilesPerRow = (1 / BACKGROUND_TILE_SIZE) + 1;
    auto tilesPerRow = static_cast<int32_t>(resolution.getX() > resolution.getY() ? (resolution.getX() / resolution.getY()) * defaultTilesPerRow : defaultTilesPerRow);
    auto tilesPerColumn = static_cast<int32_t>(resolution.getY() > resolution.getX() ? (resolution.getY() / resolution.getX()) * defaultTilesPerRow : defaultTilesPerRow);

    for (int32_t x = -tilesPerRow; x < tilesPerRow; x++) {
        for (int32_t y = -tilesPerColumn; y < tilesPerColumn; y++) {
            graphics.drawImage(Util::Math::Vector2D(x * BACKGROUND_TILE_SIZE, y * BACKGROUND_TILE_SIZE), sprites[static_cast<uint32_t>(random.nextRandomNumber() * BACKGROUND_TILE_COUNT)].getImage());
        }
    }
}

void BugDefender::keyPressed(Util::Io::Key key) {
    switch (key.getScancode()) {
        case Util::Io::Key::ESC :
            Util::Game::GameManager::getGame().stop();
            break;
        case Util::Io::Key::LEFT :
            ship->setVelocityX(-1.0);
            break;
        case Util::Io::Key::RIGHT :
            ship->setVelocityX(1.0);
            break;
        case Util::Io::Key::SPACE :
            ship->fireMissile();
        default:
            break;
    }
}

void BugDefender::keyReleased(Util::Io::Key key) {
    switch (key.getScancode()) {
        case Util::Io::Key::LEFT:
            if (ship->getVelocity().getX() < 0) {
                ship->setVelocityX(0);
            }
        case Util::Io::Key::RIGHT:
            if (ship->getVelocity().getX() > 0) {
                ship->setVelocityX(0);
            }
            break;
        default:
            break;
    }
}
