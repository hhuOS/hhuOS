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

#ifndef HHUOS_ENEMYMISSILE_H
#define HHUOS_ENEMYMISSILE_H

#include <stdint.h>

#include "lib/pulsar/2d/Sprite.h"
#include "Explosive.h"

class EnemyBug;
namespace Util {
namespace Math {
template <typename T> class Vector2;
}  // namespace Math
}  // namespace Util

class EnemyMissile : public Explosive {

public:
    /**
     * Constructor.
     */
    explicit EnemyMissile(const Util::Math::Vector2<float> &position, EnemyBug &bug);

    /**
     * Copy Constructor.
     */
    EnemyMissile(const EnemyMissile &other) = delete;

    /**
     * Assignment operator.
     */
    EnemyMissile &operator=(const EnemyMissile &other) = delete;

    /**
     * Destructor.
     */
    ~EnemyMissile() override = default;

    void initialize() override;

    void onUpdate(float delta) override;

    void onTranslationEvent(Pulsar::D2::TranslationEvent &event) override;

    void onCollisionEvent(const Pulsar::D2::CollisionEvent &event) override;

    void draw(Pulsar::Graphics &graphics) const override;

    static const constexpr uint32_t TAG = 2;
    static const constexpr float SIZE_X = 0.02;
    static const constexpr float SIZE_Y = 0.065;

private:

    Pulsar::D2::Sprite sprite;
    EnemyBug &bug;
};

#endif
