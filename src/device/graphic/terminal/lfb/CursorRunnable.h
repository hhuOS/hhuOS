/*
 * Copyright (C) 2018-2022 Heinrich-Heine-Universitaet Duesseldorf,
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

#ifndef HHUOS_CURSORRUNNABLE_H
#define HHUOS_CURSORRUNNABLE_H

#include "lib/util/async/Runnable.h"

namespace Device::Graphic {

class LinearFrameBufferTerminal;

class CursorRunnable : public Util::Async::Runnable {

public:
    /**
     * Constructor.
     */
    explicit CursorRunnable(LinearFrameBufferTerminal &terminal, char cursor);

    /**
     * Copy Constructor.
     */
    CursorRunnable(const CursorRunnable &other) = delete;

    /**
     * Assignment operator.
     */
    CursorRunnable &operator=(const CursorRunnable &other) = delete;

    /**
     * Destructor.
     */
    ~CursorRunnable() override = default;

    void run() override;

    void stop();

    void draw();

private:

    LinearFrameBufferTerminal &terminal;
    char cursor;

    bool visible = true;
    bool isRunning = true;
    bool finished = false;
};

}

#endif