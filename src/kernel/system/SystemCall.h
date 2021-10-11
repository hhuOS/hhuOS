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

#ifndef HHUOS_SYSTEMCALL_H
#define HHUOS_SYSTEMCALL_H

#include <cstdarg>
#include <kernel/interrupt/InterruptHandler.h>
#include <lib/util/system/SystemCall.h>

namespace Kernel {

class SystemCall : public Kernel::InterruptHandler {

public:

    enum Code : uint8_t {
        SCHEDULER_YIELD
    };

    /**
     * Default Constructor.
     */
    SystemCall() = default;

    /**
     * Copy constructor.
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

    static void registerSystemCall(Util::System::SystemCall::Code code, Util::System::SystemCall::Result(*func)(uint32_t paramCount, va_list params));

    void plugin() override;

    void trigger(Kernel::InterruptFrame &frame) override;

private:

    static Util::System::SystemCall::Result(*systemCalls[256])(uint32_t paramCount, va_list params);

};

}

#endif
