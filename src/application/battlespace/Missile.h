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

#ifndef HHUOS_APPLICATION_BATTLESPACE_MISSILE_H
#define HHUOS_APPLICATION_BATTLESPACE_MISSILE_H

#include <stddef.h>

#include <pulsar/3d/Model.h>

class Player;

/// A missile that can either be shot by the player, or by an enemy ship.
/// It causes damage on collision with either of the two.
/// If it collides with another missile, both missiles are destroyed.
/// If the player manages to destroy an enemy missile with one of its own, they gain points.
class Missile : public Pulsar::D3::Model {

public:
    /// Create a new enemy missile facing the given direction.
    /// The player reference is needed to grant points if the missile is destroyed.
    Missile(Player &player, const Util::Math::Vector3<float> &position, const Util::Math::Vector3<float> &direction);

    /// Create a new player missile facing the given direction.
    Missile(const Util::Math::Vector3<float> &position, const Util::Math::Vector3<float> &direction);

    /// Update the missile's position according to its direction and the given delta time.
    void onUpdate(float delta) override;

    /// Destroy the missile on collision with another object.
    /// If it was shot by an enemy ship the player gains points.
    void onCollisionEvent(const Pulsar::D3::CollisionEvent &event) override;

    /// Unique tag to distinguish missiles from other object types in collisions.
    static constexpr size_t TAG = 2;

private:

    Player *player = nullptr;
    float lifetime = 0;

    static constexpr float START_SPEED = 0.04;
    static constexpr float FULL_SPEED = 0.2;
    static constexpr float START_SPEED_TIME = 0.5;
};

#endif
