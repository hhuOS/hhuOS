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

#include "lib/util/Exception.h"
#include "PipedOutputStream.h"
#include "lib/util/stream/PipedInputStream.h"

namespace Util::Stream {

PipedOutputStream::PipedOutputStream(PipedInputStream &inputStream) : sink(&inputStream) {

}

void PipedOutputStream::connect(PipedInputStream &inputStream) {
    if (sink != nullptr) {
        Exception::throwException(Exception::ILLEGAL_STATE, "PipedOutputStream: Already connected!");
    }

    inputStream.connect(*this);
}

void PipedOutputStream::write(uint8_t c) {
    if (sink == nullptr) {
        return;
    }

    sink->write(c);
}

void PipedOutputStream::write(const uint8_t *sourceBuffer, uint32_t offset, uint32_t length) {
    if (sink == nullptr) {
        return;
    }

    sink->write(sourceBuffer, offset, length);
}

}