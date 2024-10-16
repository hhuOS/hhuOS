/*
 * Copyright (C) 2018-2024 Heinrich-Heine-Universitaet Duesseldorf,
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

#include "lib/util/game/2d/Sprite.h"
#include "lib/util/math/Random.h"
#include "lib/util/game/GameManager.h"
#include "lib/util/game/Game.h"
#include "EnemyBug.h"
#include "application/bug/Fleet.h"
#include "application/bug/Ship.h"
#include "lib/util/base/String.h"
#include "lib/util/collection/Array.h"
#include "lib/util/io/key/Key.h"

void BugDefender::initialize() {
    addObject(ship);

    for (uint32_t i = 0; i < BUGS_PER_COLUMN; i++) {
        for (uint32_t j = 0; j < BUGS_PER_ROW; j++) {
            addObject(new EnemyBug(Util::Math::Vector2D(-1.0 + j * (EnemyBug::SIZE_X + 0.05), 0.8 - i * (EnemyBug::SIZE_Y + 0.05)), enemyFleet));
        }
    }

    setKeyListener(*this);
}

void BugDefender::update([[maybe_unused]] double delta) {
    enemyFleet.applyChanges();
}

void BugDefender::initializeBackground(Util::Game::Graphics &graphics) {
    auto backgroundSprites = Util::Array<Util::Game::D2::Sprite>(BACKGROUND_TILE_COUNT);
    for (uint32_t i = 0; i < BACKGROUND_TILE_COUNT; i++) {
        backgroundSprites[i] = Util::Game::D2::Sprite(Util::String::format("/user/bug/background%u.bmp", i + 1), BACKGROUND_TILE_WIDTH, BACKGROUND_TILE_HEIGHT);
    }

    auto planetSprites = Util::Array<Util::Game::D2::Sprite>(PLANET_TILE_COUNT);
    for (uint32_t i = 0; i < PLANET_TILE_COUNT; i++) {
        planetSprites[i] = Util::Game::D2::Sprite(Util::String::format("/user/bug/planet%u.bmp", i + 1), PLANET_TILE_WIDTH, PLANET_TILE_HEIGHT);
    }

    auto surfaceSprite = Util::Game::D2::Sprite(Util::String::format("/user/bug/surface.bmp"), PLANET_TILE_WIDTH, PLANET_TILE_HEIGHT);

    auto resolution = Util::Game::GameManager::getRelativeResolution();
    auto defaultTilesPerRow = (1 / BACKGROUND_TILE_WIDTH) + 1;
    auto defaultTilePerColumn = (1 / BACKGROUND_TILE_HEIGHT) + 1;
    auto tilesPerRow = static_cast<int32_t>(resolution.getX() > resolution.getY() ? resolution.getX() * defaultTilesPerRow : defaultTilesPerRow);
    auto tilesPerColumn = static_cast<int32_t>(resolution.getY() > resolution.getX() ? resolution.getY() * defaultTilePerColumn : defaultTilePerColumn);

    for (int32_t x = -tilesPerRow; x < tilesPerRow; x++) {
        for (int32_t y = -tilesPerColumn; y < tilesPerColumn; y++) {
            backgroundSprites[static_cast<uint32_t>(random.nextRandomNumber() * BACKGROUND_TILE_COUNT)].draw(graphics, Util::Math::Vector2D(x * BACKGROUND_TILE_WIDTH, y * BACKGROUND_TILE_HEIGHT));
        }
    }

    defaultTilesPerRow = (1 / PLANET_TILE_WIDTH) + 1;
    tilesPerRow = static_cast<int32_t>(resolution.getX() > resolution.getY() ? resolution.getX() * defaultTilesPerRow : defaultTilesPerRow);

    for (int32_t x = -tilesPerRow; x < tilesPerRow; x++) {
        surfaceSprite.draw(graphics, Util::Math::Vector2D(x * PLANET_TILE_WIDTH, -1 + PLANET_TILE_HEIGHT));
    }

    for (int32_t x = -tilesPerRow; x < tilesPerRow; x++) {
        planetSprites[static_cast<uint32_t>(random.nextRandomNumber() * PLANET_TILE_COUNT)].draw(graphics, Util::Math::Vector2D(x * PLANET_TILE_WIDTH, -1));
    }
}

void BugDefender::keyPressed(const Util::Io::Key &key) {
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

void BugDefender::keyReleased(const Util::Io::Key &key) {
    switch (key.getScancode()) {
        case Util::Io::Key::LEFT:
            if (ship->getVelocity().getX() < 0) {
                ship->setVelocityX(0);
            }
            break;
        case Util::Io::Key::RIGHT:
            if (ship->getVelocity().getX() > 0) {
                ship->setVelocityX(0);
            }
            break;
        default:
            break;
    }
}
