#include "ArchiveFileNode.h"

namespace Filesystem::Tar {

ArchiveFileNode::ArchiveFileNode(Util::File::Tar::Archive &archive, Util::File::Tar::Archive::Header fileHeader) {
    auto path = Util::Memory::String(fileHeader.filename);
    if(!path.isEmpty()) {
        Util::Data::Array<Util::Memory::String> tokens = path.split("/");
        name = tokens[tokens.length() - 1];
    }

    length = Util::File::Tar::Archive::calculateFileSize(fileHeader);
    dataAddress = Util::Memory::Address<uint32_t>(archive.getFile(path));
}

Util::Memory::String ArchiveFileNode::getName() {
    return name;
}

Util::File::Type ArchiveFileNode::getFileType() {
    return Util::File::REGULAR;
}

uint64_t ArchiveFileNode::getLength() {
    return length;
}

Util::Data::Array<Util::Memory::String> ArchiveFileNode::getChildren() {
    return Util::Data::Array<Util::Memory::String>(0);
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

    auto targetAddress = Util::Memory::Address<uint32_t>(targetBuffer);
    targetAddress.copyRange(dataAddress.add(pos), numBytes);

    return numBytes;
}

uint64_t ArchiveFileNode::writeData(const uint8_t *sourceBuffer, uint64_t pos, uint64_t numBytes) {
    return 0;
}

}