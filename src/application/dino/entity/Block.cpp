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
 *
 * The dino game is based on a bachelor's thesis, written by Malte Sehmer.
 * The original source code can be found here: https://github.com/Malte2036/hhuOS
 */

#include "Block.h"

#include "lib/util/game/GameManager.h"
#include "lib/util/game/Game.h"
#include "lib/util/game/Scene.h"
#include "lib/util/game/2d/event/CollisionEvent.h"
#include "PlayerDino.h"
#include "application/dino/particle/BloodEmitter.h"
#include "lib/util/base/Exception.h"
#include "lib/util/game/2d/collider/RectangleCollider.h"
#include "lib/util/math/Vector2D.h"

namespace Util {
namespace Game {
class Graphics;
namespace D2 {
class TranslationEvent;
}  // namespace D2
}  // namespace Game
}  // namespace Util

Block::Block(Tag tag, const Util::Math::Vector2D &position, uint32_t countX, uint32_t countY) :
        Util::Game::D2::Entity(tag, position, Util::Game::D2::RectangleCollider(position, Util::Math::Vector2D(SIZE * countX, SIZE * countY), Util::Game::D2::RectangleCollider::STATIC)),
        countX(countX), countY(countY) {}

void Block::initialize() {
    sprite = Util::Game::D2::Sprite(getSpritePath(static_cast<Block::Tag>(getTag())), SIZE, SIZE);
}

void Block::onUpdate([[maybe_unused]] double delta) {}

void Block::draw(Util::Game::Graphics &graphics) {
    for (uint32_t x = 0; x < countX; x++) {
        for (uint32_t y = 0; y < countY; y++) {
            sprite.draw(graphics, getPosition() + Util::Math::Vector2D(x * SIZE, y * SIZE));
        }
    }
}

void Block::onTranslationEvent([[maybe_unused]] Util::Game::D2::TranslationEvent &event) {}

void Block::onCollisionEvent(Util::Game::D2::CollisionEvent &event) {
    if (getTag() == WATER && event.getCollidedWidth().getTag() == PlayerDino::TAG && event.getSide() == Util::Game::D2::RectangleCollider::TOP) {
        auto &scene = Util::Game::GameManager::getGame().getCurrentScene();
        scene.addObject(new BloodEmitter(event.getCollidedWidth().getPosition(), BloodEmitter::WATER));
    }
}

Util::String Block::getSpritePath(Tag tag) {
    switch (tag) {
        case BOX:
            return "/user/dino/block/box.bmp";
        case GRASS:
            return "/user/dino/block/grass.bmp";
        case DIRT:
            return "/user/dino/block/dirt.bmp";
        case WATER:
            return "/user/dino/block/water.bmp";
        default:
            Util::Exception::throwException(Util::Exception::INVALID_ARGUMENT, "Invalid block tag!");
    }
}
