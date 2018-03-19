/**
 * IntDispatcher - responsible for registering and dispatching interrupts to the
 * corresponding handlers.
 *
 * @author Michael Schoettner, Filip Krakowski, Fabian Ruhland, Burak Akguel, Christian Gesse
 * @date HHU, 2018
 */

#ifndef __IntDispatcher_include__
#define __IntDispatcher_include__

#include "kernel/interrupts/ISR.h"
#include "kernel/KernelService.h"

#include <cstdint>

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
    IntDispatcher() = delete;

    IntDispatcher(const IntDispatcher &other) = delete;
    /**
     * Register an interrupt handler to an interrupt number.
     *
     * @param slot Interrupt number for this handler
     * @param gate Pointer to the handler itself
     */
    static void assign(uint8_t slot, ISR &gate);
    
    /**
     * Get the interrutp handler that is registered for an interrupt number
     * under the given device number.
     *
     * @param slot Interrupt number
     * @param Device number
     * @return Pointer to the requested interrupt handler.
     */
    static ISR* report(uint8_t slot, uint8_t device);

private:

    const static uint8_t MAP_SIZE_X = 64;
    const static uint8_t MAP_SIZE_Y = 16;

    // Interrupt handler map - for each interrupt number up
    // to MAX_SIZE_Y devices can be registered as handlers
    static ISR* map[MAP_SIZE_X][MAP_SIZE_Y];
};

#endif
