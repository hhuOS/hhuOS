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
 */

#ifndef HHUOS_APPLICATION_BUG_DEFENDER_ENEMYBUG_H
#define HHUOS_APPLICATION_BUG_DEFENDER_ENEMYBUG_H

#include <stddef.h>

#include "Explosive.h"

#include <pulsar/2d/SpriteAnimation.h>

class Fleet;

/// An enemy bug, moving from side to side on the screen.
/// Bugs are organized in a `Fleet`, forming a grid of enemies.
/// Each time the grid hits a left or right boundary, it moves down a bit.
/// Each bug fires missiles downwards in random time intervals.
/// When a bug has moved so far down that it touches the player, the game is lost.
class EnemyBug : public Explosive {

public:
    /// Create a new enemy bug instance belonging to the given fleet.
    explicit EnemyBug(const Util::Math::Vector2<float> &position, Fleet &fleet);

    /// Initialize the bug instance, loading its animation sprites.
    void initialize() override;

    /// Update the enemy bug by moving it left or right, according to the given time delta.
    /// This method also checks if the bug should fire a missile by generating a random number.
    void onUpdate(float delta) override;

    /// Handle translation events.
    /// If a bug hits the left or right boundary, the whole fleet moves downwards.
    void onTranslationEvent(Pulsar::D2::TranslationEvent &event) override;

    /// Handle a collision with another entity.
    /// Bugs can only collide with player missiles, in which case they are instantly destroyed.
    void onCollisionEvent(const Pulsar::D2::CollisionEvent &event) override;

    /// Draw the bug using its current animation frame.
    void draw(Pulsar::Graphics &graphics) const override;

    /// Fire a missile downwards.
    void fireMissile() const;

    /// Unique tag to distinguish bugs from other object types in collisions.
    static constexpr size_t TAG = 3;
    /// Width of a bug in game coordinates.
    static constexpr float WIDTH = 0.15;
    /// Height of a bug in game coordinates.
    static constexpr float HEIGHT = 0.1;

private:

    Pulsar::D2::SpriteAnimation animation;
    Fleet &fleet;

    float lastMissileRollTime = 0;
};

#endif
