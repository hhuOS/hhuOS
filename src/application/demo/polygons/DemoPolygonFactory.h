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

#ifndef HHUOS_APPLICATION_DEMO_DEMOPOLYGONFACTORY_H
#define HHUOS_APPLICATION_DEMO_DEMOPOLYGONFACTORY_H

#include "DemoPolygon.h"

#include <util/collection/Array.h>
#include <util/math/Random.h>
#include <util/math/Vector2.h>

/// A factory class that creates random 2D polygons on request.
class DemoPolygonFactory {

public:
    /// Create a new polygon factory instance.
    DemoPolygonFactory() = default;

    /// Create a new demo polygon.
    /// The polygon instance is created on the heap, and the caller is responsible for freeing it.
    DemoPolygon* createPolygon();

private:

    Util::Math::Random random;

    static const Util::Array<Util::Array<Util::Math::Vector2<float>>> SHAPES;
};

#endif