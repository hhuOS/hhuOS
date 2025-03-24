/*
 * Copyright (C) 2018-2025 Heinrich-Heine-Universitaet Duesseldorf,
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

#ifndef TEXTURE_H
#define TEXTURE_H

#include "lib/util/base/String.h"
#include "lib/tinygl/include/GL/gl.h"

namespace Util::Game::D3 {

class Texture {

public:
    /**
     * Default Constructor.
     */
    Texture() = default;

    /**
     * Constructor.
     */
    explicit Texture(const String &path);

    /**
     * Copy Constructor.
     */
    Texture(const Texture &other) = default;

    /**
     * Assignment operator.
     */
    Texture &operator=(const Texture &other) = default;

    /**
     * Destructor.
     */
    ~Texture() = default;

    [[nodiscard]] GLuint getTextureID() const;

private:

    GLuint textureId = 0;
};

}

#endif
