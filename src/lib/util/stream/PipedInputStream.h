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

#ifndef HHUOS_PIPEDINPUTSTREAM_H
#define HHUOS_PIPEDINPUTSTREAM_H

#include <cstdint>

#include "InputStream.h"

namespace Util::Stream {

class PipedOutputStream;

class PipedInputStream : public InputStream {

public:

    PipedInputStream();

    explicit PipedInputStream(PipedOutputStream &outputStream);

    PipedInputStream(const PipedInputStream &copy) = delete;

    PipedInputStream &operator=(const PipedInputStream &copy) = delete;

    ~PipedInputStream() override;

    void connect(PipedOutputStream &outputStream);

    int16_t read() override;

    int32_t read(uint8_t *targetBuffer, uint32_t offset, uint32_t length) override;

private:

    virtual void write(uint8_t c);

    virtual void write(const uint8_t *sourceBuffer, uint32_t offset, uint32_t length);

    PipedOutputStream *source = nullptr;

    uint8_t *buffer;
    int32_t inPosition = -1;
    int32_t outPosition = 0;

    static const constexpr uint32_t BUFFER_SIZE = 1024;

    friend class PipedOutputStream;

};

}

#endif
