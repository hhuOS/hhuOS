#include "FatNode.h"


extern "C" {
#include "lib/libc/string.h"
}

FatNode *FatNode::open(const String &path, FatFs *fatInstance) {

    if(path.length() > 4095) {
        return nullptr;
    }

    FatNode *node = new FatNode(fatInstance);

    node->path = path;

    if(fatInstance->f_stat((char*) path, &node->info) != FR_OK) {
        if(fatInstance->f_opendir(&node->fatObject.dir, (char*) path) != FR_OK) {
            delete node;
            return nullptr;
        } else {
            memset(&node->info, 0, sizeof(FILINFO));
            strcpy(node->info.fname, "/");
            strcpy(node->info.altname, "/");
            node->info.fattrib = 20;
            return node;
        }
    }
            
    if(node->info.fattrib & AM_DIR) {
        if(fatInstance->f_opendir(&node->fatObject.dir, (char*) path) == FR_OK)
            return node;
    }

    if(!(node->info.fattrib & AM_DIR)) {
        if(fatInstance->f_open(&node->fatObject.file, (char*) path, FA_READ | FA_WRITE) == FR_OK)
            return node;
    }
    
    delete node;
    return nullptr;
}

String FatNode::getName() {
    return info.fname;
}

uint8_t FatNode::getFileType() {
    if(info.fattrib & AM_DIR)
        return DIRECTORY_FILE;
    
    return REGULAR_FILE;
}

uint64_t FatNode::getLength() {

    if(fatInstance->f_stat((char*) path, &info) != FR_OK) {
        return 0;
    }

    return info.fsize;
}

String FatNode::getChild(uint32_t pos) {
    if(!(info.fattrib & AM_DIR))
        return nullptr;

    fatInstance->f_readdir(&fatObject.dir, nullptr);
    
    FILINFO childInfo;
    for(uint32_t i = 0; i <= pos; i++) {
        if(fatInstance->f_readdir(&fatObject.dir, &childInfo) != FR_OK)
            return nullptr;

        if(childInfo.fname[0] == 0)
            return nullptr;
    }

    char *ret = new char[strlen(childInfo.fname) + 1];
    strcpy(ret, childInfo.fname);

    return ret;
}

char *FatNode::readData(char *buf, uint64_t pos, uint32_t numBytes) {
    if(info.fattrib & AM_DIR)
        return nullptr;
    
    if(fatInstance->f_lseek(&fatObject.file, pos) != FR_OK)
        return nullptr;
    
    if(f_eof(&fatObject.file)) {
        buf[0] = VFS_EOF;
        return buf;
    }

    uint32_t readBytes;
    if(fatInstance->f_read(&fatObject.file, buf, numBytes, &readBytes) != FR_OK)
        return nullptr;

    if(readBytes < numBytes && f_eof(&fatObject.file))
        buf[numBytes] = VFS_EOF;
        
    return buf;
}

int32_t FatNode::writeData(char *buf, uint64_t pos, uint32_t numBytes) {
    if(info.fattrib & AM_DIR)
        return -1;

    if(fatInstance->f_lseek(&fatObject.file, pos) != FR_OK)
        return -1;

    uint32_t writtenBytes;
    if(fatInstance->f_write(&fatObject.file, buf, numBytes, &writtenBytes) != FR_OK)
        return -1;
        
    fatInstance->f_sync(&fatObject.file);
    return numBytes;
}