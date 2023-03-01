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
 *
 * The network stack is based on a bachelor's thesis, written by Malte Sehmer.
 * The original source code can be found here: https://git.hhu.de/bsinfo/thesis/ba-maseh100
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
     * Default Constructor.
     */
    Sprite();

    /**
     * Constructor.
     */
    Sprite(const String &path, double width, double height);

    /**
     * Copy Constructor.
     */
    Sprite(const Sprite &other) = default;

    /**
     * Assignment operator.
     */
    Sprite &operator=(const Sprite &other) = default;

    /**
     * Destructor.
     */
    ~Sprite() = default;

    [[nodiscard]] const Graphic::Image& getImage() const;

    [[nodiscard]] double getWidth() const;

    [[nodiscard]] double getHeight() const;

private:

    Graphic::Image *image;
    double width;
    double height;
};

}

#endif
