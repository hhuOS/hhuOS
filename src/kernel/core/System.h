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

#include "kernel/service/KernelService.h"
#include "lib/async/Spinlock.h"

#include <cstdint>
#include "lib/util/String.h"
#include "kernel/thread/ThreadState.h"
#include "lib/util/HashMap.h"

namespace Kernel {

/**
 * Provides access to registered KernelServices.
 *
 * @author Filip Krakowski
 */
class System {

public:

    System() = delete;

    /**
     * Returns an already registered service.
     *
     * @tparam T The service's type
     * @return The service
     */
    template<class T>
    static T *getService() {

        if (!isServiceRegistered(T::SERVICE_NAME)) {

            Device::Cpu::throwException(Device::Cpu::Exception::INVALID_ARGUMENT, "Invalid service!");
        }

        return (T *) serviceMap.get(T::SERVICE_NAME);
    }

    /**
	 * Registers an instance of a kernel service under a given ID.
	 *
	 * @param serviceId The unique service id.
	 * @param kernelService Instance of the KernelService
	 */
    static void registerService(const Util::String &serviceId, KernelService *const &kernelService);

    /**
     * Indicates whether a particular service has already been registered.
     *
     * @param serviceId The service's id
     * @return true, if the service has already been registered, false else
     */
    static bool isServiceRegistered(const Util::String &serviceId);

    /**
     * Triggers a kernel panic printing relevant information inside a bluescreen.
     *
     * @param frame The interrupt frame
     */
    static void panic(InterruptFrame *frame);

private:

    static Util::HashMap<Util::String, KernelService *> serviceMap;

    static Async::Spinlock serviceLock;

    static const uint32_t SERVICE_MAP_SIZE = 47;
};

}

#endif
