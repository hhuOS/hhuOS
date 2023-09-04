/*
 * Copyright (C) 2018-2023 Heinrich-Heine-Universitaet Duesseldorf,
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
 * Battle Space has been implemented during a bachelor's thesis by Richard Josef Schweitzer
 * The original source code can be found here: https://git.hhu.de/risch114/bachelorarbeit
 */

#ifndef HHUOS_MISSILE_H
#define HHUOS_MISSILE_H

#include "lib/util/game/3d/Model.h"

class Missile : public Util::Game::D3::Model {

public:
    /**
     * Constructor.
     */
    Missile(const Util::Math::Vector3D &translation, const Util::Math::Vector3D &rotation, double scale, const Util::Graphic::Color &color);

    /**
     * Copy Constructor.
     */
    Missile(const Missile &other) = delete;

    /**
     * Assignment operator.
     */
    Missile &operator=(const Missile &other) = delete;

    /**
     * Destructor.
     */
    ~Missile() override = default;

    void onUpdate(double delta) override;

    void onCollisionEvent(Util::Game::D3::CollisionEvent &event) override;

    static const constexpr uint32_t TAG = 2;

    static const constexpr double START_SPEED = 0.04;
    static const constexpr double FULL_SPEED = 0.2;
    static const constexpr double START_SPEED_TIME = 0.5;

private:

    double lifetime = 0;
};

#endif
