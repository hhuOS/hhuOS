
#include "kernel/service/FilesystemService.h"
#include "FileDescriptorManager.h"
#include "filesystem/Filesystem.h"
#include "lib/util/base/Panic.h"
#include "kernel/service/Service.h"
#include "kernel/process/FileDescriptor.h"

namespace Kernel {

FileDescriptorManager::FileDescriptorManager(int32_t size) : size(size), descriptorTable(new FileDescriptor[size]{}) {
    if (size < 0) {
        Util::Panic::fire(Util::Panic::INVALID_ARGUMENT, "FileDescriptorManager: Size is negative!");
    }
}

FileDescriptorManager::~FileDescriptorManager() {
    delete[] descriptorTable;
}

int32_t FileDescriptorManager::registerFile(Filesystem::Node *node) const {
    for (int32_t fileDescriptor = 0; fileDescriptor < size; fileDescriptor++) {
        if (!descriptorTable[fileDescriptor].isValid()) {
            descriptorTable[fileDescriptor].clear();
            descriptorTable[fileDescriptor].setNode(node);
            return fileDescriptor;
        }
    }

    return -1;
}

int32_t FileDescriptorManager::openFile(const Util::String &path) const {
    auto &filesystem = Kernel::Service::getService<Kernel::FilesystemService>().getFilesystem();
    auto *node = filesystem.getNode(path);
    if (node == nullptr) {
        return -1;
    }

    return registerFile(node);
}

void FileDescriptorManager::closeFile(int32_t fileDescriptor) const {
    if (fileDescriptor < 0) {
        Util::Panic::fire(Util::Panic::INVALID_ARGUMENT, "Invalid file descriptor!");
    }

    descriptorTable[fileDescriptor].clear();
}

FileDescriptor& FileDescriptorManager::getDescriptor(int32_t fileDescriptor) const {
    if (fileDescriptor == -1) {
        Util::Panic::fire(Util::Panic::INVALID_ARGUMENT, "Invalid file descriptor!");
    }

    auto &descriptor = descriptorTable[fileDescriptor];
    if (!descriptor.isValid()) {
        Util::Panic::fire(Util::Panic::INVALID_ARGUMENT, "Invalid file descriptor!");
    }

    return descriptor;
}

}