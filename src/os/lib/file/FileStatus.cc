#include <lib/String.h>
#include <kernel/filesystem/FsNode.h>
#include <kernel/services/FileSystem.h>
#include <kernel/Kernel.h>
#include "FileStatus.h"

FileStatus::FileStatus(FsNode *node, const String &path) : node(node) {
    this->path = FileSystem::parsePath(path);
}

FileStatus::~FileStatus() {
    if(node != nullptr) {
        delete node;
    }
}

bool FileStatus::exists(const String &path) {
    auto *filesystem = Kernel::getService<FileSystem>();
    FsNode *node = filesystem->getNode(path);

    if(node == nullptr) {
        return false;
    }

    delete node;
    return true;
}

FileStatus *FileStatus::stat(const String &path) {
    auto *filesystem = Kernel::getService<FileSystem>();
    FsNode *node = filesystem->getNode(path);

    if(node == nullptr) {
        return nullptr;
    }

    return new FileStatus(node, path);
}

String FileStatus::getName() {
    return node->getName();
}

String FileStatus::getAbsolutePath() {
    return path;
}

uint32_t FileStatus::getFileType() {
    return node->getFileType();
}