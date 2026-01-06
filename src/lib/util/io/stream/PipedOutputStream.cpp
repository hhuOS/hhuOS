/*
 * Copyright (C) 2017-2026 Heinrich Heine University Düsseldorf,
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

#include "PipedOutputStream.h"

#include "util/base/Panic.h"
#include "util/io/stream/PipedInputStream.h"

namespace Util {
namespace Io {

void PipedOutputStream::connect(PipedInputStream &sink) {
    if (PipedOutputStream::sink != nullptr) {
        Panic::fire(Panic::ILLEGAL_STATE, "PipedOutputStream: Already connected!");
    }

    sink.connect(*this);
}

bool PipedOutputStream::write(const uint8_t byte) {
    if (sink == nullptr) {
        return false;
    }

    return sink->write(byte);
}

size_t PipedOutputStream::write(const uint8_t *sourceBuffer, size_t offset, size_t length) {
    if (sink == nullptr) {
        return 0;
    }

    return sink->write(sourceBuffer, offset, length);
}

size_t PipedOutputStream::getWritableBytes() const {
    return sink->getWritableBytes();
}

}
}
