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

#ifndef HHUOS_APPLICATION_DEMO_GROUND_H
#define HHUOS_APPLICATION_DEMO_GROUND_H

#include <stddef.h>

#include <util/math/Vector2.h>
#include <pulsar/2d/Entity.h>
#include <pulsar/2d/Sprite.h>

/// The ground on which the raindrop particles fall.
class Ground : public Pulsar::D2::Entity {

public:
    /// Create a new ground instance at the given position.
    explicit Ground(const Util::Math::Vector2<float> &position);

    /// Initialize the ground instance by loading its sprite.
    void initialize() override;

    /// Draw the ground object using its sprite.
    void draw(Pulsar::Graphics &graphics) const override;

    /// Unique tag to distinguish the ground from other object types in collision events.
    static constexpr size_t TAG = 6;
    /// The width of the ground in game coordinates.
    static constexpr float WIDTH = 1.0;
    /// The height of the ground in game coordinates.
    static constexpr float HEIGHT = 0.1;

private:

    Pulsar::D2::Sprite sprite;
};


#endif
