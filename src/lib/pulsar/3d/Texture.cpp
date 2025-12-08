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

#include <stddef.h>

#include "Texture.h"

#include "util/graphic/BitmapFile.h"
#include "util/graphic/Color.h"
#include "pulsar/Resources.h"

namespace Pulsar {
namespace D3 {

const Texture Texture::INVALID_TEXTURE = Texture();

Texture::Texture(const Util::String &path) {
    const auto *image = Util::Graphic::BitmapFile::open(path);
    const auto *imagePixels = image->getPixelBuffer();

    // The Image class stores pixels as an array of the struct 'Color'.
    // We need to convert this to an array of bytes in the RGB format (24-bit).
    auto *textureData = new uint8_t[image->getWidth() * image->getHeight() * 3];
    for (size_t i = 0; i < image->getWidth() * image->getHeight(); i++) {
        auto color = imagePixels[i];
        textureData[i * 3] = color.getRed();
        textureData[i * 3 + 1] = color.getGreen();
        textureData[i * 3 + 2] = color.getBlue();
    }

    // Generate the OpenGL texture
    glGenTextures(1, &textureId); // Generate a texture ID
    glBindTexture(GL_TEXTURE_2D, static_cast<GLint>(textureId)); // Tell OpenGL which texture to edit
    glTexImage2D(GL_TEXTURE_2D, // Type of texture
        0,                      // Mipmap level, 0 for base
        3,                      // Number of color components in texture
        image->getWidth(),      // Width of the texture
        image->getHeight(),     // Height of the texture
        0,                      // Border width in pixels
        GL_RGB,                 // Format of pixel data
        GL_UNSIGNED_BYTE,       // Type of pixel data
        textureData);           // Pointer to the image data

    // The texture data is now stored by OpenGL, we can delete our copy
    delete image;
    delete[] textureData;
}

}
}