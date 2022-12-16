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

#ifndef HHUOS_SYSTEMCALL_H
#define HHUOS_SYSTEMCALL_H

#include <cstdint>
#include <cstdarg>

#include "kernel/interrupt/InterruptHandler.h"
#include "lib/util/system/System.h"

namespace Kernel {
struct InterruptFrame;

class SystemCall : public Kernel::InterruptHandler {

public:
    /**
     * Default Constructor.
     */
    SystemCall() = default;

    /**
     * Copy Constructor.
     */
    SystemCall(const SystemCall &other) = delete;

    /**
     * Assignment operator.
     */
    SystemCall &operator=(const SystemCall &other) = delete;

    /**
     * Destructor.
     */
    ~SystemCall() override = default;

    static void registerSystemCall(Util::System::Code code, Util::System::Result(*func)(uint32_t paramCount, va_list params));

    void plugin() override;

    void trigger(const Kernel::InterruptFrame &frame) override;

private:

    static Util::System::Result(*systemCalls[256])(uint32_t paramCount, va_list params);

};

}

#endif
