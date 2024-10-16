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

#include "Fruit.h"

#include "PlayerDino.h"
#include "lib/util/game/2d/event/CollisionEvent.h"
#include "lib/util/game/GameManager.h"
#include "lib/util/game/Game.h"
#include "application/dino/Level.h"
#include "lib/util/base/String.h"
#include "lib/util/collection/Array.h"
#include "lib/util/game/2d/Sprite.h"
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

Fruit::Fruit(const Util::Math::Vector2D &position, const Util::Io::File &nextLevelFile) :
        Util::Game::D2::Entity(TAG, position, Util::Game::D2::RectangleCollider(position, Util::Math::Vector2D(SIZE, SIZE), Util::Game::D2::RectangleCollider::STATIC)),
        nextLevelFile(nextLevelFile) {}

void Fruit::initialize() {
    animation = Util::Game::D2::SpriteAnimation(Util::Array<Util::Game::D2::Sprite>({
        Util::Game::D2::Sprite("/user/dino/item/apple1.bmp", SIZE, SIZE),
        Util::Game::D2::Sprite("/user/dino/item/apple2.bmp", SIZE, SIZE),
        Util::Game::D2::Sprite("/user/dino/item/apple3.bmp", SIZE, SIZE),
        Util::Game::D2::Sprite("/user/dino/item/apple4.bmp", SIZE, SIZE),
        Util::Game::D2::Sprite("/user/dino/item/apple5.bmp", SIZE, SIZE),
        Util::Game::D2::Sprite("/user/dino/item/apple6.bmp", SIZE, SIZE),
        Util::Game::D2::Sprite("/user/dino/item/apple7.bmp", SIZE, SIZE),
        Util::Game::D2::Sprite("/user/dino/item/apple8.bmp", SIZE, SIZE),
        Util::Game::D2::Sprite("/user/dino/item/apple9.bmp", SIZE, SIZE),
        Util::Game::D2::Sprite("/user/dino/item/apple10.bmp", SIZE, SIZE),
        Util::Game::D2::Sprite("/user/dino/item/apple11.bmp", SIZE, SIZE),
        Util::Game::D2::Sprite("/user/dino/item/apple12.bmp", SIZE, SIZE),
        Util::Game::D2::Sprite("/user/dino/item/apple13.bmp", SIZE, SIZE),
        Util::Game::D2::Sprite("/user/dino/item/apple14.bmp", SIZE, SIZE),
        Util::Game::D2::Sprite("/user/dino/item/apple15.bmp", SIZE, SIZE),
        Util::Game::D2::Sprite("/user/dino/item/apple16.bmp", SIZE, SIZE),
        Util::Game::D2::Sprite("/user/dino/item/apple17.bmp", SIZE, SIZE)}), 0.85);
}

void Fruit::onUpdate(double delta) {
    animation.update(delta);
}

void Fruit::draw(Util::Game::Graphics &graphics) {
    animation.draw(graphics, getPosition());
}

void Fruit::onTranslationEvent([[maybe_unused]] Util::Game::D2::TranslationEvent &event) {}

void Fruit::onCollisionEvent(Util::Game::D2::CollisionEvent &event) {
    if (event.getCollidedWidth().getTag() == PlayerDino::TAG) {
        auto &dino = reinterpret_cast<PlayerDino&>(event.getCollidedWidth());
        auto &game = Util::Game::GameManager::getGame();
        game.pushScene(new Level(nextLevelFile, dino.getPoints() + 10));
        game.switchToNextScene();
    }
}
