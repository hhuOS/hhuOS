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

#include "SpriteDemo.h"

#include "lib/util/game/Game.h"
#include "lib/util/game/GameManager.h"
#include "application/demo/sprites/DemoSprite.h"
#include "application/demo/sprites/DemoSpriteFactory.h"
#include "lib/util/io/key/Key.h"

namespace Util {
namespace Game {
class Graphics;
}  // namespace Game
}  // namespace Util

SpriteDemo::SpriteDemo(uint32_t initialCount) : initialCount(initialCount) {
}

void SpriteDemo::initialize() {
    for (uint32_t i = 0; i < initialCount; i++) {
        auto *sprite = factory.createSprite();
        entities.offer(sprite);
        addObject(sprite);
    }

    setKeyListener(*this);
}

void SpriteDemo::update([[maybe_unused]] double delta) {}

void SpriteDemo::initializeBackground([[maybe_unused]] Util::Game::Graphics &graphics) {}

void SpriteDemo::keyPressed(const Util::Io::Key &key) {
    switch (key.getScancode()) {
        case Util::Io::Key::PLUS: {
            auto *sprite = factory.createSprite();
            entities.offer(sprite);
            addObject(sprite);
            break;
        }
        case Util::Io::Key::MINUS: {
            if (entities.size() > 0) {
                removeObject(entities.poll());
            }
            break;
        }
        case Util::Io::Key::ESC:
            Util::Game::GameManager::getGame().stop();
            break;
        default:
            break;
    }
}

void SpriteDemo::keyReleased([[maybe_unused]] const Util::Io::Key &key) {}
