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

#ifndef HHUOS_DEMOPOLYGON_H
#define HHUOS_DEMOPOLYGON_H

#include "lib/util/game/Polygon.h"
#include "lib/util/collection/Array.h"
#include "lib/util/graphic/Color.h"

namespace Util {
namespace Math {
class Vector2D;
}  // namespace Math

namespace Game {
class Graphics2D;
class CollisionEvent;
class TranslationEvent;
}  // namespace Game
}  // namespace Util

class DemoPolygon : public Util::Game::Polygon {

public:
    /**
     * Default Constructor.
     */
    DemoPolygon();

    /**
     * Constructor.
     */
    DemoPolygon(const Util::Array<Util::Math::Vector2D> &vertices, const Util::Graphic::Color &color, double rotationSpeed, double scaleSpeed);

    /**
     * Copy Constructor.
     */
    DemoPolygon(const DemoPolygon &other) = delete;

    /**
     * Assignment operator.
     */
    DemoPolygon &operator=(const DemoPolygon &other) = delete;

    /**
     * Destructor.
     */
    ~DemoPolygon() override = default;

    void initialize();

    void update(double delta);

    void onTranslationEvent(Util::Game::TranslationEvent &event) override;

    void onCollisionEvent(Util::Game::CollisionEvent &event) override;

    void draw(Util::Game::Graphics2D &graphics) override;

private:

    Util::Graphic::Color color;
    double rotationSpeed;
    double scaleSpeed;
    double currentScale = 1;
    bool scaleUp = true;
};

#endif