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

#ifndef HHUOS_BUFFEREDINPUTSTREAM_H
#define HHUOS_BUFFEREDINPUTSTREAM_H

#include <stdint.h>

#include "FilterInputStream.h"

namespace Util {
namespace Io {
class InputStream;
}  // namespace Io
}  // namespace Util

namespace Util::Io {

class BufferedInputStream : public FilterInputStream {

public:

    explicit BufferedInputStream(InputStream &stream);

    BufferedInputStream(InputStream &stream, uint32_t size);

    BufferedInputStream(const BufferedInputStream &copy) = delete;

    BufferedInputStream &operator=(const BufferedInputStream &copy) = delete;

    ~BufferedInputStream() override;

    int16_t read() override;
	
	int16_t peek() override;

    int32_t read(uint8_t *target, uint32_t offset, uint32_t length) override;

    bool isReadyToRead() override;

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
