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

#ifndef HHUOS_APPLICATION_BATTLESPACE_ENEMY_DEBRIS_H
#define HHUOS_APPLICATION_BATTLESPACE_ENEMY_DEBRIS_H

#include <stdint.h>

#include <pulsar/3d/Model.h>
#include <util/math/Vector3.h>

/// A piece of debris that is spawned when an enemy ships is destroyed.
/// On enemy destruction three of such debris are spawned and fly away in distinct directions, simulating an explosion.
/// It uses the same tag as enemy ships, so that collisions with them also cause damage to the player and other enemies.
class EnemyDebris : public Pulsar::D3::Model {

public:
    /// Create a new debris object with a position, rotation and scale.
    /// The model id must be a number between 1 and 3 and specifies which 3D model is loaded to render the debris.
    /// The model is loaded from "/user/battlespace/debris<modelId>.obj"
    /// The model is also determines the direction in which the object moves.
    /// Each debris object rotates around itself.
    /// The rotation vector is made up of random numbers during initialization.
    EnemyDebris(uint8_t modelId, const Util::Math::Vector3<float> &position, const Util::Math::Vector3<float> &rotation,
        float scale);

    /// Initialize the debris object, loading the 3D model.
    void initialize() override;

    /// Update the debris object's position and rotation according to delta time.
    void onUpdate(float delta) override;

private:

    const uint8_t modelId;
    float lifetime = 2.0f;

    Util::Math::Vector3<float> translateDirection;
    Util::Math::Vector3<float> rotationDirection;
};

#endif
