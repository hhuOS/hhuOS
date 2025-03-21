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
 *
 * The rogue game has been implemented during a bachelor's thesis by Kevin Weber
 * The original source code can be found here: https://git.hhu.de/bsinfo/thesis/ba-keweb100
 */

#ifndef HHUOS_ROGUE3D_ITEM_H
#define HHUOS_ROGUE3D_ITEM_H

#include <stdint.h>

#include "lib/util/collection/Array.h"
#include "lib/util/game/3d/Entity.h"
#include "lib/util/math/Vector3.h"

namespace Util::Game {
class Graphics;
namespace D3 {
class CollisionEvent;
}  // namespace D3
} // namespace Util::Game

class Item : public Util::Game::D3::Entity {

public:
    /**
     * Constructor.
     */
    Item(const Util::Math::Vector3<double> &position, uint32_t tag);

    /**
     * Copy Constructor.
     */
    Item(const Item &other) = delete;

    /**
     * Assignment operator.
     */
    Item &operator=(const Item &other) = delete;

    /**
     * Destructor.
     */
    ~Item() override = default;

    void initialize() override;

    void onUpdate(double delta) override;

    void draw(Util::Game::Graphics &graphics) override;

    void onCollisionEvent(Util::Game::D3::CollisionEvent &event) override;

    static const constexpr uint32_t TAG_DMG_UP = 4;
    static const constexpr uint32_t TAG_HEALTH_UP = 5;
    static const constexpr uint32_t TAG_NEXT_LVL = 100;

private:

    Util::Array<Util::Math::Vector3<double>> heart = {
        Util::Math::Vector3<double>(0.0f, 0.0f, 0.0f),
        Util::Math::Vector3<double>(-0.5f, 1.0f, 0.0f),
        Util::Math::Vector3<double>(0.5f, 1.0f, 0.0f),

        Util::Math::Vector3<double>(0.0f, 1.0f, 0.0f),
        Util::Math::Vector3<double>(-0.5f, 1.0f, 0.0f),
        Util::Math::Vector3<double>(-0.25, 1.33f, 0.0f),

        Util::Math::Vector3<double>(0.0f, 1.0f, 0.0f),
        Util::Math::Vector3<double>(0.50f, 1.0f, 0.0f),
        Util::Math::Vector3<double>(0.25, 1.33f, 0.0f)
    };

    Util::Array<Util::Math::Vector3<double>> dmgUp = {
        Util::Math::Vector3<double>(-0.5f, 0.0f, 0.0f),
        Util::Math::Vector3<double>(-0.5f, 1.0f, 0.0f),
        Util::Math::Vector3<double>(0.5f, 1.0f, 0.0f),

        Util::Math::Vector3<double>(0.5f, 0.0f, 0.0f),
        Util::Math::Vector3<double>(0.5f, 1.0f, 0.0f),
        Util::Math::Vector3<double>(-0.5f, 1.0f, 0.0f),

        Util::Math::Vector3<double>(0.5f, 1.0f, 0.0f),
        Util::Math::Vector3<double>(0.0f, 1.5f, 0.0f),
        Util::Math::Vector3<double>(-0.5f, 1.0f, 0.0f)
    };
};

#endif