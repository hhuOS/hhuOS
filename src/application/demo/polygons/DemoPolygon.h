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
 */

#ifndef HHUOS_APPLICATION_DEMO_DEMOPOLYGON_H
#define HHUOS_APPLICATION_DEMO_DEMOPOLYGON_H

#include <util/collection/Array.h>
#include <util/math/Vector2.h>
#include <util/graphic/Color.h>
#include <pulsar/2d/Polygon.h>

/// A 2D polygon that rotates and scales up and down over time.
class DemoPolygon : public Pulsar::D2::Polygon {

public:
    /// Create a new 2D polygon of the given vertices.
    /// The initial scale factor dictates the scale applied from the start.
    /// The polygon grows and shrinks around that scale factor.
    DemoPolygon(const Util::Array<Util::Math::Vector2<float>> &vertices, const Util::Math::Vector2<float> &position,
        const Util::Graphic::Color &color, float initialScaleFactor, float scaleSpeed, float rotationSpeed);

    /// Rotate and scale the polygon according to the given delta time.
    void onUpdate(float delta) override;

private:

    const float rotationSpeed = 0;
    const float scaleSpeed = 0;

    float currentScale = 1;
    bool scaleUp = true;
};

#endif