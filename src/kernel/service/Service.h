/*
 * Copyright (C) 2017-2026 Heinrich Heine University Düsseldorf,
 * Institute of Computer Science, Department Operating Systems
 * Main developers: Christian Gesse <christian.gesse@hhu.de>, Fabian Ruhland <ruhland@hhu.de>
 * Original development team: Burak Akguel, Christian Gesse, Fabian Ruhland, Filip Krakowski, Michael Schöttner
 * This project has been supported by several students.
 * A full list of integrated student theses can be found here: https://github.com/hhuOS/hhuOS/wiki/Student-theses
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

#ifndef __KernelService_include__
#define __KernelService_include__

#include <stdint.h>

#include "lib/util/base/Panic.h"

namespace Util {
namespace Async {
class Spinlock;
}  // namespace Async
}  // namespace Util

namespace Kernel {

/**
 * KernelService - Base class for all Kernel services.
 *
 * @author Michael Schoettner, Filip Krakowski, Christian Gesse, Fabian Ruhland, Burak Akguel
 * @date HHU, 2018
 */
class Service {

public:
    /**
     * Default Constructor.
     */
    Service() = default;

    /**
     * Copy Constructor.
     */
    Service(const Service &copy) = delete;

    /**
     * Assignment operator.
     */
    Service& operator=(const Service &other) = delete;

    /**
     * Destructor.
     */
    virtual ~Service() = default;

    static bool isServiceRegistered(uint32_t serviceId);

    static void registerService(uint32_t serviceId, Service *kernelService);

    template<class T>
    static T& getService() {
        if (!isServiceRegistered(T::SERVICE_ID)) {
            Util::Panic::fire(Util::Panic::INVALID_ARGUMENT, "Invalid service!");
        }

        return *reinterpret_cast<T*>(services[T::SERVICE_ID]);
    }

private:

    static Service* services[256];
    static Util::Async::Spinlock lock;
};

}

#endif
