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

#include "Model.h"
#include "lib/util/game/GameManager.h"
#include "lib/util/game/3d/Scene.h"
#include "lib/util/math/Vector2D.h"
#include "lib/util/collection/ArrayList.h"
#include "lib/util/io/stream/FileInputStream.h"
#include "lib/util/io/stream/BufferedInputStream.h"
#include "lib/util/math/Math.h"
#include "lib/util/game/ResourceManager.h"

namespace Util::Game::D3 {

Model::Model(uint32_t tag, const String &modelPath, const Math::Vector3D &position, const Math::Vector3D &rotation, const Math::Vector3D &scale) : Entity(tag, position, rotation, scale, SphereCollider(position, Math::max(scale.getX(), scale.getY(), scale.getZ()))), modelPath(modelPath) {}

Model::Model(uint32_t tag, const String &modelPath, const Math::Vector3D &position, const Math::Vector3D &rotation, const Math::Vector3D &scale, const Graphic::Color &color) : Entity(tag, position, rotation, scale, SphereCollider(position, Math::max(scale.getX(), scale.getY(), scale.getZ()))), modelPath(modelPath), color(color) {}

void Model::initialize() {
    if (!ResourceManager::hasObjectFile(modelPath)) {
        ResourceManager::addObjectFile(modelPath, ObjectFile::open(modelPath));
    }

    objectFile = ResourceManager::getObjectFile(modelPath);
    transformedBuffer = Util::Array<Math::Vector3D>(objectFile->getVertices().length());
    calculateTransformedVertices();
}

void Model::calculateTransformedVertices() {
    const auto &vertices = objectFile->getVertices();
    for (uint32_t i = 0; i < vertices.length(); i++) {
        auto vertex = vertices[i];

        vertex = vertex.scale(getScale());
        vertex = vertex.rotate(getRotation());
        vertex = vertex + getPosition();

        transformedBuffer[i] = vertex;
    }
}

void Model::draw(Graphics &graphics) {
    auto &camera = Util::Game::GameManager::getCurrentScene().getCamera();
    auto minXProjection = graphics.projectPoint(getPosition() + Util::Math::Vector3D(-getCollider().getRadius(), 0, 0), camera.getPosition(), camera.getRotation());
    auto maxXProjection = graphics.projectPoint(getPosition() + Util::Math::Vector3D(getCollider().getRadius(), 0, 0), camera.getPosition(), camera.getRotation());
    auto minYProjection = graphics.projectPoint(getPosition() + Util::Math::Vector3D(0, -getCollider().getRadius(), 0), camera.getPosition(), camera.getRotation());
    auto maxYProjection = graphics.projectPoint(getPosition() + Util::Math::Vector3D(0, getCollider().getRadius(), 0), camera.getPosition(), camera.getRotation());
    auto minZProjection = graphics.projectPoint(getPosition() + Util::Math::Vector3D(0, 0, -getCollider().getRadius()), camera.getPosition(), camera.getRotation());
    auto maxZProjection = graphics.projectPoint(getPosition() + Util::Math::Vector3D(0, 0, getCollider().getRadius()), camera.getPosition(), camera.getRotation());

    if (((minXProjection.getX() >= -1 && minXProjection.getX() <= 1) && (minXProjection.getY() >= -1 && minXProjection.getY() <= 1)) ||
            ((maxXProjection.getX() >= -1 && maxXProjection.getX() <= 1) && (maxXProjection.getY() >= -1 && maxXProjection.getY() <= 1)) ||
            ((minYProjection.getX() >= -1 && minYProjection.getX() <= 1) && (minYProjection.getY() >= -1 && minYProjection.getY() <= 1)) ||
            ((maxYProjection.getX() >= -1 && maxYProjection.getX() <= 1) && (maxYProjection.getY() >= -1 && maxYProjection.getY() <= 1)) ||
            ((minZProjection.getX() >= -1 && minZProjection.getX() <= 1) && (minZProjection.getY() >= -1 && minZProjection.getY() <= 1)) ||
            ((maxZProjection.getX() >= -1 && maxZProjection.getX() <= 1) && (maxZProjection.getY() >= -1 && maxZProjection.getY() <= 1))) {
        graphics.setColor(color);
        graphics.drawModel(transformedBuffer, objectFile->getEdges());
    }
}

void Model::onTransformChange() {
    calculateTransformedVertices();
}

}