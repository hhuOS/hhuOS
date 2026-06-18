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

#ifndef HHUOS_APPLICATION_BUG_PLAYERMISSILE_H
#define HHUOS_APPLICATION_BUG_PLAYERMISSILE_H

#include <stddef.h>

#include "Ship.h"

#include <pulsar/2d/Entity.h>
#include <pulsar/2d/Sprite.h>

/// A missile that was fired by the player.
/// It moves upwards from the position it was shot.
/// It can collide with enemy bugs and missiles and destroy them.
/// There can only exist one player missile at a time.
/// The player can fire the next missile once the current missile collides or leaves the screen.
/// In contrast to the `EnemyMissile` player missiles do not show an explosion animation.
/// This would look bad, since enemy missile and enemy bugs already play such an animation,
/// once they are hit by a player missile.
class PlayerMissile : public Pulsar::D2::Entity {

public:
    /// Create a new player missile object at the given position.
    /// The `Ship` reference is needed to allow the player to fire the next missile once this one is removed.
    PlayerMissile(const Util::Math::Vector2<float> &position, Ship &ship);

    /// Initialize the player missile instance, loading its sprite.
    void initialize() override;

    /// Handle translation events.
    /// Check if the new y-coordinate is larger than 1.0 (i.e., the missile leaves the screen).
    /// If so, remove the missile and allow the player to fire the next one.
    void onTranslationEvent(Pulsar::D2::TranslationEvent &event) override;

    /// Handle a collision with another entity.
    /// Player missiles can collide with enemy bugs and enemy missiles.
    /// In both cases, the player missile is removed and the player can fire the next one.
    void onCollisionEvent(const Pulsar::D2::CollisionEvent &event) override;

    /// Draw the missile using its sprite.
    void draw(Pulsar::Graphics &graphics) const override;

    /// Unique tag to distinguish player missiles from other object types in collisions.
    static constexpr size_t TAG = 1;
    /// Width of a player missile in game coordinates.
    static constexpr float WIDTH = 0.03;
    /// Height of a player missile in game coordinates.
    static constexpr float HEIGHT = 0.065;

private:

    Pulsar::D2::Sprite sprite;
    Ship &ship;
};

#endif
