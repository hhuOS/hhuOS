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

#include "lib/util/collection/ArrayList.h"
#include "kernel/interrupt/InterruptDispatcher.h"
#include "kernel/interrupt/InterruptHandler.h"
#include "lib/util/base/Exception.h"
#include "lib/util/collection/List.h"

namespace Kernel {
struct InterruptFrame;

void InterruptDispatcher::dispatch(const InterruptFrame &frame, InterruptVector vector) {
    // Throw exception, if there is no handler registered
    auto *handlerList = handler[vector];
    if (handlerList == nullptr) {
        Util::Exception::throwException(Util::Exception::ILLEGAL_STATE, "InterruptDispatcher: No handler registered!");
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

}