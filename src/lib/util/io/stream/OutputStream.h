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

#ifndef HHUOS_OUTPUTSTREAM_H
#define HHUOS_OUTPUTSTREAM_H

#include <cstdint>

namespace Util::Io {

/**
 * Interface for classes, that write output as a stream of bytes.
 */
class OutputStream {

public:

    OutputStream() = default;

    OutputStream(const OutputStream &copy) = delete;

    OutputStream &operator=(const OutputStream &copy) = delete;

    virtual ~OutputStream() = default;

    virtual void write(uint8_t c) = 0;

    virtual void write(const uint8_t *sourceBuffer, uint32_t offset, uint32_t length) = 0;

    virtual void flush();
};

}

#endif
