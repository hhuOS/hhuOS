#include "TarArchiveDriver.h"
#include "TarArchiveNode.h"

TarArchiveDriver::TarArchiveDriver(Tar::Archive *archive) :
        archive(archive), fileHeaders(archive->getFileHeaders()) {

}

TarArchiveDriver::~TarArchiveDriver() {
    delete archive;
}

bool TarArchiveDriver::createFs(StorageDevice *device) {
    return false;
}

bool TarArchiveDriver::mount(StorageDevice *device) {
    return false;
}

FsNode *TarArchiveDriver::getNode(const String &path) {
    for(Tar::Header *header : archive->getFileHeaders()) {
        String currentPath = header->filename;

        if(path == currentPath) {
            return new TarArchiveNode(archive, header);
        }

        if(currentPath.beginsWith(path)) {
            return new TarArchiveNode(archive, path);
        }
    }

    return nullptr;
}

bool TarArchiveDriver::createNode(const String &path, uint8_t fileType) {
    return false;
}

bool TarArchiveDriver::deleteNode(const String &path) {
    return false;
}
