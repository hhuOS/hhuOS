/*
 * Copyright (C) 2018-2025 Heinrich-Heine-Universitaet Duesseldorf,
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
 * The HPET driver is based on a bachelor's thesis, written by Suratsch Hassan.
 * The original source code can be found here: https://git.hhu.de/bsinfo/thesis/ba-suhas102
 */

#ifndef HHUOS_PERIODICTIMER_H
#define HHUOS_PERIODICTIMER_H

#include <stdint.h>

#include "device/interrupt/InterruptRequest.h"
#include "kernel/interrupt/InterruptHandler.h"
#include "lib/util/collection/Array.h"

namespace Kernel {
enum GlobalSystemInterrupt : uint32_t;
enum InterruptVector : uint8_t;
struct InterruptFrame;
}  // namespace Kernel
namespace Util {
namespace Async {
class Runnable;
}  // namespace Async
}  // namespace Util

namespace Device {

class Hpet;

class Timer : public Kernel::InterruptHandler {

public:
    /**
     * Constructor.
     */
    Timer(Hpet &hpet, uint8_t id, Device::InterruptRequest interrupt);

    /**
     * Copy Constructor.
     */
    Timer(const Timer &other) = delete;

    /**
     * Assignment operator.
     */
    Timer &operator=(const Timer &other) = delete;

    /**
     * Destructor.
     */
    ~Timer() override = default;

    static Util::Array<InterruptRequest> getValidInterruptLines(Hpet &hpet, uint8_t id);

    void arm(uint64_t comparatorValue, Util::Async::Runnable &onInterrupt);

    /**
     * Overriding function from InterruptHandler.
     */
    void plugin() override;

    /**
     * Overriding function from InterruptHandler.
     */
    void trigger(const Kernel::InterruptFrame &frame, Kernel::InterruptVector slot) override;

private:

    enum Capability {
        INTERRUPT_LEVEL_TRIGGERED = 1 << 1,
        INTERRUPT_ENABLE = 1 << 2,
        PERIODIC_MODE_ENABLE = 1 << 3,
        PERIODIC_CAPABLE = 1 << 4,
        WIDTH = 1 << 5,
        VALUE_SET = 1 << 6,
        FORCE_32_BIT = 1 << 7,
        FSB_INTERRUPT_ENABLE = 1 << 8,
        FSB_INTERRUPT_CAPABLE = 1 << 9,
    };

    [[nodiscard]] uint16_t getRegisterOffset(uint16_t offset) const;

    Hpet &hpet;
    uint8_t id;
    Kernel::GlobalSystemInterrupt interrupt;
    Util::Async::Runnable *onInterrupt;

    uint32_t intervalCounter = 0;
};

}

#endif
