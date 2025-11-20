/*
 * Copyright (C) 2017-2025 Heinrich Heine University Düsseldorf,
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

#ifndef HHUOS_LIB_PULSAR_3D_TEXTURE_H
#define HHUOS_LIB_PULSAR_3D_TEXTURE_H

#include "util/base/String.h"
#include "tinygl/include/GL/gl.h"

namespace Pulsar::D3 {

/// Represents an OpenGL texture loaded from an image file for use in 3D rendering.
/// The texture is loaded from the specified file path and assigned a unique OpenGL texture ID
/// via glGenTextures()/glBindTexture(). This ID can be used with the `Graphics` class for rendering textured 3D models.
class Texture {

public:
    /// Create a new invalid texture instance without loading any image (ID = 0).
    Texture() = default;

    /// Create a new texture instance by loading the image from the given file path.
    explicit Texture(const Util::String &path);

    /// Check if the texture is valid (i.e. has a non-zero OpenGL texture ID).
    [[nodiscard]] bool isValid() const;

    /// Get the OpenGL texture ID assigned to this texture.
    [[nodiscard]] GLuint getTextureID() const;

private:

    GLuint textureId = 0;
};

}

#endif
