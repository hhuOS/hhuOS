/*
 * Copyright (C) 2018-2022 Heinrich-Heine-Universitaet Duesseldorf,
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

#ifndef HHUOS_POLYGON_H
#define HHUOS_POLYGON_H

#include "lib/util/data/Array.h"
#include "Drawable.h"

namespace Util {
namespace Game {
class Graphics2D;
}  // namespace Game
}  // namespace Util

namespace Util::Game {

class Polygon : public Drawable {

public:
    /**
     * Constructor.
     */
    Polygon(const Data::Array<double> &x, const Data::Array<double> &y);

    /**
     * Copy Constructor.
     */
    Polygon(const Polygon &other) = default;

    /**
     * Assignment operator.
     */
    Polygon &operator=(const Polygon &other) = default;

    /**
     * Destructor.
     */
    ~Polygon() override = default;

    void scale(double factor);

    void rotate(double angle);

    void translate(double xTranslation, double yTranslation);

    void draw(Graphics2D &graphics) const override;

private:

    void calculateCenter();

    Data::Array<double> x;
    Data::Array<double> y;
    double xCenter = 0;
    double yCenter = 0;
};

}

#endif
