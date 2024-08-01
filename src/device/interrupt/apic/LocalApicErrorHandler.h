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

#ifndef HHUOS_LOCALAPICERRORHANDLER_H
#define HHUOS_LOCALAPICERRORHANDLER_H

#include <stdint.h>

#include "kernel/interrupt/InterruptHandler.h"

namespace Kernel {
enum InterruptVector : uint8_t;
struct InterruptFrame;
}  // namespace Kernel

namespace Device {

class LocalApicErrorHandler : public Kernel::InterruptHandler {

public:
    /**
     * Default Constructor.
     */
    LocalApicErrorHandler() = default;

    /**
     * Copy Constructor.
     */
    LocalApicErrorHandler(const LocalApicErrorHandler &other) = delete;

    /**
     * Assignment operator.
     */
    LocalApicErrorHandler &operator=(const LocalApicErrorHandler &other) = delete;

    /**
     * Destructor.
     */
    ~LocalApicErrorHandler() override = default;

    /**
     * Overriding function from InterruptHandler.
     */
    void plugin() override;

    /**
     * Overriding function from InterruptHandler.
     */
    void trigger(const Kernel::InterruptFrame &frame, Kernel::InterruptVector slot) override;
};

}

#endif
