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
 * Battle Space has been implemented during a bachelor's thesis by Richard Josef Schweitzer
 * The original source code can be found here: https://git.hhu.de/bsinfo/thesis/ba-risch114
 */

#ifndef HHUOS_ENEMYDEBRIS_H
#define HHUOS_ENEMYDEBRIS_H

#include <stdint.h>

#include "lib/util/game/3d/Model.h"
#include "lib/util/math/Vector3.h"

class EnemyDebris : public Util::Game::D3::Model {

public:
    /**
     * Constructor.
     */
    EnemyDebris(const Util::Math::Vector3<double> &position, const Util::Math::Vector3<double> &rotation, double scale, uint8_t modelId);

    /**
     * Copy Constructor.
     */
    EnemyDebris(const EnemyDebris &other) = delete;

    /**
     * Assignment operator.
     */
    EnemyDebris &operator=(const EnemyDebris &other) = delete;

    /**
     * Destructor.
     */
    ~EnemyDebris() override = default;

    void initialize() override;

    void onUpdate(double delta) override;

    void onCollisionEvent(Util::Game::D3::CollisionEvent &event) override;

private:

    uint8_t modelId;
    double lifetime = 0;

    Util::Math::Vector3<double> translateDirection;
    Util::Math::Vector3<double> rotationDirection;
};

#endif
