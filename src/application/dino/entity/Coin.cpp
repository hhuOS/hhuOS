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
 * The dino game is based on a bachelor's thesis, written by Malte Sehmer.
 * The original source code can be found here: https://github.com/Malte2036/hhuOS
 */

#include "Coin.h"

#include "Block.h"
#include "PlayerDino.h"
#include "lib/util/pulsar/2d/event/CollisionEvent.h"
#include "lib/util/pulsar/Game.h"
#include "lib/util/pulsar/Scene.h"
#include "lib/util/base/String.h"
#include "lib/util/collection/Array.h"
#include "lib/util/pulsar/2d/Sprite.h"
#include "lib/util/pulsar/2d/collider/RectangleCollider.h"
#include "lib/util/math/Vector2.h"

Coin::Coin(const Util::Math::Vector2<double> &position) : Util::Pulsar::D2::Entity(TAG, position, Util::Pulsar::D2::RectangleCollider(position, Util::Math::Vector2<double>(Block::SIZE, Block::SIZE), Util::Pulsar::D2::RectangleCollider::PERMEABLE)) {}

void Coin::initialize() {
    animation = Util::Pulsar::D2::SpriteAnimation(Util::Array<Util::Pulsar::D2::Sprite>({
        Util::Pulsar::D2::Sprite("/user/dino/item/coin1.bmp", Block::SIZE, Block::SIZE),
        Util::Pulsar::D2::Sprite("/user/dino/item/coin2.bmp", Block::SIZE, Block::SIZE),
        Util::Pulsar::D2::Sprite("/user/dino/item/coin3.bmp", Block::SIZE, Block::SIZE),
        Util::Pulsar::D2::Sprite("/user/dino/item/coin4.bmp", Block::SIZE, Block::SIZE),
        Util::Pulsar::D2::Sprite("/user/dino/item/coin5.bmp", Block::SIZE, Block::SIZE),
        Util::Pulsar::D2::Sprite("/user/dino/item/coin6.bmp", Block::SIZE, Block::SIZE),
        Util::Pulsar::D2::Sprite("/user/dino/item/coin7.bmp", Block::SIZE, Block::SIZE),
        Util::Pulsar::D2::Sprite("/user/dino/item/coin8.bmp", Block::SIZE, Block::SIZE),
        Util::Pulsar::D2::Sprite("/user/dino/item/coin9.bmp", Block::SIZE, Block::SIZE),
        Util::Pulsar::D2::Sprite("/user/dino/item/coin10.bmp", Block::SIZE, Block::SIZE),
        Util::Pulsar::D2::Sprite("/user/dino/item/coin11.bmp", Block::SIZE, Block::SIZE),
        Util::Pulsar::D2::Sprite("/user/dino/item/coin12.bmp", Block::SIZE, Block::SIZE)}), 1);
}

void Coin::onUpdate(double delta) {
    animation.update(delta);
}

void Coin::draw(Util::Pulsar::Graphics &graphics) const {
    animation.draw(graphics, getPosition());
}

void Coin::onTranslationEvent([[maybe_unused]] Util::Pulsar::D2::TranslationEvent &event) {}

void Coin::onCollisionEvent(Util::Pulsar::D2::CollisionEvent &event) {
    if (event.getCollidedWidth().getTag() == PlayerDino::TAG) {
        auto &dino = reinterpret_cast<PlayerDino&>(event.getCollidedWidth());
        dino.setPoints(dino.getPoints() + 1);

        removeFromScene();
    }
}
