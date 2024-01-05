/*
 * Copyright (C) 2018-2024 Heinrich-Heine-Universitaet Duesseldorf,
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
 * The particle system is based on a bachelor's thesis, written by Abdulbasir Gümüs.
 * The original source code can be found here: https://git.hhu.de/bsinfo/thesis/ba-abgue101
 */

#ifndef HHUOS_SINGLETIMEEMITTER_H
#define HHUOS_SINGLETIMEEMITTER_H

#include "Emitter.h"

namespace Util::Game::D2 {

class SingleTimeEmitter : public Emitter {

public:
    /**
     * Default Constructor.
     */
    SingleTimeEmitter(uint32_t tag, uint32_t particleTag, const Util::Math::Vector2D &position);

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
