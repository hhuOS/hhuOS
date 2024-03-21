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

#include "InterruptService.h"

#include "device/interrupt/InterruptRequest.h"
#include "device/interrupt/apic/Apic.h"
#include "kernel/interrupt/InterruptVector.h"
#include "kernel/log/Log.h"
#include "device/interrupt/apic/LocalApic.h"
#include "kernel/service/ProcessService.h"
#include "lib/util/base/System.h"

namespace Kernel {

class InterruptHandler;
struct InterruptFrameOld;

InterruptService::InterruptService(Device::Pic *pic) : pic(pic) {
    idt.load();
}

void InterruptService::useApic(Device::Apic *apic) {
    InterruptService::apic = apic;
}

bool InterruptService::usesApic() const {
    return apic != nullptr;
}

InterruptService::~InterruptService() {
    delete pic;
    delete apic;
}

void InterruptService::assignInterrupt(InterruptVector slot, InterruptHandler &handler) {
    dispatcher.assign(slot, handler);
}

void InterruptService::handleException(const InterruptFrame &frame, uint32_t errorCode, InterruptVector vector) {
    auto &processService = Service::getService<ProcessService>();
    if (processService.getCurrentProcess().isKernelProcess()) {
        Device::Cpu::disableInterrupts();
        Util::Exception::throwException(static_cast<Util::Exception::Error>(vector), "CPU exception!");
        Device::Cpu::halt();
    }

    Util::System::out << Util::Exception::getExceptionName(static_cast<Util::Exception::Error>(vector)) << ": CPU exception!" << Util::Io::PrintStream::endl << Util::Io::PrintStream::flush;
    processService.exitCurrentProcess(-1);
}

void InterruptService::dispatchInterrupt(const InterruptFrame &frame, InterruptVector slot) {
    dispatcher.dispatch(frame, slot);
}

void InterruptService::allowHardwareInterrupt(Device::InterruptRequest interrupt) {
    if (usesApic()) {
        apic->allow(interrupt);
    } else if (interrupt - 32 <= Device::InterruptRequest::SECONDARY_ATA) {
        pic->allow(interrupt);
    }
}

void InterruptService::forbidHardwareInterrupt(Device::InterruptRequest interrupt) {
    if (usesApic()) {
        apic->forbid(interrupt);
    } else if (interrupt - 32 <= Device::InterruptRequest::SECONDARY_ATA) {
        pic->forbid(interrupt);
    }
}

void InterruptService::sendEndOfInterrupt(InterruptVector interrupt) {
    if (usesApic()) {
        apic->sendEndOfInterrupt(interrupt);
    } else if (interrupt - 32 <= Device::InterruptRequest::SECONDARY_ATA) {
        pic->sendEndOfInterrupt(static_cast<Device::InterruptRequest>(interrupt - 32));
    }
}

bool InterruptService::checkSpuriousInterrupt(InterruptVector interrupt) {
    if (usesApic()) {
        return interrupt == InterruptVector::SPURIOUS;
    }

    if (interrupt != InterruptVector::LPT1 && interrupt != InterruptVector::SECONDARY_ATA) {
        return false;
    }

    return pic->isSpurious(static_cast<Device::InterruptRequest>(interrupt - 32));

}

Device::Apic &InterruptService::getApic() {
    return *apic;
}

bool InterruptService::status(Device::InterruptRequest interrupt) {
    return !(usesApic() ? apic->status(interrupt) : pic->status(interrupt));
}

uint16_t InterruptService::getInterruptMask() {
    uint16_t mask = 0;

    for (uint32_t i = Device::InterruptRequest::PIT; i <= Device::InterruptRequest::SECONDARY_ATA; i++) {
        auto interruptStatus = status(static_cast<Device::InterruptRequest>(i));
        mask |= (interruptStatus ? 1 : 0) << i;
    }

    return mask;
}

void InterruptService::setInterruptMask(uint16_t mask) {
    for (uint32_t i = Device::InterruptRequest::PIT; i <= Device::InterruptRequest::SECONDARY_ATA; i++) {
        auto interruptStatus = ((mask >> i) & 0x0001) == 0x0001;
        if (interruptStatus) {
            allowHardwareInterrupt(static_cast<Device::InterruptRequest>(i));
        } else {
            forbidHardwareInterrupt(static_cast<Device::InterruptRequest>(i));
        }
    }
}

uint8_t InterruptService::getCpuId() const {
    return usesApic() ? Device::LocalApic::getId() : 0;
}

bool InterruptService::isParallelComputingAllowed() const {
    return parallelComputingAllowed;
}

void InterruptService::allowParallelComputing() {
    InterruptService::parallelComputingAllowed = true;
}

}