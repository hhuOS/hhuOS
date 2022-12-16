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

#include "FilterOutputStream.h"

#include "lib/util/stream/OutputStream.h"

namespace Util::Stream {

FilterOutputStream::FilterOutputStream(OutputStream &stream) : stream(stream) {}

void FilterOutputStream::write(uint8_t c) {
    stream.write(c);
}

void FilterOutputStream::write(const uint8_t *sourceBuffer, uint32_t offset, uint32_t length) {
    stream.write(sourceBuffer, offset, length);
}

void FilterOutputStream::flush() {
    stream.flush();
}

}