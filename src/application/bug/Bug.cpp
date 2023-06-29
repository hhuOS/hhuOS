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

#include "Bug.h"
#include "lib/util/game/entity/component/LinearMovementComponent.h"
#include "lib/util/game/GameManager.h"

Bug::Bug(const Util::Math::Vector2D &position) : Util::Game::Entity(TAG, position, Util::Game::RectangleCollider(position, Util::Game::Collider::STATIC, SIZE_X, SIZE_Y)) {
    addComponent(new Util::Game::LinearMovementComponent(*this));
}

void Bug::initialize() {
    animation = Util::Game::SpriteAnimation(Util::Array<Util::Game::Sprite>({
        Util::Game::Sprite("/initrd/bug/bug1.bmp", SIZE_X, SIZE_Y),
        Util::Game::Sprite("/initrd/bug/bug2.bmp", SIZE_X, SIZE_Y)}), 0.5);
}

void Bug::onUpdate(double delta) {
    animation.update(delta);
}

void Bug::onTranslationEvent(Util::Game::TranslationEvent &event) {}

void Bug::onCollisionEvent(Util::Game::CollisionEvent &event) {
    Util::Game::GameManager::getCurrentScene().removeObject(this);
}

void Bug::draw(Util::Game::Graphics2D &graphics) {
    graphics.drawImage(getPosition(), animation.getCurrentSprite().getImage());
}

void Bug::fireMissile() {
    mayFireMissile = false;
}

void Bug::allowFireMissile() {
    mayFireMissile = true;
}
