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

#ifndef HHUOS_EXCEPTION_H
#define HHUOS_EXCEPTION_H

#include <cstdint>

namespace Util {

class Exception {

public:

    enum Error : uint32_t {
        NULL_POINTER = 0xc8,
        OUT_OF_BOUNDS = 0xc9,
        INVALID_ARGUMENT = 0xca,
        KEY_NOT_FOUND = 0xcb,
        ILLEGAL_STATE = 0xcc,
        OUT_OF_MEMORY = 0Xcd,
        OUT_OF_PHYSICAL_MEMORY = 0xce,
        OUT_OF_PAGING_MEMORY = 0xcf,
        ILLEGAL_PAGE_ACCESS = 0xd0,
        CLASS_NOT_FOUND = 0xd1,
        PAGING_ERROR = 0xd2,
        UNSUPPORTED_OPERATION = 0xd3
    };

    [[noreturn]] static void throwException(Error error, const char *message = "");

    [[nodiscard]] static const char* getExceptionName(Error error);

private:

    static const char *softwareExceptions[];

};

}

#endif