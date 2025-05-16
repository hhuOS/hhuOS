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
 * The particle system is based on a bachelor's thesis, written by Abdulbasir Gümüs.
 * The original source code can be found here: https://git.hhu.de/bsinfo/thesis/ba-abgue101
 */

#ifndef HHUOS_SINGLETIMEEMITTER_H
#define HHUOS_SINGLETIMEEMITTER_H

#include <stdint.h>

#include "Emitter.h"

namespace Util {
namespace Math {
template <typename T> class Vector2;
}  // namespace Math
}  // namespace Util

namespace Util::Game::D2 {

class SingleTimeEmitter : public Emitter {

public:
    /**
     * Default Constructor.
     */
    SingleTimeEmitter(uint32_t tag, uint32_t particleTag, const Util::Math::Vector2<double> &position);

    /**
     * Copy Constructor.
     */
    SingleTimeEmitter(const SingleTimeEmitter &other) = delete;

    /**
     * Assignment operator.
     */
    SingleTimeEmitter &operator=(const SingleTimeEmitter &other) = delete;

    /**
     * Destructor.
     */
    ~SingleTimeEmitter() override = default;

    void initialize() override;

    void onUpdate(double delta) override;

private:

    bool emitted = false;
};

}

#endif
