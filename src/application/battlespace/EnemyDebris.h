/*
 * Copyright (C) 2017-2026 Heinrich Heine University Düsseldorf,
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
 * Battle Space has been implemented during a bachelor's thesis by Richard Josef Schweitzer
 * The original source code can be found here: https://git.hhu.de/bsinfo/thesis/ba-risch114
 */

#ifndef HHUOS_ENEMYDEBRIS_H
#define HHUOS_ENEMYDEBRIS_H

#include <stdint.h>

#include "lib/pulsar/3d/Model.h"
#include "lib/util/math/Vector3.h"

class EnemyDebris : public Pulsar::D3::Model {

public:
    /**
     * Constructor.
     */
    EnemyDebris(const Util::Math::Vector3<float> &position, const Util::Math::Vector3<float> &rotation, float scale, uint8_t modelId);

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

    void onUpdate(float delta) override;

    void onCollisionEvent(const Pulsar::D3::CollisionEvent &event) override;

private:

    uint8_t modelId;
    float lifetime = 0;

    Util::Math::Vector3<float> translateDirection;
    Util::Math::Vector3<float> rotationDirection;
};

#endif
