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

#ifndef HHUOS_BUFFEREDOUTPUTSTREAM_H
#define HHUOS_BUFFEREDOUTPUTSTREAM_H

#include <cstdint>

#include "FilterOutputStream.h"

namespace Util {
namespace Stream {
class OutputStream;
}  // namespace Stream
}  // namespace Util

namespace Util::Stream {

class BufferedOutputStream : public FilterOutputStream {

public:

    explicit BufferedOutputStream(OutputStream &stream, uint32_t size = DEFAULT_BUFFER_SIZE);

    BufferedOutputStream(const BufferedOutputStream &copy) = delete;

    BufferedOutputStream &operator=(const BufferedOutputStream &copy) = delete;

    ~BufferedOutputStream() override;

    void write(uint8_t c) override;

    void write(const uint8_t *sourceBuffer, uint32_t offset, uint32_t length) override;

    void flush() override;

private:

    uint8_t *buffer;
    uint32_t size;
    uint32_t position = 0;

    static const constexpr uint32_t DEFAULT_BUFFER_SIZE = 512;
};

}

#endif
