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
    if(node->getFileType() == FsNode::DIRECTORY_FILE && node->getName() == "/" && path != "/") {
        // This directory is the root of a mount-point.
        // We need to find the name, of the directory, where it is mounted in.
        Util::Array<String> tmp =  String(path).split(path, '/');

        if(tmp.length() == 0) {
            return path.substring(1, path.length());
        }

        return tmp[tmp.length() - 1];
    }

    return node->getName();
}

String FileStatus::getAbsolutePath() {
    return path;
}

uint32_t FileStatus::getFileType() {
    return node->getFileType();
}