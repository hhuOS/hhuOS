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

#ifndef HHUOS_DRAWABLE_H
#define HHUOS_DRAWABLE_H

#include "Graphics2D.h"

namespace Util::Game {

class Drawable {

public:
    /**
     * Default Constructor.
     */
    Drawable() = default;

    /**
     * Copy Constructor.
     */
    Drawable(const Drawable &other) = delete;

    /**
     * Assignment operator.
     */
    Drawable &operator=(const Drawable &other) = delete;

    /**
     * Destructor.
     */
    ~Drawable() = default;

    virtual void draw(Graphics2D &lfb) const = 0;
};

}

#endif
