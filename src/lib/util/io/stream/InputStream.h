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

#ifndef HHUOS_INPUTSTREAM_H
#define HHUOS_INPUTSTREAM_H

#include <cstdint>
#include "lib/util/base/String.h"

namespace Util::Io {

class InputStream {

public:

    InputStream() = default;

    InputStream(const InputStream &copy) = delete;

    InputStream& operator=(const InputStream &copy) = delete;

    virtual ~InputStream() = default;

    virtual int16_t read() = 0;

    virtual int32_t read(uint8_t *targetBuffer, uint32_t offset, uint32_t length) = 0;

    String readString(uint32_t length);

    String readLine();

    uint32_t skip(uint32_t amount);

private:

    static const constexpr uint32_t SKIP_BUFFER_SIZE = 1024;

};

}

#endif
