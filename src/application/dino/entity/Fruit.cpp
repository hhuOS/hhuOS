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

#include "Fruit.h"

#include "PlayerDino.h"
#include "lib/pulsar/2d/event/CollisionEvent.h"
#include "lib/pulsar/Game.h"
#include "application/dino/Level.h"
#include "lib/util/base/String.h"
#include "lib/util/collection/Array.h"
#include "lib/pulsar/2d/Sprite.h"
#include "lib/pulsar/2d/collider/RectangleCollider.h"
#include "lib/util/math/Vector2.h"

Fruit::Fruit(const Util::Math::Vector2<float> &position, const Util::Io::File &nextLevelFile) :
        Pulsar::D2::Entity(TAG, position, Pulsar::D2::RectangleCollider(position, SIZE, SIZE, Pulsar::D2::RectangleCollider::STATIC)),
        nextLevelFile(nextLevelFile) {}

void Fruit::initialize() {
    animation = Pulsar::D2::SpriteAnimation(Util::Array<Pulsar::D2::Sprite>({
        Pulsar::D2::Sprite("/user/dino/item/apple1.bmp", SIZE, SIZE),
        Pulsar::D2::Sprite("/user/dino/item/apple2.bmp", SIZE, SIZE),
        Pulsar::D2::Sprite("/user/dino/item/apple3.bmp", SIZE, SIZE),
        Pulsar::D2::Sprite("/user/dino/item/apple4.bmp", SIZE, SIZE),
        Pulsar::D2::Sprite("/user/dino/item/apple5.bmp", SIZE, SIZE),
        Pulsar::D2::Sprite("/user/dino/item/apple6.bmp", SIZE, SIZE),
        Pulsar::D2::Sprite("/user/dino/item/apple7.bmp", SIZE, SIZE),
        Pulsar::D2::Sprite("/user/dino/item/apple8.bmp", SIZE, SIZE),
        Pulsar::D2::Sprite("/user/dino/item/apple9.bmp", SIZE, SIZE),
        Pulsar::D2::Sprite("/user/dino/item/apple10.bmp", SIZE, SIZE),
        Pulsar::D2::Sprite("/user/dino/item/apple11.bmp", SIZE, SIZE),
        Pulsar::D2::Sprite("/user/dino/item/apple12.bmp", SIZE, SIZE),
        Pulsar::D2::Sprite("/user/dino/item/apple13.bmp", SIZE, SIZE),
        Pulsar::D2::Sprite("/user/dino/item/apple14.bmp", SIZE, SIZE),
        Pulsar::D2::Sprite("/user/dino/item/apple15.bmp", SIZE, SIZE),
        Pulsar::D2::Sprite("/user/dino/item/apple16.bmp", SIZE, SIZE),
        Pulsar::D2::Sprite("/user/dino/item/apple17.bmp", SIZE, SIZE)}), 0.85);
}

void Fruit::onUpdate(float delta) {
    animation.update(delta);
}

void Fruit::draw(Pulsar::Graphics &graphics) const {
    animation.draw(graphics, getPosition());
}

void Fruit::onTranslationEvent([[maybe_unused]] Pulsar::D2::TranslationEvent &event) {}

void Fruit::onCollisionEvent(const Pulsar::D2::CollisionEvent &event) {
    if (event.getCollidedWidth().getTag() == PlayerDino::TAG) {
        auto &dino = reinterpret_cast<PlayerDino&>(event.getCollidedWidth());
        auto &game = Pulsar::Game::getInstance();
        game.pushScene(new Level(nextLevelFile, dino.getPoints() + 10));
        game.switchToNextScene();
    }
}
