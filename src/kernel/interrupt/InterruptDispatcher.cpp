/*
 * Copyright (C) 2018 Burak Akguel, Christian Gesse, Fabian Ruhland, Filip Krakowski, Michael Schoettner
 * Heinrich-Heine University
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

#include "kernel/core/System.h"
#include "kernel/core/Symbols.h"
#include "device/cpu/IoPort.h"
#include "lib/libc/printf.h"
#include "device/sound/PcSpeaker.h"
#include "lib/math/Random.h"
#include "kernel/debug/GdbServer.h"
#include "kernel/thread/Scheduler.h"
#include "device/cpu/Cpu.h"
#include "kernel/interrupt/InterruptDispatcher.h"
#include "kernel/core/Management.h"
#include "device/misc/Pic.h"
#include "lib/libc/sprintf.h"

namespace Kernel {

extern "C" {
void dispatchInterrupt(InterruptFrame *frame);
}

/**
 * Central function to deal with interrupts and faults - is called from assembler code
 *
 * @param *frame - pointer to the interrupt frame containing all relevant data
 */
void dispatchInterrupt(InterruptFrame *frame) {
    InterruptDispatcher::getInstance().dispatch(frame);
}

InterruptDispatcher::InterruptDispatcher() : debugHandlers(), handler() {
    assign(0x80, systemCall);
}

InterruptDispatcher &InterruptDispatcher::getInstance() noexcept {

    static InterruptDispatcher instance;

    return instance;
}

void InterruptDispatcher::dispatch(InterruptFrame *frame) {

    // Extract interrupt information
    uint8_t slot = (uint8_t) frame->interrupt;
    uint32_t flags = frame->error;

    // Throw bluescreen on Protected Mode exceptions except pagefault
    if (slot < 32 && !GdbServer::isInitialized() && slot != (uint32_t) Cpu::Error::PAGE_FAULT) {

        System::panic(frame);
    }

    // If this is a software exception, throw a bluescreen with error data
    if (slot >= Cpu::SOFTWARE_EXCEPTIONS_START) {

        System::panic(frame);
    }

    // Ignore spurious interrupts
    if (slot == 39 && Pic::isSpurious()) {

        return;
    }

    if (slot == 14) {

        GdbServer::memError = true;

        // Get page fault address and flags
        uint32_t faulting_address = 0;
        // the faulted linear address is loaded in the cr2 register
        asm ("mov %%cr2, %0" : "=r" (faulting_address));

        // There should be no access to the first page (address 0)
        if (faulting_address == 0) {
            frame->interrupt = (uint32_t) Cpu::Exception::NULLPOINTER;
            System::panic(frame);
        }

        // Pass faulting address to the system management
        Management::getInstance().setFaultParams(faulting_address, flags);
    }

    if (handler.size() == 0) {

        sendEoi(slot);

        return;
    }

    Util::List<InterruptHandler *> *list = report(slot);

    if (list == nullptr && slot >= 32) {
        char error[256];

        sprintf(error, "No handler registered for interrupt number %u!", frame->interrupt);

        Cpu::throwException(Cpu::Exception::ILLEGAL_STATE, error);

        return;
    }

    if (list != nullptr) {
        uint32_t size = list->size();

        for (uint32_t i = 0; i < size; i++) {

            list->get(i)->trigger(*frame);
        }
    }

    if (slot < 32 && GdbServer::isInitialized() && slot != 14) {
        GdbServer::handleInterrupt(*frame);
    }

    sendEoi(slot);
}

void InterruptDispatcher::assign(uint8_t slot, InterruptHandler &isr) {

    if (!handler.containsKey(slot)) {

        handler.put(slot, new Util::ArrayList<InterruptHandler *>);
    }

    handler.get(slot)->add(&isr);
}

Util::List<InterruptHandler *> *InterruptDispatcher::report(uint8_t slot) {

    if (!handler.containsKey(slot)) {

        return nullptr;
    }

    return handler.get(slot);
}

void InterruptDispatcher::sendEoi(uint32_t slot) {

    if (slot > 32) {

        Pic::getInstance().sendEOI(Pic::Interrupt(slot - 32));
    }
}

void InterruptDispatcher::assignDebug(uint8_t slot, debugFunction debugHandler) {

    debugHandlers.put(slot, debugHandler);
}

debugFunction InterruptDispatcher::reportDebug(uint8_t slot) {

    if (!debugHandlers.containsKey(slot)) {

        return nullptr;
    }

    return debugHandlers.get(slot);
}

}