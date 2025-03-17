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
 */

#include "Pic.h"

#include "device/cpu/IoPort.h"
#include "device/interrupt/InterruptRequest.h"
#include "kernel/interrupt/InterruptVector.h"
#include "kernel/service/Service.h"
#include "kernel/service/TimeService.h"
#include "lib/util/time/Timestamp.h"

namespace Device {

Pic::Pic() {
    auto &timeService = Kernel::Service::getService<Kernel::TimeService>();
    auto delay = Util::Time::Timestamp::ofMicroseconds(10);

    // Start initialization sequence on both PICs (ICW1)
    masterCommandPort.writeByte(INITIALIZE);
    timeService.busyWait(delay);
    slaveCommandPort.writeByte(INITIALIZE);
    timeService.busyWait(delay);

    // Set interrupt offsets (ICW2)
    masterDataPort.writeByte(Kernel::InterruptVector::PIT);
    timeService.busyWait(delay);
    slaveDataPort.writeByte(Kernel::InterruptVector::RTC);
    timeService.busyWait(delay);

    // Setup cascading PICs (ICW3)
    masterDataPort.writeByte(0x04);
    timeService.busyWait(delay);
    slaveDataPort.writeByte(0x02);
    timeService.busyWait(delay);

    // Enable 8086-mode (ICW4)
    masterDataPort.writeByte(0x01);
    timeService.busyWait(delay);
    slaveDataPort.writeByte(0x01);
    timeService.busyWait(delay);

    // Disable all interrupt lines
    masterDataPort.writeByte(0xfb); // Allow cascading interrupts
    timeService.busyWait(delay);
    slaveDataPort.writeByte(0xff);
    timeService.busyWait(delay);
}

void Pic::allow(InterruptRequest interrupt) {
    auto &port = getDataPort(interrupt);
    uint8_t mask = getMask(interrupt);

    port.writeByte(port.readByte() & ~mask);
}

void Pic::forbid(InterruptRequest interrupt) {
    auto &port = getDataPort(interrupt);
    uint8_t mask = getMask(interrupt);

    port.writeByte(port.readByte() | mask);
}

bool Pic::status(InterruptRequest interrupt) {
    const IoPort &port = getDataPort(interrupt);
    uint8_t mask = getMask(interrupt);

    return port.readByte() & mask;
}

void Pic::sendEndOfInterrupt(InterruptRequest interrupt) {
    if (interrupt >= InterruptRequest::RTC) {
        slaveCommandPort.writeByte(END_OF_INTERRUPT);
    }

    masterCommandPort.writeByte(END_OF_INTERRUPT);
}

const IoPort &Pic::getDataPort(InterruptRequest interrupt) {
    if (interrupt >= InterruptRequest::RTC) {
        return slaveDataPort;
    }

    return masterDataPort;
}

uint8_t Pic::getMask(InterruptRequest interrupt) {
    if (interrupt >= InterruptRequest::RTC) {
        return (uint8_t) (1 << ((uint8_t) interrupt - 8));
    }

    return (uint8_t) (1 << (uint8_t) interrupt);
}

bool Pic::isSpurious(InterruptRequest interrupt) {
    if (interrupt == InterruptRequest::LPT1) {
        masterCommandPort.writeByte(READ_ISR);
        return (masterCommandPort.readByte() & SPURIOUS_INTERRUPT) == 0;
    } else if (interrupt == InterruptRequest::SECONDARY_ATA) {
        slaveCommandPort.writeByte(READ_ISR);
        if ((slaveCommandPort.readByte() & SPURIOUS_INTERRUPT) == 0) {
            sendEndOfInterrupt(InterruptRequest::CASCADE);
            return true;
        }
    }

    return false;
}

}