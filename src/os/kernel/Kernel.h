/**
 * Kernel - provides global access to kernel services so that they are available
 * to all applications.
 *
 * @author Michael Schoettner, Filip Krakowski, Christian Gesse, Fabian Ruhland, Burak Akguel
 * @date HHU, 2018
 */

#ifndef __Kernel_include__
#define __Kernel_include__


#include "kernel/KernelService.h"
#include "kernel/Spinlock.h"

#include <cstdint>
#include <lib/deprecated/HashMap.h>
#include <lib/String.h>
#include <kernel/threads/ThreadState.h>
#include <lib/util/HashMap.h>

/**
 * @author Filip Krakowski
 */
class Kernel {

public:

    Kernel() = delete;

    /**
     * Returns the kernel service that is registered with a given ID.
     *
     * @param serviceId String conatining a unique service ID.
     */
    static KernelService* getService(const String &serviceId);

    /**
	 * Registers an instance of a kernel service under a given ID.
	 *
	 * @param serviceId String conatining a unique service ID.
	 * @param kernelService Instance of the kernel service
	 */
    static void registerService(const String &serviceId, KernelService* const &kernelService);

    /**
     * Triggers a kernel panic with the given interrupt frame
     * Shows sort of a bluescreen.
     *
     * @param frame Pointer to the interrupt frame
     */
    static void panic(InterruptFrame *frame);

private:
    // hasmap that alls all registered kernel services with their IDs
    static Util::HashMap<String, KernelService*> serviceMap;
    // Spinlock for the kernel service
    static Spinlock serviceLock;

    // size of the service map - is a prime due to hashing
    static const uint32_t SERVICE_MAP_SIZE = 47;

    /**
     * Prints the stack trace for a kernel panic.
     *
     * @param basePointer Value of the EBP
     * @param instructionPointer Value of the EIP
     * @param skip How many calls in stack trace are to be skipped
     */
    static void printStacktrace(uint32_t basePointer, uint32_t instructionPointer, uint32_t skip = 0);

    /**
     * Print state of registers in given interrupt frame
     *
     * @param frame The interrupt frame where values should be taken from
     */
    static void printRegisters(const InterruptFrame &frame);
};


#endif
