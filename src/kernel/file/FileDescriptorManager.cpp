#include "kernel/system/System.h"
#include "kernel/service/FilesystemService.h"
#include "FileDescriptorManager.h"
#include "kernel/file/FileDescriptorManager.h"
#include "filesystem/core/Filesystem.h"
#include "filesystem/core/Node.h"
#include "lib/util/Exception.h"

namespace Kernel {

FileDescriptorManager::FileDescriptorManager(int32_t size) : size(size), descriptorTable(new Filesystem::Node*[size]) {
    if (size < 0) {
        Util::Exception::throwException(Util::Exception::INVALID_ARGUMENT, "FileDescriptorManager: Size is negative!");
    }

    for (int32_t i = 0; i < size; i++) {
        descriptorTable[i] = nullptr;
    }
}

FileDescriptorManager::~FileDescriptorManager() {
    delete[] descriptorTable;
}

int32_t FileDescriptorManager::registerFile(Filesystem::Node *node) {
    for (int32_t fileDescriptor = 0; fileDescriptor < size; fileDescriptor++) {
        if (descriptorTable[fileDescriptor] == nullptr) {
            descriptorTable[fileDescriptor] = node;
            return fileDescriptor;
        }
    }

    return -1;
}

int32_t FileDescriptorManager::openFile(const Util::Memory::String &path) {
    auto &filesystem = Kernel::System::getService<Kernel::FilesystemService>().getFilesystem();
    auto *node = filesystem.getNode(path);
    if (node == nullptr) {
        return -1;
    }

    return registerFile(node);
}

void FileDescriptorManager::closeFile(int32_t fileDescriptor) {
    if (fileDescriptor == -1) {
        Util::Exception::throwException(Util::Exception::INVALID_ARGUMENT, "Invalid file descriptor!");
    }

    auto *node = descriptorTable[fileDescriptor];
    if (node != nullptr) {
        delete node;
        descriptorTable[fileDescriptor] = nullptr;
    }
}

Filesystem::Node &FileDescriptorManager::getNode(int32_t fileDescriptor) {
    if (fileDescriptor == -1) {
        Util::Exception::throwException(Util::Exception::INVALID_ARGUMENT, "Invalid file descriptor!");
    }

    auto *node = descriptorTable[fileDescriptor];
    if (node == nullptr) {
        Util::Exception::throwException(Util::Exception::INVALID_ARGUMENT, "Invalid file descriptor!");
    }

    return *node;
}

}