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

#ifndef HHUOS_SPRITE_H
#define HHUOS_SPRITE_H

#include "lib/util/base/String.h"

namespace Util {
namespace Graphic {
class Image;
}  // namespace Graphic
}  // namespace Util

namespace Util::Game {

class Sprite {

public:
    /**
     * Constructor.
     */
    Sprite(const String &path, double width, double height);

    /**
     * Copy Constructor.
     */
    Sprite(const Sprite &other) = delete;

    /**
     * Assignment operator.
     */
    Sprite &operator=(const Sprite &other) = delete;

    /**
     * Destructor.
     */
    ~Sprite();

    [[nodiscard]] const Graphic::Image& getImage() const;

private:

    Graphic::Image *image;
};

}

#endif
