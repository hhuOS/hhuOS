#include "ArchiveDirectoryNode.h"

#include "lib/util/Exception.h"
#include "lib/util/file/tar/Archive.h"

namespace Filesystem::Tar {

ArchiveDirectoryNode::ArchiveDirectoryNode(Util::File::Tar::Archive &archive, const Util::Memory::String &path) {
    if(path.isEmpty() || path == "/") {
        name = "/";
    } else {
        Util::Data::Array<Util::Memory::String> tokens = path.split("/");
        name = tokens[tokens.length() - 1];
    }

    for(const auto &header : archive.getFileHeaders()) {
        Util::Memory::String fullPath = header.filename;

        if(fullPath.beginsWith(path) && fullPath != path) {
            Util::Memory::String subPath = fullPath.substring(path.length(), fullPath.length());
            Util::Memory::String childName = subPath.split("/")[0];

            if(!children.contains(childName)) {
                children.add(childName);
            }
        }
    }
}

Util::Memory::String ArchiveDirectoryNode::getName() {
    return name;
}

Util::File::Type ArchiveDirectoryNode::getFileType() {
    return Util::File::DIRECTORY;
}

uint64_t ArchiveDirectoryNode::getLength() {
    return 0;
}

Util::Data::Array<Util::Memory::String> ArchiveDirectoryNode::getChildren() {
    return children.toArray();
}

uint64_t ArchiveDirectoryNode::readData(uint8_t *targetBuffer, uint64_t pos, uint64_t numBytes) {
    Util::Exception::throwException(Util::Exception::UNSUPPORTED_OPERATION, "ArchiveDriver: Trying to read from a directory!");
}

uint64_t ArchiveDirectoryNode::writeData(const uint8_t *sourceBuffer, uint64_t pos, uint64_t numBytes) {
    Util::Exception::throwException(Util::Exception::UNSUPPORTED_OPERATION, "ArchiveDriver: Trying to write to a directory!");
}

}