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

/**
 * Returns a DirEntry-struct, that contains information about this directory.
 * 
 * @return The DirEntry-struct
 */
DirEntry *Directory::getInfo() {

    DirEntry *entry = new DirEntry();

    if (node->getName() == FileSystem::ROOT && path != FileSystem::ROOT) {

        Util::Array<String> token = path.split(FileSystem::SEPARATOR);

        entry->name = token[token.length() - 1];

    } else {
        entry->name = node->getName();
    }

    entry->fullPath = path;

    entry->fileType = node->getFileType();

    entry->length = node->getLength();

    return entry;
}

/**
 * Each consecutive call to this function returns a DirEntry-struct, representing the next child of this directory.
 * 
 * @return The next DirEntry-struct, or nullptr if there are none left.
 */
DirEntry *Directory::nextEntry() {

    String childName;

    String childPath;

    FileSystem *fileSystem = Kernel::getService<FileSystem>();

    if(pos == 0) {

        childName = ".";

        childPath = path;

    } else if(pos == 1) {

        DirEntry *entry = getInfo();

        childName = "..";

        childPath = path.substring(0, path.length() - entry->name.length());

        delete entry;

    } else {

        childName = node->getChild(pos - 2);

        if(childName.isEmpty()) {
            return nullptr;
        }

        childPath = path;

        if (!childPath.endsWith(FileSystem::SEPARATOR)) {
            childPath += FileSystem::SEPARATOR;
        }

        childPath += childName;
    }

    FsNode *child = fileSystem->getNode(childPath);

    DirEntry *entry = new DirEntry();

    entry->name = childName;

    entry->fullPath = path;

    if (!entry->fullPath.endsWith(FileSystem::SEPARATOR)) {
        entry->fullPath += FileSystem::SEPARATOR;
    }

    entry->fullPath += childName;

    entry->fileType = child->getFileType();

    entry->length = child->getLength();

    pos++;

    delete child;

    return entry;
}

/**
 * The variable 'pos' determines which child will be returned by the next call to 'nextEntry'.
 * This function can be used to set 'pos'.
 * 
 * @param The new value for 'pos'.
 */
void Directory::setPos(uint32_t pos) {
    this->pos = pos;
}