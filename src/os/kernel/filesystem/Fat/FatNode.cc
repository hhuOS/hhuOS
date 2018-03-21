#include <kernel/filesystem/Fat/FatFsLib/FatFs.h>
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

Util::Array<String> FatNode::getChildren() {
    if(!(info.fattrib & AM_DIR)) {
        return Util::Array<String>(0);
    }

    uint32_t childCount = 0;

    FILINFO childInfo{};

    while(true) {
        FRESULT res = fatInstance->f_readdir(&fatObject.dir, &childInfo);

        if(res != FR_OK || childInfo.fname[0] == 0) {
            break;
        }

        childCount++;
    }

    Util::Array<String> ret(childCount);

    fatInstance->f_readdir(&fatObject.dir, nullptr);

    for(uint32_t i = 0; i < childCount; i++) {
        FRESULT res = fatInstance->f_readdir(&fatObject.dir, &childInfo);

        if(res != FR_OK || childInfo.fname[0] == 0) {
            break;
        }

        ret[i] = childInfo.fname;
    }

    fatInstance->f_readdir(&fatObject.dir, nullptr);

    return ret;
}

uint64_t FatNode::readData(char *buf, uint64_t pos, uint64_t numBytes) {
    if(info.fattrib & AM_DIR) {
        return 0;
    }
    
    if(fatInstance->f_lseek(&fatObject.file, pos) != FR_OK) {
        return 0;
    }

    uint32_t readBytes;
    if(fatInstance->f_read(&fatObject.file, buf, static_cast<UINT>(numBytes), &readBytes) != FR_OK) {
        return 0;
    }

    if(readBytes < numBytes) {
        buf[readBytes] = VFS_EOF;
    }
        
    return readBytes;
}

uint64_t FatNode::writeData(char *buf, uint64_t pos, uint64_t numBytes) {
    if(info.fattrib & AM_DIR) {
        return 0;
    }

    if(fatInstance->f_lseek(&fatObject.file, pos) != FR_OK) {
        return 0;
    }

    uint32_t writtenBytes;
    if(fatInstance->f_write(&fatObject.file, buf, static_cast<UINT>(numBytes), &writtenBytes) != FR_OK) {
        return 0;
    }
        
    fatInstance->f_sync(&fatObject.file);
    return writtenBytes;
}