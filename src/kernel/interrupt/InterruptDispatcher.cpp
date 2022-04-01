/*
 * Copyright (C) 2018-2022 Heinrich-Heine-Universitaet Duesseldorf,
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

#include "device/interrupt/Pic.h"
#include "kernel/system/System.h"
#include "device/cpu/Cpu.h"
#include "InterruptDispatcher.h"

namespace Kernel {

InterruptDispatcher &InterruptDispatcher::getInstance() noexcept {
    static InterruptDispatcher instance;
    return instance;
}

void InterruptDispatcher::dispatch(InterruptFrame *frame) {
    auto slot = static_cast<uint8_t>(frame->interrupt);

    // Throw bluescreen on exceptions except page fault
    if ((slot < 32 && slot != static_cast<uint32_t>(Device::Cpu::Error::PAGE_FAULT)) || (slot >= Util::Exception::NULL_POINTER)) {
        auto &schedulerService = System::getService<SchedulerService>();
        if (schedulerService.getCurrentProcess().isKernelProcess()) {
            System::panic(*frame);
        }

        Util::System::out << Device::Cpu::getExceptionName(frame->interrupt) << ": " << Util::System::errorMessage << Util::Stream::PrintWriter::endl << Util::Stream::PrintWriter::flush;
        schedulerService.exitCurrentProcess(-1);
    }

    // Ignore spurious interrupts
    if (slot == 39 && Device::Pic::getInstance().isSpurious()) {
        return;
    }

    if (handler.size() == 0) {
        sendEoi(slot);
        return;
    }

    Util::Data::List<InterruptHandler*> *list = getHandlerForSlot(slot);

    if (list == nullptr && slot >= 32) {
        Util::Exception::throwException(Util::Exception::ILLEGAL_STATE, "No handler registered!");
    }

    auto interruptDepthWrapper = Util::Async::Atomic<uint32_t>(interruptDepth);
    interruptDepthWrapper.inc();
    asm volatile("sti");

    if (list != nullptr) {
        uint32_t size = list->size();
        for (uint32_t i = 0; i < size; i++) {
            list->get(i)->trigger(*frame);
        }
    }

    asm volatile("cli");
    interruptDepthWrapper.dec();

    sendEoi(slot);
}

void InterruptDispatcher::assign(uint8_t slot, InterruptHandler &isr) {
    if (!handler.containsKey(slot)) {
        handler.put(slot, new Util::Data::ArrayList<InterruptHandler *>);
    }

    handler.get(slot)->add(&isr);
}

Util::Data::List<InterruptHandler*>* InterruptDispatcher::getHandlerForSlot(uint8_t slot) {
    if (!handler.containsKey(slot)) {
        return nullptr;
    }

    return handler.get(slot);
}

uint32_t InterruptDispatcher::getInterruptDepth() const {
    return interruptDepth;
}

void InterruptDispatcher::sendEoi(uint32_t slot) {
    if (slot > 32) {
        Device::Pic::getInstance().sendEOI(Device::Pic::Interrupt(slot - 32));
    }
}


}