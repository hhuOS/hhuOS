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

#ifndef HHUOS_BUFFEREDINPUTSTREAM_H
#define HHUOS_BUFFEREDINPUTSTREAM_H

#include "FilterInputStream.h"

namespace Util::Stream {

class BufferedInputStream : public FilterInputStream {

public:

    explicit BufferedInputStream(InputStream &stream);

    BufferedInputStream(InputStream &stream, uint32_t size);

    BufferedInputStream(const BufferedInputStream &copy) = delete;

    BufferedInputStream &operator=(const BufferedInputStream &copy) = delete;

    ~BufferedInputStream() override;

    int16_t read() override;

    uint32_t read(uint8_t *target, uint32_t offset, uint32_t length) override;

    void close() override;

private:

    bool refill();

    uint8_t *buffer;
    uint32_t size;
    uint32_t position = 0;
    uint32_t valid = 0;

    static const constexpr uint32_t DEFAULT_BUFFER_SIZE = 512;
};

}

#endif
