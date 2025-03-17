/*
 * Copyright (C) 2018-2025 Heinrich-Heine-Universitaet Duesseldorf,
 * Institute of Computer Science, Department Operating Systems
 * Burak Akguel, Christian Gesse, Fabian Ruhland, Filip Krakowski, Michael Schoettner
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

#ifndef HHUOS_EXPLOSIVE_H
#define HHUOS_EXPLOSIVE_H

#include <stdint.h>

#include "lib/util/game/2d/SpriteAnimation.h"
#include "lib/util/game/2d/Entity.h"

namespace Util {
namespace Math {
template <typename T> class Vector2;
}  // namespace Math

namespace Game {
namespace D2 {
class RectangleCollider;
}  // namespace D2
}  // namespace Game
}  // namespace Util

class Explosive : public Util::Game::D2::Entity {

public:
    /**
     * Constructor.
     */
    Explosive(uint32_t tag, const Util::Math::Vector2<double> &position, const Util::Game::D2::RectangleCollider &collider);

    /**
     * Copy Constructor.
     */
    Explosive(const Explosive &other) = delete;

    /**
     * Assignment operator.
     */
    Explosive &operator=(const Explosive &other) = delete;

    /**
     * Destructor.
     */
    ~Explosive() override = default;

    void initialize() override;

    void onUpdate(double delta) override;

    void draw(Util::Game::Graphics &graphics) override;

    void explode();

    [[nodiscard]] bool isExploding() const;

    [[nodiscard]] bool hasExploded() const;

private:

    Util::Game::D2::SpriteAnimation animation;

    bool shouldExplode = false;
    bool exploding = false;

    double explosionTimer = 0;
};

#endif
