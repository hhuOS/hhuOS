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
#include "lib/util/collection/List.h"
#include "lib/util/io/stream/PrintStream.h"
#include "lib/util/base/System.h"
#include "kernel/interrupt/InterruptVector.h"
#include "kernel/system/BlueScreen.h"

namespace Kernel {

void InterruptDispatcher::dispatch(const InterruptFrame &frame, InterruptVector vector) {
    // Throw exception, if there is no handler registered
    auto *handlerList = handler[vector];
    if (handlerList == nullptr) {
        Util::Exception::throwException(Util::Exception::ILLEGAL_STATE, "No handler registered!");
    }

    // Call installed interrupt handlers
    for (uint32_t i = 0; i < handlerList->size(); i++) {
        handlerList->get(i)->trigger(frame, vector);
    }
}

void InterruptDispatcher::assign(uint8_t slot, InterruptHandler &isr) {
    if (handler[slot] == nullptr) {
        handler[slot] = new Util::ArrayList<InterruptHandler*>;
    }

    handler[slot]->add(&isr);
}

bool InterruptDispatcher::isUnrecoverableException(InterruptVector slot) {
    return (slot < PIT || (slot >= NULL_POINTER && slot <= UNSUPPORTED_OPERATION)) && handler[slot] == nullptr;
}

}