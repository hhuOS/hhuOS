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

#ifndef HHUOS_DINO_H
#define HHUOS_DINO_H

#include <stdint.h>

#include "lib/pulsar/2d/Entity.h"
#include "lib/pulsar/2d/SpriteAnimation.h"

namespace Util {
namespace Math {
template <typename T> class Vector2;
}  // namespace Math
}  // namespace Util

class Dino : public Pulsar::D2::Entity {

public:
    /**
     * Constructor.
     */
    explicit Dino(const Util::Math::Vector2<double> &position, bool flipX);

    /**
     * Copy Constructor.
     */
    Dino(const Dino &other) = delete;

    /**
     * Assignment operator.
     */
    Dino &operator=(const Dino &other) = delete;

    /**
     * Destructor.
     */
    ~Dino() override = default;

    void initialize() override;

    void onUpdate(double delta) override;

    void onTranslationEvent(Pulsar::D2::TranslationEvent &event) override;

    void onCollisionEvent(Pulsar::D2::CollisionEvent &event) override;

    void draw(Pulsar::Graphics &graphics) const override;

    static const constexpr uint32_t TAG = 5;
    static const constexpr double SIZE = 0.15;

private:

    bool flipX;
    Pulsar::D2::SpriteAnimation animation;
};

#endif
