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
 *
 * The dino game is based on a bachelor's thesis, written by Malte Sehmer.
 * The original source code can be found here: https://github.com/Malte2036/hhuOS
 */

#ifndef HHUOS_ENEMYFROG_H
#define HHUOS_ENEMYFROG_H

#include <stdint.h>

#include "lib/pulsar/2d/Entity.h"
#include "lib/pulsar/2d/SpriteAnimation.h"
#include "application/dino/particle/GrassEmitter.h"
#include "lib/util/time/Timestamp.h"

namespace Util {
namespace Math {
template <typename T> class Vector2;
}  // namespace Math
}  // namespace Util

class EnemyFrog : public Pulsar::D2::Entity {

public:
    /**
     * Constructor.
     */
    explicit EnemyFrog(const Util::Math::Vector2<double> &position);

    /**
     * Copy Constructor.
     */
    EnemyFrog(const EnemyFrog &other) = delete;

    /**
     * Assignment operator.
     */
    EnemyFrog &operator=(const EnemyFrog &other) = delete;

    /**
     * Destructor.
     */
    ~EnemyFrog() override = default;

    void initialize() override;

    void onUpdate(double delta) override;

    void draw(Pulsar::Graphics &graphics) const override;

    void onTranslationEvent(Pulsar::D2::TranslationEvent &event) override;

    void onCollisionEvent(Pulsar::D2::CollisionEvent &event) override;

    static const constexpr uint32_t TAG = 7;

private:

    enum Direction {
        LEFT, RIGHT
    };

    Pulsar::D2::SpriteAnimation animation;
    Direction direction = LEFT;

    GrassEmitter *grassEmitter = new GrassEmitter(*this);
    Util::Time::Timestamp lastEmissionTime;

    static const constexpr double SIZE = 0.08;
    static const constexpr double VELOCITY = 0.25;
    static const constexpr uint32_t EMISSION_INTERVAL_MS = 500;
};

#endif
