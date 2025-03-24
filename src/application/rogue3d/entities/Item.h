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

#include "lib/util/game/3d/Entity.h"

namespace Util {
namespace Math {
template <typename T> class Vector3;
}  // namespace Math
template <typename T> class Array;
}  // namespace Util

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

    static Util::Array<Util::Math::Vector3<double>> HEART;
    static Util::Array<Util::Math::Vector3<double>> DMG_UP;

    static uint32_t HEALTH_LIST_ID;
    static uint32_t DMG_UP_LIST_ID;
    static uint32_t NEXT_LVL_LIST_ID;
};

#endif