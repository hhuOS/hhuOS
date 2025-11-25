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

#include "Model.h"

#include "util/math/Math.h"
#include "util/math/Vector3.h"
#include "pulsar/Graphics.h"
#include "pulsar/Resources.h"
#include "pulsar/3d/Entity.h"
#include "pulsar/3d/ObjectFile.h"
#include "pulsar/3d/collider/SphereCollider.h"
#include "pulsar/3d/Texture.h"

namespace Pulsar::D3 {

Model::Model(const size_t tag, const Util::String &modelPath, const Util::Math::Vector3<float> &position,
    const Util::Math::Vector3<float> &rotation, const Util::Math::Vector3<float> &scale,
    const Util::Graphic::Color &color) : Entity(tag, position, rotation, scale,
        SphereCollider(position, Util::Math::max(scale.getX(), scale.getY(), scale.getZ()))),
    modelPath(modelPath), color(color) {}

Model::Model(const size_t tag, const Util::String &modelPath, const Util::String &texturePath,
    const Util::Math::Vector3<float> &position, const Util::Math::Vector3<float> &rotation,
    const Util::Math::Vector3<float> &scale) : Entity(tag, position, rotation, scale,
        SphereCollider(position, Util::Math::max(scale.getX(), scale.getY(), scale.getZ()))),
    modelPath(modelPath), texturePath(texturePath) {}

void Model::initialize() {
    if (!Resources::hasObjectFile(modelPath)) {
        Resources::addObjectFile(modelPath, new ObjectFile(modelPath));
    }

    objectFile = Resources::getObjectFile(modelPath);

    if (!texturePath.isEmpty()) {
        if (!Resources::hasTexture(texturePath)) {
            Resources::addTexture(texturePath, new Texture(texturePath));
        }

        texture = Resources::getTexture(texturePath);
    }
}

void Model::draw(Graphics &graphics) const {
    graphics.setColor(color);
    graphics.drawModel3D(*this);
}

const Util::Array<Util::Math::Vector3<float>> &Model::getVertices() const {
    return objectFile->getVertices();
}

const Util::Array<Util::Math::Vector3<float>> &Model::getVertexNormals() const {
    return objectFile->getVertexNormals();
}

const Util::Array<Util::Math::Vector3<float>> &Model::getVertexTextures() const {
    return objectFile->getVertexTextures();
}

const Util::Array<size_t> &Model::getVertexDrawOrder() const {
    return objectFile->getVertexDrawOrder();
}

const Util::Array<size_t> &Model::getNormalDrawOrder() const {
    return objectFile->getNormalDrawOrder();
}

const Util::Array<size_t> &Model::getTextureDrawOrder() const {
    return objectFile->getTextureDrawOrder();
}

const Texture& Model::getTexture() const {
    if (texture ==  nullptr) {
        return Texture::INVALID_TEXTURE;
    }

    return *texture;
}

const Util::Graphic::Color& Model::getColor() const {
    return color;
}

}
