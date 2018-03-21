#include "FatDriver.h"
#include "FatNode.h"

FatDriver::~FatDriver() {
    if(fatInstance != nullptr) {
        delete fatInstance;
    }
}

bool FatDriver::mount(StorageDevice *device) {
    this->device = device;
    fatInstance = new FatFs(device);

    return fatInstance->f_mount(device, 1) == FR_OK;

}

bool FatDriver::createFs(StorageDevice *device) {
    auto *tmpFat = new FatFs(device);

    auto fatType = static_cast<uint8_t>(device->getSectorCount() <= MAX_FAT16 ? FM_FAT : FM_FAT32);
    bool ret = tmpFat->f_mkfs(fatType, 0, new char[4096], 4096) == FR_OK;

    delete tmpFat;
    return ret;
}

FsNode *FatDriver::getNode(const String &path) {
    if(path.length() == 0 || path == "/")
        return FatNode::open("", fatInstance);

    FILINFO info{};
    if(fatInstance->f_stat((char *) path, &info) != FR_OK)
        return nullptr;
    
    return FatNode::open(path, fatInstance);
}

bool FatDriver::createNode(const String &path, uint8_t fileType) {
    if(fileType == FsNode::DIRECTORY_FILE) {
        if(fatInstance->f_mkdir((char *) path) == FR_OK) {
            return true;
        }
    } else if(fileType == FsNode::REGULAR_FILE) {
        FIL file{};
        if(fatInstance->f_open(&file, (char *) path, FA_WRITE | FA_CREATE_ALWAYS) == FR_OK) {
            fatInstance->f_close(&file);
            return true;
        }
    }

    return false;
}

bool FatDriver::deleteNode(const String &path) {
    FILINFO info{};
    if(fatInstance->f_stat((char *) path, &info) == FR_OK) {
        if (fatInstance->f_unlink((char *) path) == FR_OK) {
            return true;
        }
    }

    return false;
}
