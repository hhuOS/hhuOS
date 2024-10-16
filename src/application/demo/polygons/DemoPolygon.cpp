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

#include "DemoPolygon.h"

#include "lib/util/game/Graphics.h"
#include "lib/util/math/Vector2D.h"
#include "lib/util/game/2d/event/TranslationEvent.h"

namespace Util {
namespace Game {
namespace D2 {
class CollisionEvent;
}  // namespace D2
}  // namespace Game
}  // namespace Util

DemoPolygon::DemoPolygon() : Util::Game::D2::Entity(0, Util::Math::Vector2D(0, 0)), polygon(Util::Array<Util::Math::Vector2D>(0)), color(0, 0, 0), rotationSpeed(0), scaleSpeed(0) {}

DemoPolygon::DemoPolygon(const Util::Array<Util::Math::Vector2D> &vertices, const Util::Math::Vector2D &position, const Util::Graphic::Color &color, double initialScaleFactor, double scaleSpeed, double rotationSpeed) :
        Util::Game::D2::Entity(0, Util::Math::Vector2D(0, 0)), polygon(vertices), color(color), rotationSpeed(rotationSpeed), scaleSpeed(scaleSpeed) {
    setPosition(position);
    polygon.setPosition(getPosition());
    polygon.scale(initialScaleFactor);
}

void DemoPolygon::initialize() {}

void DemoPolygon::onUpdate(double delta) {
    const double rotationAngle = delta * rotationSpeed;
    const double scaleFactor = scaleUp ? 1 + (delta * scaleSpeed) : 1 - (delta * scaleSpeed);
    currentScale *= scaleFactor;
    if (currentScale >= 2) {
        scaleUp = false;
    } else if (currentScale <= 0.5) {
        scaleUp = true;
    }

    polygon.rotate(rotationAngle);
    polygon.scale(scaleFactor);
}

void DemoPolygon::draw(Util::Game::Graphics &graphics) {
    graphics.setColor(color);
    polygon.draw(graphics);
}

void DemoPolygon::onTranslationEvent(Util::Game::D2::TranslationEvent &event) {
    polygon.setPosition(event.getTargetPosition());
}

void DemoPolygon::onCollisionEvent([[maybe_unused]] Util::Game::D2::CollisionEvent &event) {}
