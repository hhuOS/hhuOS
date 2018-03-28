/*
* Copyright (C) 2018 Burak Akguel, Christian Gesse, Fabian Ruhland, Filip Krakowski, Michael Schoettner
* Heinrich-Heine University
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

#ifndef __IntDispatcher_include__
#define __IntDispatcher_include__

#include "kernel/interrupts/InterruptHandler.h"
#include "kernel/threads/ThreadState.h"
#include "kernel/KernelService.h"

#include "lib/util/ArrayList.h"
#include "lib/util/HashMap.h"

#include <cstdint>

/**
 * IntDispatcher - responsible for registering and dispatching interrupts to the
 * corresponding handlers.
 *
 * @author Michael Schoettner, Filip Krakowski, Fabian Ruhland, Burak Akguel, Christian Gesse
 * @date HHU, 2018
 */
class IntDispatcher : public KernelService {
    
public:
	// enum of important intterupt numbers
    enum {
        pagefault = 14,
        timer = 32,
        keyboard = 33,
        mouse = 44
    };
    
    // no constructor needed
    IntDispatcher();

    IntDispatcher(const IntDispatcher &other) = delete;
    /**
     * Register an interrupt handler to an interrupt number.
     *
     * @param slot Interrupt number for this handler
     * @param gate Pointer to the handler itself
     */
    void assign(uint8_t slot, InterruptHandler &gate);
    
    /**
     * Get the interrupt handlers that are registered for a specific interrupt.
     *
     * @param slot Interrupt number
     * @return Pointer to a list of all registered handlers or <em>nullptr</em> if no handlers are registered
     */
    Util::List<InterruptHandler*>* report(uint8_t slot);

    /**
     * Dispatched the interrupt to all registered interrupt handlers.
     *
     * @param frame The interrupt frame
     */
    void dispatch(InterruptFrame *frame);

    static IntDispatcher &getInstance();

private:

    Util::HashMap<uint8_t, Util::ArrayList<InterruptHandler*>*> handler;
};

#endif
