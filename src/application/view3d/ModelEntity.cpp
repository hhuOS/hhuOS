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
 */

#include "ModelEntity.h"

#include "lib/util/math/Vector3D.h"

namespace Util {
namespace Game {
namespace D3 {
class CollisionEvent;
}  // namespace D3
}  // namespace Game
}  // namespace Util

ModelEntity::ModelEntity(const Util::String &modelPath) : Util::Game::D3::Model(0, modelPath, Util::Math::Vector3D(0, 0, 3), Util::Math::Vector3D(0, 0, 0), Util::Math::Vector3D(1, 1, 1)) {}

void ModelEntity::onUpdate([[maybe_unused]] double delta) {}

void ModelEntity::onCollisionEvent([[maybe_unused]] Util::Game::D3::CollisionEvent &event) {}
