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
 */

#ifndef HHUOS_INTERRUPTSERVICE_H
#define HHUOS_INTERRUPTSERVICE_H

#include <cstdint>

#include "device/interrupt/pic/Pic.h"
#include "kernel/interrupt/InterruptDispatcher.h"
#include "kernel/service/Service.h"
#include "device/port/serial/SerialPort.h"
#include "kernel/interrupt/InterruptDescriptorTable.h"

namespace Device {
class Apic;
enum InterruptRequest : uint8_t;
}  // namespace Device

namespace Kernel {
class InterruptHandler;
struct InterruptFrameOld;
class Logger;
enum InterruptVector : uint8_t;

class InterruptService : public Service {

public:
    /**
     * Constructor for use with classic PIC.
     */
    explicit InterruptService(Device::Pic *pic);

    /**
     * Constructor for use with modern APIC.
     */
    explicit InterruptService(Device::Apic *apic);

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

#pragma GCC push_options
#pragma GCC target("general-regs-only")

    [[noreturn]] void handleException(const InterruptFrame &frame, uint32_t errorCode, InterruptVector vector);

    void dispatchInterrupt(const InterruptFrame &frame, InterruptVector slot);

#pragma GCC pop_options

    void allowHardwareInterrupt(Device::InterruptRequest interrupt);

    void forbidHardwareInterrupt(Device::InterruptRequest interrupt);

    bool status(Device::InterruptRequest interrupt);

    uint16_t getInterruptMask();

    void setInterruptMask(uint16_t mask);

    void sendEndOfInterrupt(InterruptVector interrupt);

    [[nodiscard]] bool checkSpuriousInterrupt(InterruptVector interrupt);

    [[nodiscard]] Device::Apic& getApic();

    [[nodiscard]] uint8_t getCpuId() const;

    [[nodiscard]] bool isParallelComputingAllowed() const;

    void allowParallelComputing();

    static const constexpr uint8_t SERVICE_ID = 1;

private:

    Device::Pic *pic = nullptr;
    Device::Apic *apic = nullptr;

    InterruptDispatcher dispatcher;
    InterruptDescriptorTable idt;

    bool parallelComputingAllowed = false;
};

}

#endif
