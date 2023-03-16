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

#ifndef __InterruptDispatcher_include__
#define __InterruptDispatcher_include__

#include <cstdint>

#include "lib/util/async/Atomic.h"
#include "InterruptVector.h"

namespace Util {

template <typename T> class List;

}  // namespace Util

namespace Kernel {
class InterruptHandler;
struct InterruptFrame;

/**
 * InterruptDispatcher - responsible for registering and dispatching interrupts to the
 * corresponding handlers.
 *
 * @author Michael Schoettner, Filip Krakowski, Fabian Ruhland, Burak Akguel, Christian Gesse
 * @date HHU, 2018
 */
class InterruptDispatcher {

public:

    /**
     * Default Constructor.
     */
    InterruptDispatcher() = default;

    InterruptDispatcher(const InterruptDispatcher &other) = delete;

    InterruptDispatcher& operator=(const InterruptDispatcher &other) = delete;

    ~InterruptDispatcher() = default;

    /**
     * Register an interrupt handler to an interrupt number.
     *
     * @param slot Interrupt number for this handler
     * @param isr Pointer to the handler itself
     */
    void assign(uint8_t slot, InterruptHandler &isr);

    /**
     * Dispatched the interrupt to all registered interrupt handlers.
     *
     * @param frame The interrupt frame
     */
    void dispatch(const InterruptFrame &frame);

private:

    static bool isUnrecoverableException(Kernel::InterruptVector slot);

    Util::List<InterruptHandler*>* handler[256];

};

}

#endif