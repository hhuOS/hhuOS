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

#ifndef __SYSTEMMANAGEMENT_H__
#define __SYSTEMMANAGEMENT_H__

#include <cstdint>

#include "lib/util/Exception.h"

namespace Util {
namespace Async {
class Spinlock;
}  // namespace Async
namespace Memory {
class HeapMemoryManager;
}  // namespace Memory
}  // namespace Util

namespace Kernel {
class InterruptHandler;
class Logger;
class Service;
class SystemCall;
struct InterruptFrame;
struct TaskStateSegment;

/**
 * SystemManagement
 *
 * Is responsible for everything that has to do with address spaces and memory.
 * Keeps track of all registered address spaces and can dispatch memory requests and
 * mapping requests to the corresponding memory managers and page directories.
 *
 * @author Burak Akguel, Christian Gesse, Filip Krakowski, Fabian Ruhland, Michael Schoettner
 * @date 2018
 */
class System {

public:

    /**
     * Default Constructor.
     * Deleted, as this class has only static members.
     */
    System() = delete;

    /**
     * Copy Constructor.
     */
    System(const System &other) = delete;

    /**
     * Assignment operator.
     */
    System& operator=(const System &other) = delete;

    /**
     * Destructor.
     */
    ~System() = default;

    static void initializeSystem();

    static void* allocateEarlyMemory(uint32_t size);

    static void freeEarlyMemory(void *pointer);

    static void handleEarlyInterrupt(const InterruptFrame &frame);

    /**
     * Returns an already registered service.
     *
     * @tparam T The service's type
     * @return The service
     */
    template<class T>
    static T &getService() {
        if (!isServiceRegistered(T::SERVICE_ID)) {
            Util::Exception::throwException(Util::Exception::INVALID_ARGUMENT, "Invalid service!");
        }

        return (T&) *serviceMap[T::SERVICE_ID];
    }

    /**
	 * Registers an instance of a kernel service under a given ID.
	 *
	 * @param serviceId The unique service id.
	 * @param kernelService Instance of the KernelService
	 */
    static void registerService(uint32_t serviceId, Service *kernelService);

    /**
     * Indicates whether a particular service has already been registered.
     *
     * @param serviceId The service's id
     * @return true, if the service has already been registered, false else
     */
    static bool isServiceRegistered(uint32_t serviceId);

    /**
     * Triggers a kernel panic printing relevant information inside a bluescreen.
     *
     * @param frame The interrupt frame
     */
    static void panic(const InterruptFrame &frame);

    /**
     * Creates an entry into a given GDT (Global Descriptor Table).
     * Memory for the GDT must be allocated before.
     *
     * @param gdt Pointer to the first entry of the GDT
     * @param num Number of GDT-entry we want to set
     * @param base Base address of the segment described by the GDT-entry
     * @param limit End address of the segment described by the GDT-entry
     * @param access Access bits for segment
     * @param flags Flags for segment
     */
    static void createGlobalDescriptorTableEntry(uint16_t *gdt, uint16_t num, uint32_t base, uint32_t limit, uint8_t access, uint8_t flags);

    static void initializeGlobalDescriptorTables(uint16_t *systemGdt, uint16_t *biosGdt, uint16_t *systemGdtDescriptor, uint16_t *biosGdtDescriptor, uint16_t *physicalGdtDescriptor);

    /**
     * Checks if the system management is fully initialized.
     *
     * @return State of the SystemManagement
     */
    static bool isInitialized();

    static TaskStateSegment& getTaskStateSegment();

private:

    /**
     * Calculate the amount of usable, installed physical memory using information provided by the bootloader.
     *
     * @return Amount of usable physical memory
     */
    static uint32_t calculatePhysicalMemorySize();

    static Util::Memory::HeapMemoryManager& initializeKernelHeap();

    static bool initialized;

    static Service* serviceMap[256];
    static Util::Async::Spinlock serviceLock;

    static TaskStateSegment taskStateSegment;
    static Util::Memory::HeapMemoryManager *kernelHeapMemoryManager;
    static InterruptHandler *pagefaultHandler;
    static SystemCall systemCall;
    static Logger log; // Use only after _init() has finished!
};

}

#endif
