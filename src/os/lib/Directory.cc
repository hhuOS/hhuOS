#include "Directory.h"

/**
 * Tries to open the directory at a specified path.
 * 
 * @param path The directory's path.
 * 
 * @return On success, the directory;
 *         else nullptr.
 */
Directory *Directory::open(const String &path) {
    FileSystem *fileSystem = Kernel::getService<FileSystem>();

    FsNode *node = fileSystem->getNode(path);

    if(node != nullptr && node->getFileType() == DIRECTORY_FILE) {
        if(!node->getName().beginsWith(".")) {
            return new Directory(node, path);
        }
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