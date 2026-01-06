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

#ifndef HHUOS_ENEMYBUG_H
#define HHUOS_ENEMYBUG_H

#include <stdint.h>

#include "lib/pulsar/2d/SpriteAnimation.h"
#include "Explosive.h"

class Fleet;
namespace Util {
namespace Math {
template <typename T> class Vector2;
}  // namespace Math
}  // namespace Util

class EnemyBug : public Explosive {

public:
    /**
     * Constructor.
     */
    explicit EnemyBug(const Util::Math::Vector2<float> &position, Fleet &fleet);

    /**
     * Copy Constructor.
     */
    EnemyBug(const EnemyBug &other) = delete;

    /**
     * Assignment operator.
     */
    EnemyBug &operator=(const EnemyBug &other) = delete;

    /**
     * Destructor.
     */
    ~EnemyBug() override = default;

    void initialize() override;

    void onUpdate(float delta) override;

    void onTranslationEvent(Pulsar::D2::TranslationEvent &event) override;

    void onCollisionEvent(const Pulsar::D2::CollisionEvent &event) override;

    void draw(Pulsar::Graphics &graphics) const override;

    void fireMissile();

    static const constexpr uint32_t TAG = 3;
    static const constexpr float SIZE_X = 0.15;
    static const constexpr float SIZE_Y = 0.1;

private:

    Pulsar::D2::SpriteAnimation animation;
    Fleet &fleet;

    float lastMissileRollTime = 0;
};

#endif
