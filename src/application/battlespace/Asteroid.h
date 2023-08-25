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

#ifndef HHUOS_ASTEROID_H
#define HHUOS_ASTEROID_H

#include "lib/util/game/3d/Model.h"
#include "lib/util/math/Random.h"

class Asteroid : public Util::Game::D3::Model {

public:
    /**
     * Constructor.
     */
    Asteroid(const Util::Math::Vector3D &position, const Util::Math::Vector3D &rotation, const Util::Math::Vector3D &scale, uint8_t modelId);

    /**
     * Copy Constructor.
     */
    Asteroid(const Asteroid &other) = delete;

    /**
     * Assignment operator.
     */
    Asteroid &operator=(const Asteroid &other) = delete;

    /**
     * Destructor.
     */
    ~Asteroid() override = default;

    void onUpdate(double delta) override;

    void onCollisionEvent(Util::Game::D3::CollisionEvent &event) override;

    static const constexpr uint32_t TAG = 1;
};

#endif
