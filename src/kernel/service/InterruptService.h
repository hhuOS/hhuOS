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

#ifndef HHUOS_INTERRUPTSERVICE_H
#define HHUOS_INTERRUPTSERVICE_H

#include <stdarg.h>
#include <stdint.h>

#include "kernel/interrupt/InterruptDispatcher.h"
#include "kernel/service/Service.h"
#include "kernel/interrupt/InterruptDescriptorTable.h"
#include "lib/util/base/System.h"
#include "kernel/interrupt/SystemCallDispatcher.h"
#include "kernel/interrupt/GlobalSystemInterrupt.h"

namespace Device {
class Apic;
enum InterruptRequest : uint8_t;
class Pic;
}  // namespace Device

namespace Kernel {
class InterruptHandler;
enum InterruptVector : uint8_t;
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

    void loadIdt();

    void usePic(Device::Pic *pic);

    void useApic(Device::Apic *apic);

    bool usesApic() const;

    InterruptVector getTimerInterrupt() const;

    void assignInterrupt(InterruptVector slot, InterruptHandler &handler);

    void assignSystemCall(Util::System::Code code, bool(*func)(uint32_t paramCount, va_list params));

#pragma GCC push_options
#pragma GCC target("general-regs-only")

    [[noreturn]] void handleException(const InterruptFrame &frame, uint32_t errorCode, InterruptVector vector);

    void dispatchInterrupt(const InterruptFrame &frame, InterruptVector slot);

    void dispatchSystemCall(Util::System::Code code, uint16_t paramCount, va_list params, bool &result);

#pragma GCC pop_options

    void allowHardwareInterrupt(Device::InterruptRequest interrupt);

    void forbidHardwareInterrupt(Device::InterruptRequest interrupt);

    bool status(Device::InterruptRequest interrupt);

    uint16_t getInterruptMask();

    void setInterruptMask(uint16_t mask);

    Device::InterruptRequest getInterruptSource(GlobalSystemInterrupt gsi);

    GlobalSystemInterrupt getInterruptTarget(Device::InterruptRequest interrupt);

    GlobalSystemInterrupt getMaxInterruptTarget();

    void sendEndOfInterrupt(InterruptVector interrupt);

    bool checkSpuriousInterrupt(InterruptVector interrupt);

    Device::Apic& getApic();

    uint8_t getCpuId() const;

    static const constexpr uint8_t SERVICE_ID = 1;

private:

    Device::Pic *pic = nullptr;
    Device::Apic *apic = nullptr;

    InterruptDescriptorTable idt;
    InterruptDispatcher interruptDispatcher;
    SystemCallDispatcher systemCallDispatcher;

    bool parallelComputingAllowed = false;
};

}

#endif
