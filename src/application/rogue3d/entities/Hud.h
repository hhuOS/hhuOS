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
 *
 * The rogue game has been implemented during a bachelor's thesis by Kevin Weber
 * The original source code can be found here: https://git.hhu.de/bsinfo/thesis/ba-keweb100
 */

#ifndef HHUOS_ROGUE3D_HUD_H
#define HHUOS_ROGUE3D_HUD_H

#include <stdint.h>

#include "Room.h"
#include "lib/pulsar/3d/Entity.h"

class Player;

class Hud : public Pulsar::D3::Entity {

public:
    /**
     * Constructor.
     */
    explicit Hud(Room::Type map[4][4], const Player &player);

    /**
     * Copy Constructor.
     */
    Hud(const Hud &other) = delete;

    /**
     * Assignment operator.
     */
    Hud &operator=(const Hud &other) = delete;

    /**
     * Destructor.
     */
    ~Hud() override = default;

    void initialize() override;

    void draw(Pulsar::Graphics &graphics) const override;

    void onCollisionEvent(const Pulsar::D3::CollisionEvent &event) override;

    void onUpdate(float delta) override;

    void setCurrentPosition(uint32_t newRow,uint32_t newColumn);

    static const constexpr uint32_t TAG = 0;

private:

    const Player &player;

    uint32_t currentRow = 0;
    uint32_t currentColumn = 0;
    Room::Type map[4][4]{};
};

#endif
