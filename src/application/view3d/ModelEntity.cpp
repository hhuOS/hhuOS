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
 * view3d has originally been implemented during a bachelor's thesis by Richard Josef Schweitzer
 * The original source code can be found here: https://git.hhu.de/bsinfo/thesis/ba-risch114
 */

#include "ModelEntity.h"

#include <lib/util/game/Graphics.h>

#include "lib/util/math/Vector3.h"

ModelEntity::ModelEntity(const Util::String &modelPath) : Model(0, modelPath, Util::Math::Vector3<double>(0, 0, 0), Util::Math::Vector3<double>(0, 0, 0), Util::Math::Vector3<double>(1, 1, 1)) {}

ModelEntity::ModelEntity(const Util::String &modelPath, const Util::String &texturePath) : Model(0, modelPath, texturePath, Util::Math::Vector3<double>(0, 0, 0), Util::Math::Vector3<double>(0, 0, 0), Util::Math::Vector3<double>(1, 1, 1)) {}

void ModelEntity::initialize() {
    Model::initialize();

    drawListID = Util::Game::Graphics::startList3D();
    Util::Game::Graphics::listModel3D(*this);
    Util::Game::Graphics::endList3D();
}

void ModelEntity::draw(Util::Game::Graphics &graphics) const {
    graphics.setColor(getColor());
    graphics.drawList3D(getPosition(), getScale(), getRotation(), drawListID);
}

void ModelEntity::onUpdate([[maybe_unused]] double delta) {}

void ModelEntity::onCollisionEvent([[maybe_unused]] Util::Game::D3::CollisionEvent &event) {}
