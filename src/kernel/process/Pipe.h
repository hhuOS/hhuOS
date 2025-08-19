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

#ifndef HHUOS_PIPE_H
#define HHUOS_PIPE_H

#include "lib/util/io/stream/Pipe.h"

namespace Kernel {

class Pipe : public Util::Io::FilterInputStream, public Util::Io::FilterOutputStream {

public:
    /**
     * Constructor.
     */
    explicit Pipe(uint32_t readerId);

    /**
     * Copy Constructor.
     */
    Pipe(const Pipe &other) = delete;

    /**
     * Assignment operator.
     */
    Pipe &operator=(const Pipe &other) = delete;

    /**
     * Destructor.
     */
    ~Pipe() override = default;

    int16_t read() override;

    int16_t peek() override;

    int32_t read(uint8_t *targetBuffer, uint32_t offset, uint32_t length) override;

    bool isReadyToRead() override;

    bool write(uint8_t c) override;

    uint32_t write(const uint8_t *sourceBuffer, uint32_t offset, uint32_t length) override;

    uint32_t flush() override;

private:

    bool checkWriteAccess(uint32_t processId);

    Util::Io::PipedInputStream inputStream;
    Util::Io::PipedOutputStream outputStream;

    uint32_t readerId;
    uint32_t writerId = 0;
};

}

#endif