/*
 * Copyright (C) 2017-2026 Heinrich Heine University Düsseldorf,
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

#ifndef HHUOS_SYSTEMCALLDISPATCHER_H
#define HHUOS_SYSTEMCALLDISPATCHER_H

#include "InterruptFrame.h"
#include "lib/util/base/System.h"

#define ASSIGN_SYSTEM_CALL(code, func) SystemCallDispatcher::assign(code, reinterpret_cast<void(*)()>(&func))

namespace Kernel {

class SystemCallDispatcher {
public:

    SystemCallDispatcher() = delete;

    ~SystemCallDispatcher() = delete;

    static void assign(const Util::System::Code code, void(*func)()) {
        if (systemCalls[code] != nullptr) {
            Util::Panic::fire(Util::Panic::INVALID_ARGUMENT, "SystemCallDispatcher: Code is already assigned!");
        }

        systemCalls[code] = func;
    }

    [[gnu::naked]] static void dispatch();

private:

    static void(*systemCalls[256])();

};

}

#endif
