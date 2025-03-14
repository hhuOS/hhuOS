/*
 * Copyright (C) 2018-2024 Heinrich-Heine-Universitaet Duesseldorf,
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

#include "lib/util/game/GameManager.h"
#include "lib/util/math/Vector2D.h"
#include "lib/util/math/Math.h"
#include "lib/util/game/ResourceManager.h"
#include "lib/util/game/3d/Entity.h"
#include "lib/util/game/3d/ObjectFile.h"
#include "lib/util/game/3d/collider/SphereCollider.h"
#include "lib/util/game/Camera.h"
#include "lib/util/game/Graphics.h"
#include "lib/util/game/Scene.h"

namespace Util::Game::D3 {

Model::Model(uint32_t tag, const String &modelPath, const Math::Vector3D &position, const Math::Vector3D &rotation, const Math::Vector3D &scale) : Entity(tag, position, rotation, scale, SphereCollider(position, Math::max(scale.getX(), scale.getY(), scale.getZ()))), modelPath(modelPath) {}

Model::Model(uint32_t tag, const String &modelPath, const Math::Vector3D &position, const Math::Vector3D &rotation, const Math::Vector3D &scale, const Graphic::Color &color) : Entity(tag, position, rotation, scale, SphereCollider(position, Math::max(scale.getX(), scale.getY(), scale.getZ()))), modelPath(modelPath), color(color) {}

void Model::initialize() {
    if (!ResourceManager::hasObjectFile(modelPath)) {
        ResourceManager::addObjectFile(modelPath, ObjectFile::open(modelPath));
    }

    objectFile = ResourceManager::getObjectFile(modelPath);
}

void Model::draw(Graphics &graphics) {
    graphics.setColor(color);
    graphics.drawModel(*this);
}

const Array<Math::Vector3D> &Model::getVertices() const {
    return objectFile->getVertices();
}

const Array<Math::Vector3D> &Model::getVertexNormals() const {
    return objectFile->getVertexNormals();
}

const Array<Math::Vector3D> &Model::getVertexTextures() const {
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

}