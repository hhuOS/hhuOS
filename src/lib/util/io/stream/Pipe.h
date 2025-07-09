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

#ifndef AUDIOCHANNEL_H
#define AUDIOCHANNEL_H

#include "io/stream/FilterInputStream.h"
#include "io/stream/FilterOutputStream.h"
#include "io/stream/PipedInputStream.h"
#include "io/stream/PipedOutputStream.h"

namespace Util::Io {

class Pipe : public FilterInputStream, public FilterOutputStream {

public:
    /**
     * Default Constructor.
     */
    explicit Pipe(int32_t bufferSize = DEFAULT_BUFFER_SIZE);

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

    uint32_t getReadableBytes();

    uint32_t getWritableBytes();

    void reset();

    InputStream& getInputStream();

    OutputStream& getOutputStream();

private:

    PipedInputStream inputStream;
    PipedOutputStream outputStream;

    static const constexpr int32_t DEFAULT_BUFFER_SIZE = 1024;
};

}

#endif
