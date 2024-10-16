/*
 * Copyright (C) 2018-2024 Heinrich-Heine-Universitaet Duesseldorf,
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

#include "StreamNode.h"

#include "filesystem/memory/MemoryNode.h"
#include "lib/util/io/stream/InputStream.h"
#include "lib/util/io/stream/OutputStream.h"

namespace Filesystem::Memory {

StreamNode::StreamNode(const Util::String &name, Util::Io::OutputStream *outputStream, Util::Io::InputStream *inputStream) : MemoryNode(name), outputStream(outputStream), inputStream(inputStream) {}

StreamNode::StreamNode(const Util::String &name, Util::Io::OutputStream *outputStream) : MemoryNode(name), outputStream(outputStream), inputStream(nullptr) {}

StreamNode::StreamNode(const Util::String &name, Util::Io::InputStream *inputStream) : MemoryNode(name), outputStream(nullptr), inputStream(inputStream) {}

Util::Io::File::Type StreamNode::getType() {
    return Util::Io::File::CHARACTER;
}

uint64_t StreamNode::readData(uint8_t *targetBuffer, [[maybe_unused]] uint64_t pos, uint64_t numBytes) {
    return inputStream->read(targetBuffer, 0, numBytes);
}

uint64_t StreamNode::writeData(const uint8_t *sourceBuffer, [[maybe_unused]] uint64_t pos, uint64_t numBytes) {
    outputStream->write(sourceBuffer, 0, numBytes);
    return numBytes;
}

StreamNode::~StreamNode() {
    delete outputStream;
    if (reinterpret_cast<void*>(outputStream) != reinterpret_cast<void*>(inputStream)) {
        delete inputStream;
    }
}

bool StreamNode::isReadyToRead() {
    return inputStream->isReadyToRead();
}

}