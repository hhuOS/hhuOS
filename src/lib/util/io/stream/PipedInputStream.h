/*
 * Copyright (C) 2017-2025 Heinrich Heine University Düsseldorf,
 * Institute of Computer Science, Department Operating Systems
 * Main developers: Christian Gesse <christian.gesse@hhu.de>, Fabian Ruhland <ruhland@hhu.de>
 * Original development team: Burak Akguel, Christian Gesse, Fabian Ruhland, Filip Krakowski, Michael Schöttner
 * This project has been supported by several students.
 * A full list of integrated student theses can be found here: https://github.com/hhuOS/hhuOS/wiki/Student-theses
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

#include <stdint.h>

#include "InputStream.h"
#include "lib/util/async/Spinlock.h"

namespace Util::Io {

class PipedOutputStream;

class PipedInputStream : public InputStream {

public:

    explicit PipedInputStream(int32_t bufferSize = DEFAULT_BUFFER_SIZE);

    explicit PipedInputStream(PipedOutputStream &outputStream, int32_t bufferSize = DEFAULT_BUFFER_SIZE);

    PipedInputStream(const PipedInputStream &copy) = delete;

    PipedInputStream &operator=(const PipedInputStream &copy) = delete;

    ~PipedInputStream() override;

    void connect(PipedOutputStream &outputStream);

    int16_t read() override;
	
	int16_t peek() override;

    int32_t read(uint8_t *targetBuffer, uint32_t offset, uint32_t length) override;

	void reset();

    bool isReadyToRead() override;

    uint32_t getReadableBytes();

	uint32_t getWritableBytes();

private:

    virtual bool write(uint8_t c);

    virtual uint32_t write(const uint8_t *sourceBuffer, uint32_t offset, uint32_t length);

    PipedOutputStream *source = nullptr;

    Util::Async::Spinlock lock;

    uint8_t *buffer;
    int32_t bufferSize;

    int32_t inPosition = -1;
    int32_t outPosition = 0;
	
	int16_t peekedCharacter = -1;

    static const constexpr uint32_t DEFAULT_BUFFER_SIZE = 1024;

    friend class PipedOutputStream;

};

}

#endif
