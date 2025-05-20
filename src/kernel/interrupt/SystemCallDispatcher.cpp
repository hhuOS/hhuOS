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

#include "lib/util/base/Panic.h"
#include "SystemCallDispatcher.h"
#include "kernel/interrupt/SystemCallDispatcher.h"

namespace Kernel {

void SystemCallDispatcher::assign(Util::System::Code code, bool(*func)(uint32_t, va_list)) {
    if (systemCalls[code] != nullptr) {
        Util::Panic::fire(Util::Panic::INVALID_ARGUMENT, "SystemCallDispatcher: Code is already assigned!");
    }

    systemCalls[code] = func;
}

void SystemCallDispatcher::dispatch(Util::System::Code code, uint16_t paramCount, va_list params, bool &result) const {
    if (systemCalls[code] == nullptr) {
        Util::Panic::fire(Util::Panic::OUT_OF_BOUNDS, "SystemCallDispatcher: No handler registered!");
    }

    result = systemCalls[code](paramCount, params);
}

}