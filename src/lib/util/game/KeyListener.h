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
 * The game engine is based on a bachelor's thesis, written by Malte Sehmer.
 * The original source code can be found here: https://github.com/Malte2036/hhuOS
 *
 * It has been enhanced with 3D-capabilities during a bachelor's thesis by Richard Josef Schweitzer
 * The original source code can be found here: https://git.hhu.de/bsinfo/thesis/ba-risch114
 */

#ifndef HHUOS_KEYLISTENER_H
#define HHUOS_KEYLISTENER_H

#include "lib/util/io/key/Key.h"

namespace Util::Game {

class KeyListener {

public:
    /**
     * Default Constructor.
     */
    KeyListener() = default;

    /**
     * Copy Constructor.
     */
    KeyListener(const KeyListener &other) = delete;

    /**
     * Assignment operator.
     */
    KeyListener &operator=(const KeyListener &other) = delete;

    /**
     * Destructor.
     */
    ~KeyListener() = default;

    virtual void keyPressed(const Util::Io::Key &key) = 0;

    virtual void keyReleased(const Util::Io::Key &key) = 0;
};

}

#endif