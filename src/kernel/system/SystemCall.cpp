/*
 * Copyright (C) 2018-2021 Heinrich-Heine-Universitaet Duesseldorf,
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

#include "lib/util/Exception.h"
#include "kernel/interrupt/InterruptDispatcher.h"
#include "SystemCall.h"

namespace Kernel {

Util::System::SystemCall::Result(*SystemCall::systemCalls[256])(uint32_t paramCount, va_list params){};

void SystemCall::registerSystemCall(Util::System::SystemCall::Code code, Util::System::SystemCall::Result (*func)(uint32_t, va_list)) {
    if (systemCalls[code] != nullptr) {
        Util::Exception::throwException(Util::Exception::INVALID_ARGUMENT, "SystemCall: Code is already assigned");
    }

    systemCalls[code] = func;
}

void SystemCall::plugin() {
    Kernel::InterruptDispatcher::getInstance().assign(Kernel::InterruptDispatcher::SYSTEM_CALL, *this);
}

void SystemCall::trigger(Kernel::InterruptFrame &frame) {
    uint16_t code = frame.eax & 0x0000ffffu;
    uint16_t paramCount = frame.eax >> 16;
    auto params = reinterpret_cast<va_list>(frame.ebx);
    auto &result = *reinterpret_cast<Util::System::SystemCall::Result*>(frame.ecx);

    result = systemCalls[code](paramCount, params);
}

}