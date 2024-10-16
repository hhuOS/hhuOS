#include "ArchiveDirectoryNode.h"

#include "lib/util/base/Exception.h"
#include "lib/util/io/file/tar/Archive.h"

namespace Filesystem::Tar {

ArchiveDirectoryNode::ArchiveDirectoryNode(Util::Io::Tar::Archive &archive, const Util::String &path) {
    if(path.isEmpty() || path == "/") {
        name = "/";
    } else {
        Util::Array<Util::String> tokens = path.split("/");
        name = tokens[tokens.length() - 1];
    }

    for(const auto &header : archive.getFileHeaders()) {
        Util::String fullPath = header.filename;

        if(fullPath.beginsWith(path) && fullPath != path) {
            Util::String subPath = fullPath.substring(path.length(), fullPath.length());
            Util::String childName = subPath.split("/")[0];

            if(!children.contains(childName)) {
                children.add(childName);
            }
        }
    }
}

Util::String ArchiveDirectoryNode::getName() {
    return name;
}

Util::Io::File::Type ArchiveDirectoryNode::getType() {
    return Util::Io::File::DIRECTORY;
}

uint64_t ArchiveDirectoryNode::getLength() {
    return 0;
}

Util::Array<Util::String> ArchiveDirectoryNode::getChildren() {
    return children.toArray();
}

uint64_t ArchiveDirectoryNode::readData([[maybe_unused]] uint8_t *targetBuffer, [[maybe_unused]] uint64_t pos, [[maybe_unused]] uint64_t numBytes) {
    Util::Exception::throwException(Util::Exception::UNSUPPORTED_OPERATION, "ArchiveDriver: Trying to read from a directory!");
}

uint64_t ArchiveDirectoryNode::writeData([[maybe_unused]] const uint8_t *sourceBuffer, [[maybe_unused]] uint64_t pos, [[maybe_unused]] uint64_t numBytes) {
    Util::Exception::throwException(Util::Exception::UNSUPPORTED_OPERATION, "ArchiveDriver: Trying to write to a directory!");
}

}