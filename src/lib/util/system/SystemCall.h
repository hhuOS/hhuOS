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

#ifndef HHUOS_USER_SYSTEMCALL_H
#define HHUOS_USER_SYSTEMCALL_H


#include <cstdint>

namespace Util::System {

class SystemCall {

public:

    enum Code : uint16_t {
        SYSTEM_CALL_TEST,
        SCHEDULER_YIELD
    };

    enum Result : uint16_t {
        OK,
        NOT_INITIALIZED
    };

    /**
     * Default Constructor.
     * Deleted, as this class has only static members.
     */
    SystemCall() = delete;

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
    ~SystemCall() = default;

    static Result execute(Code code, uint32_t paramCount...);

private:

    static void execute(Code code, Result &result, uint32_t paramCount...);

};

}

#endif
