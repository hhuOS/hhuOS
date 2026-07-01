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
 * The particle demo is based on a bachelor's thesis, written by Abdulbasir Gümüs.
 * The original source code can be found here: https://git.hhu.de/bsinfo/thesis/ba-abgue101
 */

#ifndef HHUOS_APPLICATION_DEMO_DINO_H
#define HHUOS_APPLICATION_DEMO_DINO_H

#include <stddef.h>

#include <util/math/Vector2.h>
#include <pulsar/2d/Entity.h>
#include <pulsar/2d/SpriteAnimation.h>

/// A dinosaur sprite that does not move but plays a short idle animation.
class Dino : public Pulsar::D2::Entity {

public:
    /// Create a new dinosaur instance at the given position.
    /// The `flipX` parameter determines whether the sprite should be mirrored at the x-axis.
    explicit Dino(const Util::Math::Vector2<float> &position, bool flipX);

    /// Initialize the dinosaur by loading its animation sprites.
    void initialize() override;

    /// Update the dinosaur's animation.
    void onUpdate(float delta) override;

    /// Draw the dinosaur using its current animation frame.
    void draw(Pulsar::Graphics &graphics) const override;

    /// Unique tag to distinguish dinosaurs from other object types in collision events.
    static constexpr size_t TAG = 5;
    /// The size of a dinosaur in game coordinates.
    static constexpr float SIZE = 0.15;

private:

    bool flipX;
    Pulsar::D2::SpriteAnimation animation;
};

#endif
