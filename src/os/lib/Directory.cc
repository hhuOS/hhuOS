#include "Directory.h"

Directory::Directory(FsNode *node, const String &path) : node(node) {
    this->path = FileSystem::parsePath(path);
};

Directory::~Directory() {
    delete node;
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
    return node->getName();
}

String Directory::getAbsolutePath() {
    return path;
}

Util::Array<String> Directory::getChildren() {
    return node->getChildren();
}