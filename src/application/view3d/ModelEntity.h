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

#ifndef HHUOS_VIEW3D_MODEL_H
#define HHUOS_VIEW3D_MODEL_H

#include "lib/util/game/3d/Model.h"
#include "lib/util/base/String.h"

namespace Util {
namespace Game {
namespace D3 {
class CollisionEvent;
}  // namespace D3
}  // namespace Game
}  // namespace Util

class ModelEntity : public Util::Game::D3::Model {

public:
    /**
     * Constructor.
     */
    explicit ModelEntity(const Util::String &modelPath);

    /**
     * Copy Constructor.
     */
    ModelEntity(const ModelEntity &other) = delete;

    /**
     * Assignment operator.
     */
    ModelEntity &operator=(const ModelEntity &other) = delete;

    /**
     * Destructor.
     */
    ~ModelEntity() override = default;

    void onUpdate(double delta) override;

    void onCollisionEvent(Util::Game::D3::CollisionEvent &event) override;
};

#endif
