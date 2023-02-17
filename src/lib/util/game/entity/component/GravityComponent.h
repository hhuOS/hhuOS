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
 */

#ifndef HHUOS_GRAVITYCOMPONENT_H
#define HHUOS_GRAVITYCOMPONENT_H

#include "Component.h"

namespace Util::Game {

class GravityComponent : public Component {

public:
    /**
    * Constructor.
    */
    GravityComponent(Entity &entity, double groundY, double mass = 2, double stopFactorX = 0.15, double gravityValue = -1.25);

    /**
     * Copy Constructor.
     */
    GravityComponent(const GravityComponent &other) = delete;

    /**
     * Assignment operator.
     */
    GravityComponent &operator=(const GravityComponent &other) = delete;

    /**
     * Destructor.
     */
    ~GravityComponent() = default;

protected:

    void update(double delta) override;

private:

    double groundY;
    double mass;
    double stopFactorX;
    double gravityValue;
};

}

#endif
