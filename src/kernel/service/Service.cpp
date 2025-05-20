/*
 * Copyright (C) 2017-2025 Heinrich Heine University Düsseldorf,
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

#include "Service.h"

#include "lib/util/base/Panic.h"
#include "lib/util/async/Spinlock.h"

namespace Kernel {

Service *Service::services[256]{};
Util::Async::Spinlock Service::lock;

bool Service::isServiceRegistered(uint32_t serviceId) {
    return services[serviceId] != nullptr;
}

void Service::registerService(uint32_t serviceId, Service *kernelService) {
    lock.acquire();
    if (isServiceRegistered(serviceId)) {
        Util::Panic::fire(Util::Panic::INVALID_ARGUMENT, "Service is already registered!");
    }

    services[serviceId] = kernelService;
    lock.release();
}

}