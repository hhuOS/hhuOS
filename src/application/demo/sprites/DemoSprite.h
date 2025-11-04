/*
 * Copyright (C) 2017-2025 Heinrich Heine University Düsseldorf,
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

#ifndef HHUOS_DEMOSPRITE_H
#define HHUOS_DEMOSPRITE_H

#include <stdint.h>

#include "lib/util/pulsar/2d/Entity.h"
#include "lib/util/pulsar/2d/SpriteAnimation.h"
#include "lib/util/math/Vector2.h"

class DemoSprite : public Util::Pulsar::D2::Entity {

public:
    /**
     * Default Constructor.
     */
    DemoSprite(const Util::Math::Vector2<double> &position, double size, double rotationSpeed, double scaleSpeed, bool flipX);

    /**
     * Copy Constructor.
     */
    DemoSprite(const DemoSprite &other) = delete;

    /**
     * Assignment operator.
     */
    DemoSprite &operator=(const DemoSprite &other) = delete;

    /**
     * Destructor.
     */
    ~DemoSprite() override = default;

    void initialize() override;

    void onUpdate(double delta) override;

    void draw(Util::Pulsar::Graphics &graphics) const override;

    void onTranslationEvent(Util::Pulsar::D2::TranslationEvent &event) override;

    void onCollisionEvent(Util::Pulsar::D2::CollisionEvent &event) override;

    static const constexpr uint32_t TAG = 0;

private:

    Util::Pulsar::D2::SpriteAnimation animation;

    Util::Math::Vector2<double> initialPosition;
    double size;
    double rotationSpeed;
    double scaleSpeed;
    bool flipX;
    bool scaleUp = true;
};


#endif
