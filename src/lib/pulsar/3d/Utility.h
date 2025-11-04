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
 * The game engine is based on a bachelor's thesis, written by Malte Sehmer.
 * The original source code can be found here: https://github.com/Malte2036/hhuOS
 *
 * It has been enhanced with 3D-capabilities during a bachelor's thesis by Richard Josef Schweitzer
 * The original source code can be found here: https://git.hhu.de/bsinfo/thesis/ba-risch114
 */

#ifndef HHUOS_UTILITY_H
#define HHUOS_UTILITY_H

#include "Entity.h"
#include "util/collection/ArrayList.h"
#include "lib/util/math/Vector3.h"

namespace Pulsar::D3 {

class Utility {

public:
    /**
     * Default Constructor.
     * Deleted, as this class has only static members.
     */
    Utility() = delete;

    /**
     * Copy Constructor.
     */
    Utility(const Utility &other) = delete;

    /**
     * Assignment operator.
     */
    Utility &operator=(Utility &other) = delete;

    /**
     * Destructor.
     * Deleted, as this class has only static members.
     */
    ~Utility() = delete;

    static Entity* findEntityUsingRaytrace(const Util::ArrayList<Entity*> &entities, Util::Math::Vector3<double> from, Util::Math::Vector3<double> direction, double length, double precision = 0.1);

    static Util::Math::Vector3<double> findLookAt(const Util::Math::Vector3<double> &from, const Util::Math::Vector3<double> &to);

    /**
     * Calculates an orbit location of start around orbitCenter at angle location orbitAngle.
     * orbitAngle should be angles in degrees; {0, 0, 1} is the forward Direction; roll values will be ignored.
     */
    static Util::Math::Vector3<double> findOrbitLocation(const Util::Math::Vector3<double> &start, const Util::Math::Vector3<double> &orbitCenter, const Util::Math::Vector3<double> &orbitAngle);

    /**
     * Calculates an orbit location of start around orbitCenter at angle location orbitAngle.
     * orbitAngle should be angles in degrees; {0, 0, 1} is the forward Direction; roll values will be ignored.
     * orbitAngle will be interpreted as relative to the current start angle around the orbitCenter.
     */
    static Util::Math::Vector3<double> findOrbitLocationRelative(const Util::Math::Vector3<double> &start, const Util::Math::Vector3<double> &orbitCenter, const Util::Math::Vector3<double> &orbitAngleRelative);
};

}

#endif
