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
 * The rogue game has been implemented during a bachelor's thesis by Kevin Weber
 * The original source code can be found here: https://git.hhu.de/bsinfo/thesis/ba-keweb100
 */

#ifndef HHUOS_ROGUE3D_PROJECTILE_H
#define HHUOS_ROGUE3D_PROJECTILE_H

#include <stdint.h>

#include "lib/util/pulsar/3d/Entity.h"
#include "lib/util/math/Vector3.h"

class Projectile : public Util::Pulsar::D3::Entity {

public:
    /**
     * Constructor.
     */
    explicit Projectile(const Util::Math::Vector3<double> &position,const Util::Math::Vector3<double> &direction,uint32_t tag);

    /**
     * Constructor.
     */
    Projectile(const Util::Math::Vector3<double> &position, const Util::Math::Vector3<double> &direction, uint32_t tag, double range);

    /**
     * Copy Constructor.
     */
    Projectile(const Projectile &other) = delete;

    /**
     * Assignment operator.
     */
    Projectile &operator=(const Projectile &other) = delete;

    /**
     * Destructor.
     */
    ~Projectile() override = default;

    void initialize() override;

    void onUpdate(double delta) override;

    void draw(Util::Pulsar::Graphics &graphics) const override;

    void onCollisionEvent(Util::Pulsar::D3::CollisionEvent &event) override;

    static const constexpr uint32_t TAG_PLAYER = 10;
    static const constexpr uint32_t TAG_ENEMY = 11;

private:

    double range = 10;

    Util::Math::Vector3<double> direction;
    Util::Math::Vector3<double> origin;

    static uint32_t DRAW_LIST_ID;
};

#endif