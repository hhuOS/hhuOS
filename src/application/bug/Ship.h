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

#ifndef HHUOS_APPLICAITON_BUG_SHIP_H
#define HHUOS_APPLICAITON_BUG_SHIP_H

#include <stddef.h>

#include "Explosive.h"

#include <pulsar/2d/Sprite.h>

/// The ship that is controlled by the player.
/// It can move left and right and fire missiles upwards at the enemy bugs.
/// It has three lives and loses one every time it is hit.
/// Once it all lives are gone, it explodes and the game is lost.
class Ship : public Explosive {

public:
    /// Create a new player ship instance at the given position.
    explicit Ship(const Util::Math::Vector2<float> &position);

    /// Initialize the ship instance, loading its sprite and explosion animation sprites.
    void initialize() override;

    /// Check if the player still has lives left. If not, explode.
    /// If the explosion animation has finished, show the game over screen.
    void onUpdate(float delta) override;

    /// Handle translation events.
    /// If the ship leaves the screen on the left or right boundary, cancel the event.
    void onTranslationEvent(Pulsar::D2::TranslationEvent &event) override;

    /// Handle a collision with another entity.
    /// The player ship can collide with an enemy missile, in which case the player loses a life,
    /// or with an enemy bug, in which case the game is instantly lost.
    void onCollisionEvent(const Pulsar::D2::CollisionEvent &event) override;

    /// Draw the ship using its sprite, or the current explosion animation sprite.
    void draw(Pulsar::Graphics &graphics) const override;

    /// Fire an upward-facing player missile.
    /// As long as this missile is on screen, the player may not fire any further missiles.
    void fireMissile();

    /// Allow the player to fire a missile.
    /// This method is called by a player missile, once it is destroyed or leaves the screen.
    void allowFireMissile();

    /// Unique tag to distinguish the player ship from other object types in collisions.
    static constexpr size_t TAG = 0;
    /// Width of the player ship in game coordinates.
    static constexpr float WIDTH = 0.2828;
    /// Height of the player ship in game coordinates.
    static constexpr float HEIGHT = 0.2;

private:

    Pulsar::D2::Sprite sprite;
    Pulsar::D2::Sprite heart;

    size_t lives = 3;
    bool mayFireMissile = true;
};

#endif
