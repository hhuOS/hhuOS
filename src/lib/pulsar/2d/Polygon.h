/*
 * Copyright (C) 2017-2026 Heinrich Heine University Düsseldorf,
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
 * The game engine is based on a bachelor's thesis, written by Malte Sehmer.
 * The original source code can be found here: https://github.com/Malte2036/hhuOS
 *
 * It has been enhanced with 3D-capabilities during a bachelor's thesis by Richard Josef Schweitzer
 * The original source code can be found here: https://git.hhu.de/bsinfo/thesis/ba-risch114
 *
 * The 3D-rendering has been rewritten using OpenGL (TinyGL) during a bachelor's thesis by Kevin Weber
 * The original source code can be found here: https://git.hhu.de/bsinfo/thesis/ba-keweb100
 *
 * The 2D particle system is based on a bachelor's thesis, written by Abdulbasir Gümüs.
 * The original source code can be found here: https://git.hhu.de/bsinfo/thesis/ba-abgue101
 */

#ifndef HHUOS_LIB_PULSAR_2D_POLYGON_H
#define HHUOS_LIB_PULSAR_2D_POLYGON_H

#include "util/collection/Array.h"
#include "util/math/Vector2.h"
#include "pulsar/2d/Entity.h"

namespace Pulsar {
namespace D2 {

/// A 2D polygon entity, defined by a set of vertices.
/// The vertices are defined in local space (i.e. relative to the position of the polygon), with (0,0) being the center.
/// The polygon is always closed, i.e. the last vertex is connected to the first vertex. It can be scaled and rotated.
/// Only its outline is drawn, it can not be filled.
class Polygon : public Entity {

public:
    /// Create a new polygon instance with a set of vertices and a color (used to draw the outline of the polygon).
    explicit Polygon(const size_t tag, const Util::Math::Vector2<float> &position,
        const Util::Array<Util::Math::Vector2<float>> &vertices,
        const Util::Graphic::Color &color = Util::Graphic::Colors::WHITE) :
        Entity(tag, position), vertices(vertices), color(color) {}

    /// Initialize the polygon (does nothing, since polygons have no resource to load or initialize).
    void initialize() final {}

    /// Scale the polygon by the given factor (scales all vertices).
    void scale(float factor) const;

    /// Rotate the polygon by the given angle in degrees (rotates all vertices around the origin).
    void rotate(float angle) const;

    /// Draw the polygon using the given graphics context (draws the outline of the polygon).
    /// This method is called automatically once per frame.
    void draw(Graphics &graphics) const final;

private:

    Util::Array<Util::Math::Vector2<float>> vertices;
    Util::Graphic::Color color;
};

}
}

#endif
