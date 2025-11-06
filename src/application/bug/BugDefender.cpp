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

#include "BugDefender.h"

#include "lib/pulsar/2d/Sprite.h"
#include "lib/util/math/Random.h"
#include "lib/pulsar/Game.h"
#include "EnemyBug.h"
#include "application/bug/Fleet.h"
#include "application/bug/Ship.h"
#include "lib/util/base/String.h"
#include "lib/util/collection/Array.h"
#include "lib/util/io/key/Key.h"
#include "lib/pulsar/Graphics.h"

void BugDefender::initialize() {
    backgroundMusic = Pulsar::AudioTrack("/user/bug/music.wav");

    addEntity(ship);

    for (uint32_t i = 0; i < BUGS_PER_COLUMN; i++) {
        for (uint32_t j = 0; j < BUGS_PER_ROW; j++) {
            addEntity(new EnemyBug(Util::Math::Vector2<double>(-1.0 + j * (EnemyBug::SIZE_X + 0.05), 0.8 - i * (EnemyBug::SIZE_Y + 0.05)), enemyFleet));
        }
    }
}

void BugDefender::update([[maybe_unused]] double delta) {
    if (!backgroundMusicHandle.isPlaying()) {
        backgroundMusicHandle = backgroundMusic.play(true);
    }

    enemyFleet.applyChanges();
}

bool BugDefender::initializeBackground(Pulsar::Graphics &graphics) {
    auto backgroundSprites = Util::Array<Pulsar::D2::Sprite>(BACKGROUND_TILE_COUNT);
    for (uint32_t i = 0; i < BACKGROUND_TILE_COUNT; i++) {
        backgroundSprites[i] = Pulsar::D2::Sprite(Util::String::format("/user/bug/background%u.bmp", i + 1), BACKGROUND_TILE_WIDTH, BACKGROUND_TILE_HEIGHT);
    }

    auto planetSprites = Util::Array<Pulsar::D2::Sprite>(PLANET_TILE_COUNT);
    for (uint32_t i = 0; i < PLANET_TILE_COUNT; i++) {
        planetSprites[i] = Pulsar::D2::Sprite(Util::String::format("/user/bug/planet%u.bmp", i + 1), PLANET_TILE_WIDTH, PLANET_TILE_HEIGHT);
    }

    auto surfaceSprite = Pulsar::D2::Sprite(Util::String::format("/user/bug/surface.bmp"), PLANET_TILE_WIDTH, PLANET_TILE_HEIGHT);

    auto dimensions = graphics.getDimensions();
    auto defaultTilesPerRow = (1 / BACKGROUND_TILE_WIDTH) + 1;
    auto defaultTilePerColumn = (1 / BACKGROUND_TILE_HEIGHT) + 1;
    auto tilesPerRow = static_cast<int32_t>(dimensions.getX() > dimensions.getY() ? dimensions.getX() * defaultTilesPerRow : defaultTilesPerRow);
    auto tilesPerColumn = static_cast<int32_t>(dimensions.getY() > dimensions.getX() ? dimensions.getY() * defaultTilePerColumn : defaultTilePerColumn);

    for (int32_t x = -tilesPerRow; x < tilesPerRow; x++) {
        for (int32_t y = -tilesPerColumn; y < tilesPerColumn; y++) {
            backgroundSprites[static_cast<uint32_t>(random.getRandomNumber() * BACKGROUND_TILE_COUNT)].draw(graphics, Util::Math::Vector2<double>(x * BACKGROUND_TILE_WIDTH, y * BACKGROUND_TILE_HEIGHT));
        }
    }

    defaultTilesPerRow = (1 / PLANET_TILE_WIDTH) + 1;
    tilesPerRow = static_cast<int32_t>(dimensions.getX() > dimensions.getY() ? dimensions.getX() * defaultTilesPerRow : defaultTilesPerRow);

    for (int32_t x = -tilesPerRow; x < tilesPerRow; x++) {
        surfaceSprite.draw(graphics, Util::Math::Vector2<double>(x * PLANET_TILE_WIDTH, -1 + PLANET_TILE_HEIGHT));
    }

    for (int32_t x = -tilesPerRow; x < tilesPerRow; x++) {
        planetSprites[static_cast<uint32_t>(random.getRandomNumber() * PLANET_TILE_COUNT)].draw(graphics, Util::Math::Vector2<double>(x * PLANET_TILE_WIDTH, -1));
    }

    return true;
}

void BugDefender::keyPressed(const Util::Io::Key &key) {
    switch (key.getScancode()) {
        case Util::Io::Key::ESC :
            Pulsar::Game::getInstance().stop();
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
