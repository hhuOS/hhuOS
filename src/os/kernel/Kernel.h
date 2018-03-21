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
 * Kernel - provides global access to kernel services so that they are available
 * to all applications.
 *
 * @author Michael Schoettner, Filip Krakowski, Christian Gesse, Fabian Ruhland, Burak Akguel
 * @date HHU, 2018
 */
class Kernel {

public:

    Kernel() = delete;

    /**
     * Returns an already registered service.
     *
     * @tparam T The service's type
     * @return The service
     */
    template<class T>
    static T* getService() {

        if (!isServiceRegistered(T::SERVICE_NAME)) {

            Cpu::throwException(Cpu::Exception::INVALID_ARGUMENT);
        }

        return (T*) serviceMap.get(T::SERVICE_NAME);
    }

    /**
	 * Registers an instance of a kernel service under a given ID.
	 *
	 * @param serviceId String conatining a unique service ID.
	 * @param kernelService Instance of the kernel service
	 */
    static void registerService(const String &serviceId, KernelService* const &kernelService);

    /**
     * Indicates whether a particular service has already been registered.
     *
     * @param serviceId The service's id
     * @return true, if the service has already been registered, false else
     */
    static bool isServiceRegistered(const String &serviceId);

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
