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

#ifndef __InterruptHandler_include__
#define __InterruptHandler_include__

#include "kernel/process/ThreadState.h"

namespace Kernel {

/**
 * Interface for an interrupt handler.
 * Every interrupt handler should derive from this interface.
 * The trigger-method is called if an interrupt occurred.
 *
 * @author Michael Schoettner, Filip Krakowski, Fabian Ruhland, Burak Akguel, Christian Gesse
 * @date HHU, 2018
 */
class InterruptHandler {

public:

    InterruptHandler() = default;

    InterruptHandler(const InterruptHandler &copy) = delete;

    InterruptHandler &operator=(const InterruptHandler &copy) = delete;

    virtual ~InterruptHandler() = default;

    /**
     * Enable interrupts for this handler.
     */
    virtual void plugin() = 0;

    /**
     * Routine to handle an interrupt. Needs to be implemented in deriving class.
     */
    virtual void trigger(const InterruptFrame &frame) = 0;
};

}

#endif
