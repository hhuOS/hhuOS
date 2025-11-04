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
 * The particle demo is based on a bachelor's thesis, written by Abdulbasir Gümüs.
 * The original source code can be found here: https://git.hhu.de/bsinfo/thesis/ba-abgue101
 */

#ifndef HHUOS_PARTICLES_GROUND_H
#define HHUOS_PARTICLES_GROUND_H

#include <stdint.h>

#include "lib/util/pulsar/2d/Entity.h"
#include "lib/util/pulsar/2d/Sprite.h"

namespace Util {
namespace Math {
template <typename T> class Vector2;
}  // namespace Math
}  // namespace Util

class Ground : public Util::Pulsar::D2::Entity {

public:
    /**
     * Default Constructor.
     */
    explicit Ground(const Util::Math::Vector2<double> &position);

    /**
     * Copy Constructor.
     */
    Ground(const Ground &other) = delete;

    /**
     * Assignment operator.
     */
    Ground &operator=(const Ground &other) = delete;

    /**
     * Destructor.
     */
    ~Ground() override = default;

    void initialize() override;

    void onUpdate(double delta) override;

    void onTranslationEvent(Util::Pulsar::D2::TranslationEvent &event) override;

    void onCollisionEvent(Util::Pulsar::D2::CollisionEvent &event) override;

    void draw(Util::Pulsar::Graphics &graphics) const override;

    static const constexpr uint32_t TAG = 6;
    static const constexpr double WIDTH = 1.0;
    static const constexpr double HEIGHT = 0.1;

private:

    Util::Pulsar::D2::Sprite sprite;
};


#endif
