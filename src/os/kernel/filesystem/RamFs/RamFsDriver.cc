#include <kernel/services/FileSystem.h>
#include <lib/util/List.h>
#include "RamFsDriver.h"
#include "RamFsNode.h"

extern "C" {
    #include "lib/libc/string.h"
}

//char **tokenizePath(const String &path, uint32_t &count) {
//    count = 0;
//    char *ptr = strtok(path, "/");
//    while(ptr != nullptr) {
//        count++;
//        ptr = strtok(nullptr, "/");
//    }
//
//    char **ret = new char *[count];
//
//    ptr = strtok(path, "/");
//    for(uint32_t i = 0; i < count; i++) {
//        ret[i] = new char[strlen(ptr) + 1];
//        strcpy(ret[i], ptr);
//        ptr = strtok(nullptr, "/");
//    }
//
//    return ret;
//}
//
//void freeTokenizedPath(char **parsedPath, uint32_t count) {
//    for(uint32_t i = 0; i < count; i++) {
//        delete parsedPath[i];
//    }
//
//    delete parsedPath;
//}

VirtualNode *getChildByName(VirtualNode *parent, const String &path) {

    LinkedList<VirtualNode> *children = parent->getChildren();

    for(uint32_t i = 0; i < children->length(); i++) {

        if(children->get(i)->getName() == path) {
            return parent->getChildren()->get(i);
        }
    }
    
    return nullptr;
}

int32_t RamFsDriver::mount(StorageDevice *disk) {
    rootNode = new VirtualNode("/", DIRECTORY_FILE);
    
    return 0;
}

int32_t RamFsDriver::makeFs(StorageDevice *device) {
    return 0;
}

FsNode *RamFsDriver::getNode(const String &path) {

    if(path.length() == 0){
        return new RamFsNode(rootNode);
    }

    Util::Array<String> token = path.split(FileSystem::SEPARATOR);

    VirtualNode *currentDir = rootNode;
    
    if(token.length() == 0) {
        return new RamFsNode(rootNode);
    }
    
    for(uint32_t i = 0; i < token.length() - 1; i++) {

        currentDir = getChildByName(currentDir, token[i]);

        if(currentDir == nullptr || currentDir->getFileType() != DIRECTORY_FILE) {
            return nullptr;
        }
    }
    
    VirtualNode *ret = getChildByName(currentDir, token[token.length() - 1]);

    if(ret == nullptr) {
        return nullptr;
    }

    return new RamFsNode(ret);
}

int32_t RamFsDriver::addNode(const String &path, VirtualNode *node) {

    Util::Array<String> token = path.split(FileSystem::SEPARATOR);

    VirtualNode *currentDir = rootNode;
    
    for(uint32_t i = 0; i < token.length(); i++) {

        currentDir = getChildByName(currentDir, token[i]);

        if(currentDir == nullptr || currentDir->getFileType() != DIRECTORY_FILE) {

            return -1;
        }
    }

    if(getChildByName(currentDir, node->getName()) != nullptr) {
        return -1;
    }

    currentDir->getChildren()->add(node);
    return 0;
}

int32_t RamFsDriver::createNode(const String &path, uint8_t fileType) {

    Util::Array<String> token = path.split(FileSystem::SEPARATOR);

    VirtualNode *currentDir = rootNode;

    if(path.length() > 0 && token.length() == 0) {
        return -1;
    }
    
    for(uint32_t i = 0; i < token.length() - 1; i++) {

        currentDir = getChildByName(currentDir, token[i]);

        if(currentDir == nullptr || currentDir->getFileType() != DIRECTORY_FILE) {

            return -1;
        }
    }

    if (token[token.length() - 1].length() > 255) {
        return -1;
    }

    if(getChildByName(currentDir, token[token.length() - 1]) != nullptr) {
        return -1;
    }

    VirtualNode *newNode = new VirtualNode(token[token.length() - 1], fileType);

    currentDir->getChildren()->add(newNode);

    return 0;
}

int32_t RamFsDriver::deleteNode(const String &path) {

    Util::Array<String> token = path.split(FileSystem::SEPARATOR);

    VirtualNode *currentDir = rootNode;
    
    if(path.length() > 0 && token.length() == 0) {
        return -1;
    }
    
    for(uint32_t i = 0; i < token.length() - 1; i++) {

        currentDir = getChildByName(currentDir, token[i]);

        if(currentDir == nullptr || currentDir->getFileType() != DIRECTORY_FILE) {

            return -1;
        }
    }
    
    VirtualNode *node = getChildByName(currentDir, token[token.length() - 1]);

    if(node == nullptr) {
        return -1;
    }

    currentDir->getChildren()->remove(node);

    delete node;

    return 0;
}
