/*
 * Copyright (C) 2021 Heinrich-Heine-Universitaet Duesseldorf,
 * Institute of Computer Science, Department Operating Systems
 *
 * This program is free software: you can redistribute it and/or modify it
 * under the terms of the GNU General Public License as published by the
 * Free Software Foundation, either version 3 of the License,
 * or (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.
 * See the GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program.  If not, see <http://www.gnu.org/licenses/>
 */

#ifndef HHUOS_INPUTSTREAM_H
#define HHUOS_INPUTSTREAM_H

#include <cstdint>

namespace Util::Stream {

class InputStream {

public:

    InputStream() = default;

    InputStream(const InputStream &copy) = delete;

    InputStream& operator=(const InputStream &copy) = delete;

    virtual ~InputStream() = default;

    virtual int16_t read() = 0;

    virtual uint32_t read(uint8_t *target, uint32_t offset, uint32_t length);

    virtual void close();
};

}

#endif
