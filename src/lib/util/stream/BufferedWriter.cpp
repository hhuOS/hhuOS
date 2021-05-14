/*
 * Copyright (C) 2018-2021 Heinrich-Heine-Universitaet Duesseldorf,
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

#include <lib/util/memory/Address.h>
#include "BufferedWriter.h"

namespace Util::Stream {

BufferedWriter::BufferedWriter(Writer &writer, uint32_t size) : writer(writer), buffer(new char[size]), size(size) {}

BufferedWriter::~BufferedWriter() {
    delete[] buffer;
}

void BufferedWriter::close() {
    writer.close();
}

void BufferedWriter::flush() {
    writer.write(reinterpret_cast<const char*>(buffer), 0, position);
    position = 0;
    writer.flush();
}

void BufferedWriter::write(const char *sourceBuffer, uint32_t offset, uint32_t length) {
    if (offset < 0 || length < 0) {
        Exception::throwException(Exception::OUT_OF_BOUNDS, "BufferedWriter: Negative offset or size!");
    }

    if (length < (size - position)) {
        Memory::Address<uint32_t> sourceAddress(sourceBuffer + offset);
        Memory::Address<uint32_t> targetAddress(buffer + position);

        targetAddress.copyRange(sourceAddress, length);
        position += length;
    } else {
        flush();
        writer.write(sourceBuffer, offset, length);
    }
}

}