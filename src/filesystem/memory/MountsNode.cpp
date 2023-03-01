/*
 * Copyright (C) 2018-2023 Heinrich-Heine-Universitaet Duesseldorf,
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

#include "MountsNode.h"

#include "kernel/service/FilesystemService.h"
#include "kernel/system/System.h"
#include "lib/util/io/stream/ByteArrayOutputStream.h"
#include "filesystem/core/Filesystem.h"
#include "filesystem/memory/MemoryNode.h"
#include "lib/util/collection/Array.h"
#include "lib/util/base/Address.h"
#include "lib/util/io/stream/PrintStream.h"

namespace Filesystem::Memory {

MountsNode::MountsNode(const Util::String &name) : MemoryNode(name) {}

uint64_t MountsNode::getLength() {
    return buildBuffer().length();
}

uint64_t MountsNode::readData(uint8_t *targetBuffer, uint64_t pos, uint64_t numBytes) {
    auto buffer = buildBuffer();

    if (pos >= buffer.length()) {
        return 0;
    }

    if (pos + numBytes > buffer.length()) {
        numBytes = (buffer.length() - pos);
    }

    auto sourceAddress = Util::Address<uint32_t>(static_cast<const char*>(buffer)).add(pos);
    auto targetAddress = Util::Address<uint32_t>(targetBuffer);
    targetAddress.copyRange(sourceAddress, numBytes);

    return numBytes;
}

Util::String MountsNode::buildBuffer() {
    auto stream = Util::Io::ByteArrayOutputStream();
    auto printStream = Util::Io::PrintStream(stream);

    auto mountInformation = Kernel::System::getService<Kernel::FilesystemService>().getMountInformation();
    for (const auto &info : mountInformation) {
        printStream << info.device << " on " << info.target << " type " << info.driver << Util::Io::PrintStream::endl;
    }

    printStream.flush();
    return stream.getContent();
}

}