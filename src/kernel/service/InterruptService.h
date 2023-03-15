/*
 * Copyright (C) 2018-2023 Heinrich-Heine-Universitaet Duesseldorf,
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

#ifndef HHUOS_INTERRUPTSERVICE_H
#define HHUOS_INTERRUPTSERVICE_H

#include <cstdint>

#include "device/interrupt/Pic.h"
#include "kernel/interrupt/InterruptDispatcher.h"
#include "kernel/service/Service.h"
#include "device/interrupt/InterruptRequest.h"
#include "kernel/interrupt/InterruptVector.h"
#include "device/interrupt/apic/Apic.h"

namespace Kernel {
class InterruptHandler;
struct InterruptFrame;

class InterruptService : public Service {

public:
    /**
     * Default Constructor.
     */
    InterruptService() = default;

    /**
     * Copy Constructor.
     */
    InterruptService(const InterruptService &other) = delete;

    /**
     * Assignment operator.
     */
    InterruptService &operator=(const InterruptService &other) = delete;

    /**
     * Destructor.
     */
    ~InterruptService() override;

    void useApic(Device::Apic *apic);

    [[nodiscard]] bool usesApic() const;

    void assignInterrupt(InterruptVector slot, InterruptHandler &handler);

    void dispatchInterrupt(const InterruptFrame &frame);

    void allowHardwareInterrupt(Device::InterruptRequest interrupt);

    void forbidHardwareInterrupt(Device::InterruptRequest interrupt);

    void sendEndOfInterrupt(InterruptVector interrupt);

    [[nodiscard]] bool checkSpuriousInterrupt(InterruptVector interrupt);

    [[nodiscard]] bool isValidApicInterrupt(InterruptVector interrupt);

    [[nodiscard]] Device::Apic& getApic();

    static const constexpr uint8_t SERVICE_ID = 1;

private:

    Device::Pic pic;
    Device::Apic *apic = nullptr;
    InterruptDispatcher dispatcher;

    static Kernel::Logger log;
};

}

#endif
