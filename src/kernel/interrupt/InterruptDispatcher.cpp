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

#include "kernel/service/InterruptService.h"
#include "kernel/system/System.h"
#include "device/cpu/Cpu.h"
#include "lib/util/collection/ArrayList.h"
#include "kernel/service/ProcessService.h"
#include "kernel/interrupt/InterruptDispatcher.h"
#include "kernel/interrupt/InterruptHandler.h"
#include "kernel/process/Process.h"
#include "kernel/process/ThreadState.h"
#include "lib/util/base/Exception.h"
#include "lib/util/collection/Array.h"
#include "lib/util/collection/Collection.h"
#include "lib/util/collection/Iterator.h"
#include "lib/util/collection/List.h"
#include "lib/util/io/stream/PrintStream.h"
#include "lib/util/base/System.h"

namespace Kernel {

InterruptDispatcher::InterruptDispatcher() : handler(new Util::List<InterruptHandler*>*[256]{}) {}

void InterruptDispatcher::dispatch(const InterruptFrame &frame) {
    auto &interruptService = System::getService<InterruptService>();
    auto slot = static_cast<Interrupt>(frame.interrupt);

    // Handle exceptions (except page fault and device not available)
    if (isUnrecoverableException(slot)) {
        auto &processService = System::getService<ProcessService>();
        if (processService.getCurrentProcess().isKernelProcess()) {
            System::panic(frame);
        }

        Util::System::out << Device::Cpu::getExceptionName(slot) << ": " << Util::System::errorMessage << Util::Io::PrintStream::endl << Util::Io::PrintStream::flush;
        processService.exitCurrentProcess(-1);
    }

    // Ignore spurious interrupts
    if (interruptService.checkSpuriousInterrupt(slot)) {
        spuriousCounterWrapper.inc();
        return;
    }

    // Throw exception interrupt, if there is no handler registered
    auto *handlerList = handler[slot];
    if (handlerList == nullptr) {
        Util::Exception::throwException(Util::Exception::ILLEGAL_STATE, "No handler registered!");
    }

    interruptDepthWrapper.inc();
    interruptService.sendEndOfInterrupt(slot);
    asm volatile("sti");

    uint32_t size = handlerList->size();
    for (uint32_t i = 0; i < size; i++) {
        handlerList->get(i)->trigger(frame);
    }

    asm volatile("cli");
    interruptDepthWrapper.dec();
}

void InterruptDispatcher::assign(uint8_t slot, InterruptHandler &isr) {
    if (handler[slot] == nullptr) {
        handler[slot] = new Util::ArrayList<InterruptHandler*>;
    }

    handler[slot]->add(&isr);
}

uint32_t InterruptDispatcher::getInterruptDepth() const {
    return interruptDepth;
}

bool InterruptDispatcher::isUnrecoverableException(InterruptDispatcher::Interrupt slot) {
    // Hardware interrupts
    if (slot >= PIT && slot <= SECONDARY_ATA) {
        return false;
    }

    // Software interrupts
    if (slot == SYSTEM_CALL) {
        return false;
    }

    // Recoverable faults
    return slot != PAGEFAULT && slot != DEVICE_NOT_AVAILABLE;
}

}