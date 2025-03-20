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

#include "Texture.h"

#include "lib/util/game/ResourceManager.h"
#include "lib/util/graphic/BitmapFile.h"
#include "lib/util/graphic/Color.h"

Util::Game::D3::Texture::Texture(const String &path) {
    if (ResourceManager::hasTexture(path)) {
        textureID = ResourceManager::getTexture(path);
    } else {
        const auto *image = Graphic::BitmapFile::open(path);
        const auto *imagePixels = image->getPixelBuffer();

        // The Image class stores pixels as an array of the struct 'Color'.
        // We need to convert this to an array of bytes in the RGB format (24-bit).
        auto *textureData = new uint8_t[image->getWidth() * image->getHeight() * 3];
        for (uint32_t i = 0; i < image->getWidth() * image->getHeight(); i++) {
            auto color = imagePixels[i];
            textureData[i * 3] = color.getRed();
            textureData[i * 3 + 1] = color.getGreen();
            textureData[i * 3 + 2] = color.getBlue();
        }

        // Generate the OpenGL texture
        glGenTextures(1, &textureID); // Generate a texture ID
        glBindTexture(GL_TEXTURE_2D, textureID); // Tell OpenGL which texture to edit
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

        // Add texture to the ResourceManager
        ResourceManager::addTexture(path, textureID);
    }
}

GLuint Util::Game::D3::Texture::getTextureID() const {
    return textureID;
}
