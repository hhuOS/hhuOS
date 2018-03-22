#include "Directory.h"

Directory::Directory(FsNode *node, const String &path) : node(node) {
    this->path = FileSystem::parsePath(path);
};

Directory::~Directory() {
    if(node != nullptr) {
        delete node;
    }
}

Directory *Directory::open(const String &path) {
    auto *fileSystem = Kernel::getService<FileSystem>();

    FsNode *node = fileSystem->getNode(path);

    if(node != nullptr && node->getFileType() == FsNode::DIRECTORY_FILE) {
        return new Directory(node, path);
    }

    if(node != nullptr) {
        delete node;
    }

    return nullptr;
}

String Directory::getName() {
    if(node->getName() == "/" && path != "/") {
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

String Directory::getAbsolutePath() {
    return path;
}

Util::Array<String> Directory::getChildren() {
    return node->getChildren();
}