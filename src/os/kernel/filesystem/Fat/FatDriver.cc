#include "FatDriver.h"
#include "FatNode.h"

bool FatDriver::mount(StorageDevice *device) {
    this->device = device;
    fatInstance = new FatFs(device);

    if(fatInstance->f_mount(device, 1) == FR_OK)
        return 0;
    
    return -1;
}

bool FatDriver::makeFs(StorageDevice *device) {
    FatFs *tmpFat = new FatFs(device);

    uint8_t fatType = device->getSectorCount() <= MAX_FAT16 ? FM_FAT : FM_FAT32;
    int32_t ret = tmpFat->f_mkfs(fatType, 0, new char[4096], 4096);

    delete tmpFat;
    return ret;
}

FsNode *FatDriver::getNode(const String &path) {
    if(path.length() == 0 || path == "/")
        return FatNode::open("", fatInstance);

    FILINFO info;
    if(fatInstance->f_stat((char *) path, &info) != FR_OK)
        return nullptr;
    
    return FatNode::open(path, fatInstance);
}

bool FatDriver::createNode(const String &path, uint8_t fileType) {
    if(fileType == DIRECTORY_FILE) {
        if(fatInstance->f_mkdir((char *) path) == FR_OK)
            return 0;
    } else if(fileType == REGULAR_FILE) {
        FIL file;
        if(fatInstance->f_open(&file, (char *) path, FA_WRITE | FA_CREATE_ALWAYS) == FR_OK) {
            fatInstance->f_close(&file);
            return 0;
        }
    }

    return -1;
}

bool FatDriver::deleteNode(const String &path) {
    FILINFO info;
    if(fatInstance->f_stat((char *) path, &info) == FR_OK)
        if(fatInstance->f_unlink((char *) path) == FR_OK)
            return 0;

    return -1;
}
