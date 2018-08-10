/*
 * Copyright (C) 2018 Burak Akguel, Christian Gesse, Fabian Ruhland, Filip Krakowski, Michael Schoettner
 * Heinrich-Heine University
 *
 * This program is free software: you can redistribute it and/or modify it under the terms of the GNU General Public
 * License as published by the Free Software Foundation, either version 3 of the License, or (at your option) any
 * later version.
 *
 * This program is distributed in the hope that it will be useful, but WITHOUT ANY WARRANTY; without even the implied
 * warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU General Public License for more
 * details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program.  If not, see <http://www.gnu.org/licenses/>
 */

#include "FatNode.h"

FatNode::FatNode(FatFs *fatInstance) : fatInstance(fatInstance) {

}

FatNode::~FatNode() {
    if(info.fattrib & AM_DIR) {
        fatInstance->f_closedir(&fatObject.dir);
    } else if(!(info.fattrib & AM_DIR)) {
        fatInstance->f_close(&fatObject.file);
    }
}

FatNode *FatNode::open(const String &path, FatFs *fatInstance) {
    auto *node = new FatNode(fatInstance);

    node->path = path;

    // Try to stat the file. If this fails, the file is either non-existent,
    // or it may be the root-directory (f_stat will fail, when executed on the root-directory).
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
        if(fatInstance->f_opendir(&node->fatObject.dir, (char*) path) == FR_OK) {
            return node;
        }
    }

    if(!(node->info.fattrib & AM_DIR)) {
        if(fatInstance->f_open(&node->fatObject.file, (char*) path, FA_READ | FA_WRITE) == FR_OK) {
            return node;
        }
    }
    
    delete node;
    return nullptr;
}

String FatNode::getName() {
    return info.fname;
}

uint8_t FatNode::getFileType() {
    if(info.fattrib & AM_DIR) {
        return DIRECTORY_FILE;
    }
    
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
        buf[readBytes] = END_OF_FILE;
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
