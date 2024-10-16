/*
 * Copyright (C) 2018-2024 Heinrich-Heine-Universitaet Duesseldorf,
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
 *
 * The APIC implementation is based on a bachelor's thesis, written by Christoph Urlacher.
 * The original source code can be found here: https://github.com/ChUrl/hhuOS
 */

#include "LocalApicErrorHandler.h"

#include "kernel/service/InterruptService.h"
#include "kernel/interrupt/InterruptVector.h"
#include "kernel/service/Service.h"

namespace Kernel {
struct InterruptFrame;
}  // namespace Kernel

namespace Device {

void LocalApicErrorHandler::plugin() {
    auto &interruptService = Kernel::Service::getService<Kernel::InterruptService>();
    interruptService.assignInterrupt(Kernel::InterruptVector::ERROR, *this);
}
void LocalApicErrorHandler::trigger([[maybe_unused]] const Kernel::InterruptFrame &frame, [[maybe_unused]] Kernel::InterruptVector slot) {}

}