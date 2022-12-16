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

#ifndef HHUOS_READER_H
#define HHUOS_READER_H

#include <cstdint>

#include "lib/util/memory/String.h"

namespace Util::Stream {

class Reader {

public:

    Reader() = default;

    Reader(const Reader &copy) = delete;

    Reader &operator=(const Reader &copy) = delete;

    virtual ~Reader() = default;

    virtual char read() = 0;

    virtual int32_t read(char *targetBuffer, uint32_t length) = 0;

    virtual int32_t read(char *targetBuffer, uint32_t offset, uint32_t length) = 0;

    virtual Memory::String read(uint32_t length) = 0;

    uint32_t skip(uint32_t amount);

private:

    static const constexpr uint32_t SKIP_BUFFER_SIZE = 1024;

};

}

#endif
