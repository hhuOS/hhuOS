/*
 * Copyright (C) 2018-2022 Heinrich-Heine-Universitaet Duesseldorf,
 * Institute of Computer Science, Department Operating Systems
 * Burak Akguel, Christian Gesse, Fabian Ruhland, Filip Krakowski, Michael Schoettner
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

#include "kernel/log/Logger.h"
#include "filesystem/fat/FatDriver.h"
#include "filesystem/fat/ff/source/diskio.h"

static Kernel::Logger log = Kernel::Logger::get("FAT");
static const constexpr DWORD DEFAULT_BLOCK_SIZE = 1;

DSTATUS disk_status(BYTE driveNumber) {
    return RES_OK;
}

DSTATUS disk_initialize(BYTE driveNumber) {
    return RES_OK;
}

DRESULT disk_read(BYTE driveNumber, BYTE *buffer, LBA_t startSector, UINT sectorCount) {
    auto &device = Filesystem::Fat::FatDriver::getStorageDevice(driveNumber);
	auto result = device.read(buffer, startSector, sectorCount);

    return result == sectorCount ? RES_OK : RES_ERROR;
}

#if FF_FS_READONLY == 0

DRESULT disk_write(BYTE driveNumber, const BYTE *buffer, LBA_t startSector, UINT sectorCount) {
    auto &device = Filesystem::Fat::FatDriver::getStorageDevice(driveNumber);
    auto result = device.write(buffer, startSector, sectorCount);

    return result == sectorCount ? RES_OK : RES_ERROR;
}

#endif

DRESULT disk_ioctl(BYTE driveNumber, BYTE command, void *buffer) {
    auto &device = Filesystem::Fat::FatDriver::getStorageDevice(driveNumber);
    switch (command) {
        case CTRL_SYNC:
            return RES_OK;
        case GET_SECTOR_COUNT: {
            auto *lba = reinterpret_cast<LBA_t *>(buffer);
            *lba = device.getSectorCount();
            return RES_OK;
        }
        case GET_SECTOR_SIZE: {
            auto *size = reinterpret_cast<WORD *>(buffer);
            *size = device.getSectorSize();
            return RES_OK;
        }
        case GET_BLOCK_SIZE: {
            auto *size = reinterpret_cast<WORD *>(buffer);
            *size = DEFAULT_BLOCK_SIZE;
            return RES_OK;
        }
        case CTRL_TRIM:
        default:
            return RES_PARERR;
    }
}