/*
 * Copyright (C) 2018-2023 Heinrich-Heine-Universitaet Duesseldorf,
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
        NULL_POINTER = 0xC8,
        OUT_OF_BOUNDS = 0xC9,
        INVALID_ARGUMENT = 0xCA,
        KEY_NOT_FOUND = 0xCB,
        ILLEGAL_STATE = 0xCC,
        OUT_OF_MEMORY = 0XCD,
        OUT_OF_PHYSICAL_MEMORY = 0xCE,
        OUT_OF_PAGING_MEMORY = 0xCF,
        ILLEGAL_PAGE_ACCESS = 0xD0,
        CLASS_NOT_FOUND = 0xD1,
        PAGING_ERROR = 0xD2,
        UNSUPPORTED_OPERATION = 0xD3
    };

    [[noreturn]] static void throwException(Error error, const char *message = "");

    [[nodiscard]] static const char* getExceptionName(Error error);

private:

    static const char *softwareExceptions[];

};

}

#endif