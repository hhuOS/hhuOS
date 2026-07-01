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

#ifndef HHUOS_APPLICATION_DEMO_DEMOSPRITE_H
#define HHUOS_APPLICATION_DEMO_DEMOSPRITE_H

#include <pulsar/2d/Entity.h>
#include <pulsar/2d/SpriteAnimation.h>
#include <util/math/Vector2.h>

/// A 2D animated sprite that scales and rotates over time.
class DemoSprite : public Pulsar::D2::Entity {

public:
    /// Create a new sprite instance at the given position.
    DemoSprite(const Util::Math::Vector2<float> &position, float size, float rotSpeed, float scaleSpeed, bool flipX);

    /// Initialize the demo sprite instance by loading its animation sprites.
    void initialize() override;

    /// Rotate and scale the sprite according to the given delta time.
    void onUpdate(float delta) override;

    /// Draw the demo sprite using its current animation frame.
    void draw(Pulsar::Graphics &graphics) const override;

private:

    Pulsar::D2::SpriteAnimation animation;

    const Util::Math::Vector2<float> initialPosition;
    const float rotationSpeed;
    const float scaleSpeed;

    float size;
    bool flipX;
    bool scaleUp = true;
};


#endif
