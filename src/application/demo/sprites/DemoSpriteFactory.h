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

#ifndef HHUOS_APPLICATION_DEMO_DEMOSPRITEFACTORY_H
#define HHUOS_APPLICATION_DEMO_DEMOSPRITEFACTORY_H

#include "DemoSprite.h"

#include <util/math/Random.h>

/// A factory class that creates random demo sprites on request.
class DemoSpriteFactory {

public:
    /// Create a new demo sprite factory instance.
    DemoSpriteFactory() = default;

    /// Create a new demo sprite.
    /// The sprite instance is created on the heap, and the caller is responsible for freeing it.
    DemoSprite* createSprite();

private:

    Util::Math::Random random;
};

#endif
