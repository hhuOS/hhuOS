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
 */

#include "Model.h"

#include "lib/util/math/Math.h"
#include "lib/util/game/Resources.h"
#include "lib/util/game/3d/Entity.h"
#include "lib/util/game/3d/ObjectFile.h"
#include "lib/util/game/3d/collider/SphereCollider.h"
#include "lib/util/game/Graphics.h"
#include "lib/util/math/Vector3.h"
#include "lib/util/game/3d/Texture.h"

namespace Util {
template <typename T> class Array;
}  // namespace Util

namespace Util::Game::D3 {

Model::Model(uint32_t tag, const String &modelPath, const Math::Vector3<double> &position, const Math::Vector3<double> &rotation, const Math::Vector3<double> &scale, const Graphic::Color &color) : Entity(tag, position, rotation, scale, SphereCollider(position, Math::max(scale.getX(), scale.getY(), scale.getZ()))), modelPath(modelPath), color(color) {}

Model::Model(uint32_t tag, const String &modelPath, const String &texturePath, const Math::Vector3<double> &position, const Math::Vector3<double> &rotation, const Math::Vector3<double> &scale) : Entity(tag, position, rotation, scale, SphereCollider(position, Math::max(scale.getX(), scale.getY(), scale.getZ()))), modelPath(modelPath), texturePath(texturePath) {}

void Model::initialize() {
    if (!Resources::hasObjectFile(modelPath)) {
        Resources::addObjectFile(modelPath, ObjectFile::open(modelPath));
    }

    objectFile = Resources::getObjectFile(modelPath);

    if (!texturePath.isEmpty()) {
        texture = Texture(texturePath);
    }
}

void Model::draw(Graphics &graphics) const {
    graphics.setColor(color);
    graphics.drawModel3D(*this);
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

const Texture& Model::getTexture() const {
    return texture;
}

const Graphic::Color& Model::getColor() const {
    return color;
}

}
