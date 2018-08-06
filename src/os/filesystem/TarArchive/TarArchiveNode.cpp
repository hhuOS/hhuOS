#include <lib/libc/printf.h>
#include "TarArchiveNode.h"

TarArchiveNode::TarArchiveNode(Tar::Archive *archive, Tar::Header *fileHeader) {
    fileType = REGULAR_FILE;

    String path = String(fileHeader->filename);

    if(!path.isEmpty()) {
        Util::Array<String> tokens = path.split("/");

        fileName = tokens[tokens.length() - 1];
    }

    fileSize = calculateFileSize(fileHeader->size);

    fileBuffer = archive->getFile(fileHeader->filename);
}

TarArchiveNode::TarArchiveNode(Tar::Archive *archive, const String &path) {
    fileType = DIRECTORY_FILE;

    for(const Tar::Header *header : archive->getFileHeaders()) {
        String fullPath = header->filename;

        if(fullPath.beginsWith(path) && fullPath != path) {
            String subPath = fullPath.substring(path.length(), fullPath.length());

            String childName = subPath.split("/")[0];

            if(!children.contains(childName)) {
                children.add(childName);
            }
        }
    }
}

String TarArchiveNode::getName() {
    return fileName;
}

uint8_t TarArchiveNode::getFileType() {
    return fileType;
}

uint64_t TarArchiveNode::getLength() {
    return fileSize;
}

Util::Array<String> TarArchiveNode::getChildren() {
    return children.toArray();
}

uint64_t TarArchiveNode::readData(char *buf, uint64_t pos, uint64_t numBytes) {
    if(fileType != REGULAR_FILE) {
        return 0;
    }

    if(pos >= fileSize) {
        return 0;
    }

    if (pos + numBytes > fileSize) {
        numBytes = (fileSize - pos);
    }

    for(uint64_t i = 0; i < numBytes; i++) {
        buf[i] = fileBuffer[pos + i];
    }

    return numBytes;
}

uint64_t TarArchiveNode::writeData(char *buf, uint64_t pos, uint64_t numBytes) {
    return 0;
}

uint32_t TarArchiveNode::calculateFileSize(const uint8_t octalSize[12]) {
    uint32_t ret = 0;
    uint32_t count = 1;

    for (uint32_t i = 11; i > 0; i--, count *= 8) {

        ret += ((octalSize[i - 1] - '0') * count);
    }

    return ret;
}