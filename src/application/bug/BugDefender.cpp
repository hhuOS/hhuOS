/*
 * Copyright (C) 2017-2026 Heinrich Heine University Düsseldorf,
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

#include <stdint.h>

#include "BugDefender.h"

#include "Ship.h"
#include "EnemyBug.h"
#include "Fleet.h"

#include <util/base/String.h>
#include <util/collection/Array.h>
#include <util/io/key/KeyEvent.h>
#include <util/math/Random.h>
#include <pulsar/2d/Sprite.h>
#include <pulsar/Game.h>
#include <pulsar/Graphics.h>

void BugDefender::initialize() {
    backgroundMusic = Pulsar::AudioTrack("/user/bug/music.wav");

    addEntity(ship);

    for (size_t i = 0; i < BUGS_PER_COLUMN; i++) {
        for (size_t j = 0; j < BUGS_PER_ROW; j++) {
            const auto x = -1.0f + static_cast<float>(j) * (EnemyBug::WIDTH + 0.05f);
            const auto y = 0.8f - static_cast<float>(i) * (EnemyBug::HEIGHT + 0.05f);
            addEntity(new EnemyBug(Util::Math::Vector2<float>(x, y), enemyFleet));
        }
    }
}

void BugDefender::update(float) {
    if (!backgroundMusicHandle.isPlaying()) {
        backgroundMusicHandle = backgroundMusic.play(true);
    }

    enemyFleet.applyChanges();
}

bool BugDefender::initializeBackground(Pulsar::Graphics &graphics) {
    // Load background sky sprites
    auto backgroundSprites = Util::Array<Pulsar::D2::Sprite>(BACKGROUND_TILE_COUNT);
    for (size_t i = 0; i < BACKGROUND_TILE_COUNT; i++) {
        const auto path = Util::String::format("/user/bug/background%u.bmp", i + 1);
        backgroundSprites[i] = Pulsar::D2::Sprite(path, BACKGROUND_TILE_WIDTH, BACKGROUND_TILE_HEIGHT);
    }

    // Load background planet ground sprites
    auto planetSprites = Util::Array<Pulsar::D2::Sprite>(PLANET_TILE_COUNT);
    for (size_t i = 0; i < PLANET_TILE_COUNT; i++) {
        const auto path = Util::String::format("/user/bug/planet%u.bmp", i + 1);
        planetSprites[i] = Pulsar::D2::Sprite(path, PLANET_TILE_WIDTH, PLANET_TILE_HEIGHT);
    }

    // Load the surface sprite (drawn above planet ground)
    const Pulsar::D2::Sprite surfaceSprite("/user/bug/surface.bmp", PLANET_TILE_WIDTH, PLANET_TILE_HEIGHT);

    // Draw the background sky
    const auto dimensions = graphics.getDimensions();
    const auto backgroundTilePerRow = static_cast<int32_t>(dimensions.getX() > dimensions.getY() ?
        dimensions.getX() * BACKGROUND_TILES_PER_ROW : BACKGROUND_TILES_PER_ROW);
    const auto backgroundTilesPerColumn = static_cast<int32_t>(dimensions.getY() > dimensions.getX() ?
        dimensions.getY() * BACKGROUND_TILES_PER_COLUMN : BACKGROUND_TILES_PER_COLUMN);

    for (int32_t x = -backgroundTilePerRow; x < backgroundTilePerRow; x++) {
        for (int32_t y = -backgroundTilesPerColumn; y < backgroundTilesPerColumn; y++) {
            const auto index = static_cast<size_t>(random.getRandomNumber<float>() * BACKGROUND_TILE_COUNT);
            const auto pos = Util::Math::Vector2<float>(static_cast<float>(x) * BACKGROUND_TILE_WIDTH,
                static_cast<float>(y) * BACKGROUND_TILE_HEIGHT);

            backgroundSprites[index].draw(graphics, pos);
        }
    }

    // Draw the planet ground
    const auto planetTilesPerRow = static_cast<int32_t>(dimensions.getX() > dimensions.getY() ?
        dimensions.getX() * PLANET_TILES_PER_ROW : PLANET_TILES_PER_ROW);

    for (int32_t x = -planetTilesPerRow; x < planetTilesPerRow; x++) {
        const auto index = static_cast<size_t>(random.getRandomNumber<float>() * PLANET_TILE_COUNT);
        const auto pos = Util::Math::Vector2<float>(static_cast<float>(x) * PLANET_TILE_WIDTH, -1);

        planetSprites[index].draw(graphics, pos);
    }

    // Draw the planet surface
    for (int32_t x = -planetTilesPerRow; x < planetTilesPerRow; x++) {
        const auto pos = Util::Math::Vector2<float>(static_cast<float>(x) * PLANET_TILE_WIDTH,
            -1 + PLANET_TILE_HEIGHT);

        surfaceSprite.draw(graphics, pos);
    }

    return true;
}

void BugDefender::keyPressed(const Util::Io::KeyEvent &key) {
    switch (key.getScancode()) {
        case Util::Io::KeyEvent::ESC :
            Pulsar::Game::getInstance().stop();
            break;
        case Util::Io::KeyEvent::LEFT :
            ship->setVelocityX(-1.0);
            break;
        case Util::Io::KeyEvent::RIGHT :
            ship->setVelocityX(1.0);
            break;
        case Util::Io::KeyEvent::SPACE :
            ship->fireMissile();
        default:
            break;
    }
}

void BugDefender::keyReleased(const Util::Io::KeyEvent &key) {
    switch (key.getScancode()) {
        case Util::Io::KeyEvent::LEFT:
            if (ship->getVelocity().getX() < 0) {
                ship->setVelocityX(0);
            }
            break;
        case Util::Io::KeyEvent::RIGHT:
            if (ship->getVelocity().getX() > 0) {
                ship->setVelocityX(0);
            }
            break;
        default:
            break;
    }
}
