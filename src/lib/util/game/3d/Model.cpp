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
 *
 * The game engine is based on a bachelor's thesis, written by Malte Sehmer.
 * The original source code can be found here: https://github.com/Malte2036/hhuOS
 *
 * It has been enhanced with 3D-capabilities during a bachelor's thesis by Richard Josef Schweitzer
 * The original source code can be found here: https://git.hhu.de/bsinfo/thesis/ba-risch114
 *
 * The 3D-rendering has been rewritten using OpenGL (TinyGL) during a bachelor's thesis by Kevin Weber
 * The original source code can be found here: https://git.hhu.de/bsinfo/thesis/ba-keweb100
 */

#include "Model.h"

#include "lib/util/graphic/BitmapFile.h"
#include "lib/util/math/Math.h"
#include "lib/util/game/ResourceManager.h"
#include "lib/util/game/3d/Entity.h"
#include "lib/util/game/3d/ObjectFile.h"
#include "lib/util/game/3d/collider/SphereCollider.h"
#include "lib/util/game/Graphics.h"
#include "lib/util/math/Vector3.h"

namespace Util {
template <typename T> class Array;
}  // namespace Util

namespace Util::Game::D3 {

Model::Model(uint32_t tag, const String &modelPath, const Math::Vector3<double> &position, const Math::Vector3<double> &rotation, const Math::Vector3<double> &scale, const Graphic::Color &color) : Entity(tag, position, rotation, scale, SphereCollider(position, Math::max(scale.getX(), scale.getY(), scale.getZ()))), modelPath(modelPath), color(color) {}

Model::Model(uint32_t tag, const String &modelPath, const String &texturePath, const Math::Vector3<double> &position, const Math::Vector3<double> &rotation, const Math::Vector3<double> &scale) : Entity(tag, position, rotation, scale, SphereCollider(position, Math::max(scale.getX(), scale.getY(), scale.getZ()))), modelPath(modelPath), texturePath(texturePath) {}

void Model::initialize() {
    if (!ResourceManager::hasObjectFile(modelPath)) {
        ResourceManager::addObjectFile(modelPath, ObjectFile::open(modelPath));
    }

    objectFile = ResourceManager::getObjectFile(modelPath);

    if (!texturePath.isEmpty()) {
        if (!ResourceManager::hasTexture(texturePath)) {
            const auto *image = Graphic::BitmapFile::open(texturePath);
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
            GLuint textureID;
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
            ResourceManager::addTexture(texturePath, textureID);
        }

        textureID = ResourceManager::getTexture(texturePath);
    }
}

void Model::draw(Graphics &graphics) {
    graphics.setColor(color);
    graphics.drawModel(*this);
}

const Array<Math::Vector3<double>> &Model::getVertices() const {
    return objectFile->getVertices();
}

const Array<Math::Vector3<double>> &Model::getVertexNormals() const {
    return objectFile->getVertexNormals();
}

const Array<Math::Vector3<double>> &Model::getVertexTextures() const {
    return objectFile->getVertexTextures();
}

const Array<uint32_t> &Model::getVertexDrawOrder() const {
    return objectFile->getVertexDrawOrder();
}

const Array<uint32_t> &Model::getNormalDrawOrder() const {
    return objectFile->getNormalDrawOrder();
}

const Array<uint32_t> &Model::getTextureDrawOrder() const {
    return objectFile->getTextureDrawOrder();
}

GLuint Model::getTextureID() const {
    return textureID;
}

}
