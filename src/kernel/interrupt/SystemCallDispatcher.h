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

#ifndef HHUOS_SYSTEMCALLDISPATCHER_H
#define HHUOS_SYSTEMCALLDISPATCHER_H

#include <stdint.h>
#include <stdarg.h>

#include "lib/util/base/System.h"

namespace Kernel {

class SystemCallDispatcher {

public:
    /**
     * Default Constructor.
     */
    SystemCallDispatcher() = default;

    /**
     * Copy Constructor.
     */
    SystemCallDispatcher(const SystemCallDispatcher &other) = delete;

    /**
     * Assignment operator.
     */
    SystemCallDispatcher &operator=(const SystemCallDispatcher &other) = delete;

    /**
     * Destructor.
     */
    ~SystemCallDispatcher() = default;

    void assign(Util::System::Code code, bool(*func)(uint32_t paramCount, va_list params));

    void dispatch(Util::System::Code code, uint16_t paramCount, va_list params, bool &result) const;

private:

    bool(*systemCalls[256])(uint32_t paramCount, va_list params){};

};

}

#endif
