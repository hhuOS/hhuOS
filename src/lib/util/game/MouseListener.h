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

#ifndef HHUOS_MOUSELISTENER_H
#define HHUOS_MOUSELISTENER_H

#include <stdint.h>
#include "lib/util/math/Vector2.h"
#include "lib/util/io/key/MouseDecoder.h"

namespace Util::Game {

class MouseListener {

public:
    /**
     * Default Constructor.
     */
    MouseListener() = default;

    /**
     * Copy Constructor.
     */
    MouseListener(const MouseListener &other) = delete;

    /**
     * Assignment operator.
     */
    MouseListener &operator=(const MouseListener &other) = delete;

    /**
     * Destructor.
     */
    ~MouseListener() = default;

    virtual void buttonPressed(Io::MouseDecoder::Button button) = 0;

    virtual void buttonReleased(Io::MouseDecoder::Button button) = 0;

    virtual void mouseMoved(const Math::Vector2<double> &relativeMovement) = 0;

    virtual void mouseScrolled(Io::MouseDecoder::ScrollDirection direction) = 0;
};

}

#endif