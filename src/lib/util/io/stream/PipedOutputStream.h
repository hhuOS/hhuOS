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

#ifndef HHUOS_PIPEDOUTPUTSTREAM_H
#define HHUOS_PIPEDOUTPUTSTREAM_H

#include <stdint.h>

#include "OutputStream.h"

namespace Util {
namespace Io {
class PipedInputStream;
}  // namespace Io
}  // namespace Util

namespace Util::Io {

class PipedOutputStream : public OutputStream {

public:

    PipedOutputStream() = default;

    explicit PipedOutputStream(PipedInputStream &inputStream);

    PipedOutputStream(const PipedOutputStream &copy) = delete;

    PipedOutputStream &operator=(const PipedOutputStream &copy) = delete;

    ~PipedOutputStream() override = default;

    void connect(PipedInputStream &sink);

    void write(uint8_t c) override;

    void write(const uint8_t *sourceBuffer, uint32_t offset, uint32_t length) override;

    [[nodiscard]] uint32_t getWritableBytes() const;

private:

    PipedInputStream *sink = nullptr;

    friend class PipedInputStream;

};

}

#endif
