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

#ifndef HHUOS_BUFFEREDOUTPUTSTREAM_H
#define HHUOS_BUFFEREDOUTPUTSTREAM_H

#include <stdint.h>

#include "FilterOutputStream.h"

namespace Util {
namespace Io {
class OutputStream;
}  // namespace Io
}  // namespace Util

namespace Util::Io {

class BufferedOutputStream : public FilterOutputStream {

public:

    explicit BufferedOutputStream(OutputStream &stream, uint32_t size = DEFAULT_BUFFER_SIZE);

    BufferedOutputStream(const BufferedOutputStream &copy) = delete;

    BufferedOutputStream &operator=(const BufferedOutputStream &copy) = delete;

    ~BufferedOutputStream() override;

    bool write(uint8_t c) override;

    uint32_t write(const uint8_t *sourceBuffer, uint32_t offset, uint32_t length) override;

    uint32_t flush() override;

private:

    uint8_t *buffer;
    uint32_t size;
    uint32_t position = 0;

    static const constexpr uint32_t DEFAULT_BUFFER_SIZE = 512;
};

}

#endif
