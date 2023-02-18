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

#include "Ground.h"

Ground::Ground(const Util::Math::Vector2D &position) : Util::Game::Entity(TAG, position, Util::Game::RectangleCollider(position, Util::Game::Collider::DYNAMIC, 5.0, 0.001)) {}

void Ground::onUpdate(double delta) {}

void Ground::onTranslationEvent(Util::Game::TranslationEvent &event) {
    event.cancel();
}

void Ground::onCollisionEvent(Util::Game::CollisionEvent &event) {}

void Ground::draw(Util::Game::Graphics2D &graphics) {
    graphics.setColor(Util::Graphic::Colors::GREEN);
    graphics.drawLine(getPosition(), Util::Math::Vector2D(getPosition().getX() + getCollider().getWidth(), getPosition().getY()));
}
