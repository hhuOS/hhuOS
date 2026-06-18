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

#ifndef HHUOS_APPLICATION_BUG_ENEMYMISSILE_H
#define HHUOS_APPLICATION_BUG_ENEMYMISSILE_H

#include <stddef.h>

#include "Explosive.h"
#include "EnemyBug.h"

#include <pulsar/2d/Sprite.h>

/// A missile that was fired by an enemy bug.
/// It moves downwards from the position it was shot.
/// It can collide with the player and player missiles.
/// It shows an explosion animation when it is destroyed.
class EnemyMissile : public Explosive {

public:
    /// Create a new enemy missile object at the given position.
    explicit EnemyMissile(const Util::Math::Vector2<float> &position);

    /// Initialize the enemy missile instance, loading its sprite and explosion animation.
    void initialize() override;

    /// Update the explosion animation if the missile is currently exploding.
    /// Remove the missile if the explosion animation has finished.
    void onUpdate(float delta) override;

    /// Handle translation events.
    /// Check if the new y-coordinate is less than -1.0 (i.e., the missile leaves the screen).
    /// If so, remove the missile.
    void onTranslationEvent(Pulsar::D2::TranslationEvent &event) override;

    /// Handle a collision with another entity.
    /// Enemy missiles can collide with the player and player missiles.
    /// In both cases, the enemy missile is destroyed and starts playing its explosion animation.
    void onCollisionEvent(const Pulsar::D2::CollisionEvent &event) override;

    /// Draw the missile using its sprite, or the current explosion animation sprite.
    void draw(Pulsar::Graphics &graphics) const override;

    /// Unique tag to distinguish enemy missiles from other object types in collisions.
    static constexpr size_t TAG = 2;
    /// Width of an enemy missile in game coordinates.
    static constexpr float WIDTH = 0.02;
    /// Height of an enemy missile in game coordinates.
    static constexpr float HEIGHT = 0.065;

private:

    Pulsar::D2::Sprite sprite;
};

#endif
