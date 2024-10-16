#include "ArchiveFileNode.h"

namespace Filesystem::Tar {

ArchiveFileNode::ArchiveFileNode(Util::Io::Tar::Archive &archive, Util::Io::Tar::Archive::Header fileHeader) {
    auto path = Util::String(fileHeader.filename);
    if(!path.isEmpty()) {
        Util::Array<Util::String> tokens = path.split("/");
        name = tokens[tokens.length() - 1];
    }

    length = Util::Io::Tar::Archive::calculateFileSize(fileHeader);
    dataAddress = Util::Address<uint32_t>(archive.getFile(path));
}

Util::String ArchiveFileNode::getName() {
    return name;
}

Util::Io::File::Type ArchiveFileNode::getType() {
    return Util::Io::File::REGULAR;
}

uint64_t ArchiveFileNode::getLength() {
    return length;
}

Util::Array<Util::String> ArchiveFileNode::getChildren() {
    return Util::Array<Util::String>(0);
}

uint64_t ArchiveFileNode::readData(uint8_t *targetBuffer, uint64_t pos, uint64_t numBytes) {
    if (dataAddress == 0) {
        return 0;
    }

    if (pos >= length) {
        return 0;
    }

    if (pos + numBytes > length) {
        numBytes = (length - pos);
    }

    auto targetAddress = Util::Address<uint32_t>(targetBuffer);
    targetAddress.copyRange(dataAddress.add(pos), numBytes);

    return numBytes;
}

uint64_t ArchiveFileNode::writeData([[maybe_unused]] const uint8_t *sourceBuffer, [[maybe_unused]] uint64_t pos, [[maybe_unused]] uint64_t numBytes) {
    return 0;
}

}