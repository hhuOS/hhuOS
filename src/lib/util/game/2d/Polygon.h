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
 * The game engine is based on a bachelor's thesis, written by Malte Sehmer.
 * The original source code can be found here: https://github.com/Malte2036/hhuOS
 *
 * It has been enhanced with 3D-capabilities during a bachelor's thesis by Richard Josef Schweitzer
 * The original source code can be found here: https://git.hhu.de/bsinfo/thesis/ba-risch114
 */

#ifndef HHUOS_POLYGON_H
#define HHUOS_POLYGON_H

#include "lib/util/collection/Array.h"
#include "lib/util/math/Vector2D.h"
#include "lib/util/game/Drawable.h"

namespace Util {
namespace Game {
class Graphics;
}  // namespace Game
}  // namespace Util

namespace Util::Game::D2 {

class Polygon : public Drawable {

public:
    /**
     * Constructor.
     */
    explicit Polygon(const Array<Math::Vector2D> &vertices);

    /**
     * Copy Constructor.
     */
    Polygon(const Polygon &other) = delete;

    /**
     * Assignment operator.
     */
    Polygon &operator=(const Polygon &other) = delete;

    /**
     * Destructor.
     */
    ~Polygon() override = default;

    [[nodiscard]] const Math::Vector2D& getCenter() const;

    [[nodiscard]] const Util::Array<Math::Vector2D>& getVertices() const;

    void scale(double factor);

    void rotate(double angle);

    void translate(Math::Vector2D translation);

    void setPosition(const Math::Vector2D &newPosition);

    void draw(Graphics &graphics) override;

private:

    void calculateCenter();

    Math::Vector2D getTopLeft();

    Array<Math::Vector2D> vertices;
    Math::Vector2D center;
};

}

#endif
