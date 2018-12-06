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

#include <kernel/services/StorageService.h>
#include <kernel/Kernel.h>
#include <lib/file/Directory.h>
#include "StorageDevice.h"
#include "Partition.h"

StorageDevice::StorageDevice(String name) : name(name) {

}

String StorageDevice::getName() {
    return name;
}

Util::Array<StorageDevice::PartitionInfo> StorageDevice::readPartitionTable() {

    partLock.acquire();

    partitionList.clear();

    uint8_t mbr[getSectorSize()];
    if(!read(mbr, 0, 1)) {
        partLock.release();
        return partitionList.toArray();
    }

    // Check MBR-Signature
    uint16_t signature = *((uint16_t *)(&mbr[510]));
    if(signature != 0xaa55) {
        partLock.release();
        return partitionList.toArray();
    }

    auto *partitions = (PartitionTableEntry *) &mbr[PARTITON_TABLE_START];
    
    // Cycle through all four primary partitions
    for(uint8_t i = 0; i < 4; i++) {
        PartitionTableEntry currentPartition = partitions[i];

        // Check system ID
        // 0 --> Unused partiton
        if(currentPartition.system_id == EMPTY)
            continue;

        // 5 --> Extended partiton
        if(currentPartition.system_id == EXTENDED_PARTITION || currentPartition.system_id == EXTENDED_PARTITION_LBA) {
            uint8_t partNumber = 5;
            auto *currentPart = new StorageDevice::PartitionInfo{partNumber, 'e', currentPartition.active_flag == 0x80, currentPartition.system_id, currentPartition.relative_sector, currentPartition.sector_count};
            partitionList.add(*currentPart);
            uint32_t nextLogicalMbr = currentPartition.relative_sector;

            // An extended partition contains a linked list of logical partitions:
            // The first partition table entry of an extended partition's MBR contains contains information about the currrent logical partition.
            // The second partition table entry points to the next logical MBR which works exactly the same.
            while(true) {
                uint8_t logicalMbr[getSectorSize()];
                read(logicalMbr, nextLogicalMbr, 1);

                // Check MBR-Signature
                signature = *((uint16_t *)(&logicalMbr[510]));
                if(signature != 0xaa55)
                    break;

                PartitionTableEntry currentLogicalPartition = *((PartitionTableEntry *) &logicalMbr[PARTITON_TABLE_START]);
                PartitionTableEntry nextLogicalPartition = *((PartitionTableEntry *) &logicalMbr[PARTITON_TABLE_START + 0x10]);

                // Check system ID
                // 0 --> Unused partiton
                if(currentLogicalPartition.system_id == EMPTY)
                    break;

                currentPart = new StorageDevice::PartitionInfo{partNumber, 'l', currentLogicalPartition.active_flag == 0x80, currentLogicalPartition.system_id, nextLogicalMbr + currentLogicalPartition.relative_sector, currentLogicalPartition.sector_count};
                partitionList.add(*currentPart);

                // Check system ID
                // 0 --> Unused partiton
                if(nextLogicalPartition.system_id == EMPTY)
                    break;
                
                // Calculate address of the next MBR
                nextLogicalMbr = currentPartition.relative_sector + nextLogicalPartition.relative_sector;

                partNumber++;
            }
        } else {
            auto *currentPart = new StorageDevice::PartitionInfo{static_cast<uint8_t>(i + 1), 'p', currentPartition.active_flag == 0x80, currentPartition.system_id, currentPartition.relative_sector, currentPartition.sector_count};
            partitionList.add(*currentPart);
        }
    }

    partLock.release();
    return partitionList.toArray();
}

uint32_t StorageDevice::writePartition(uint8_t partNumber, bool active, uint8_t systemId, uint32_t startSector, uint32_t sectorCount) {
    if(partNumber < 1) {
        return NON_EXISTENT_PARITION;
    }

    partLock.acquire();

    uint8_t mbr[getSectorSize()];
    if(!read(mbr, 0, 1)) {
        partLock.release();
        return READ_SECTOR_FAILED;
    }

    // Check MBR-Signature
    uint16_t signature = *((uint16_t *)(&mbr[510]));
    if(signature != 0xaa55) {
        partLock.release();
        return INVALID_MBR_SIGNATURE;
    }

    PartitionTableEntry partEntry{
        (uint8_t)(active ? 0x80 : 0x00),    // 0x80 --> bootable partition, 0x0 --> non-bootable partition
        0,                                  // Starting head of the partition
        0,                                  // Bits 0-6: Starting sector of the partition, Bits 7-15: Starting cylinder of the partition
        systemId,                           // Partition type identifier
        0,                                  // Ending head of the partition
        0,                                  // Bits 0-6: Ending sector of the partition, Bits 7-15: Ending cylinder of the partition
        startSector,                        // Relative sector to start of partition
        sectorCount,                        // Amount of sectors in partition
    };

    if(partNumber <= 4) {
        // Primary partition
        auto *partPtr = (PartitionTableEntry *) &mbr[PARTITON_TABLE_START + 0x10 * (partNumber - 1)];

        if(systemId == 0x05 || systemId == 0x0f)  {
            // Initialize first logical mbr
            uint8_t logicalMbr[getSectorSize()];
            memset(logicalMbr, 0, getSectorSize());

            // Write mbr-signature
            *((uint16_t *)(&logicalMbr[510])) = 0xaa55;

            // Write first logical mbr
            if(!write(logicalMbr, startSector, 1)) {
                partLock.release();
                return WRITE_SECTOR_FAILED;
            }
        }

        // Write partition entry
        *partPtr = partEntry;
        if(!write(mbr, 0, 1)) {
            partLock.release();
            return WRITE_SECTOR_FAILED;
        }

        // Register the new partition in storage service
        Kernel::getService<StorageService>()->registerDevice(new Partition(this, startSector, sectorCount, systemId,
                String::format("%sp%u", static_cast<const char*>(getName()), partNumber)));

        partLock.release();
        return SUCCESS;
    } else {
        // Logical partition
        PartitionTableEntry extPart{};
        uint8_t extPartIndex;
        
        // Search for extended partition
        auto *partitions = (PartitionTableEntry *) &mbr[PARTITON_TABLE_START];
        for(extPartIndex = 0; extPartIndex < 4; extPartIndex++) {
            PartitionTableEntry currentPartition = partitions[extPartIndex];

            if(currentPartition.system_id == EXTENDED_PARTITION || currentPartition.system_id == EXTENDED_PARTITION_LBA) {
                extPart = currentPartition;
                break;
            }

            if(extPartIndex == 3) {
                partLock.release();
                return EXTENDED_PARTITION_NOT_FOUND;
            }
        }
        
        // Iterate through logical partitions to the desired partition number or until the end of the list
        uint8_t i;
        uint8_t currentMbr[getSectorSize()];
        uint32_t currentLogicalMbr = extPart.relative_sector;
        bool appendToList = false;

        for(i = 5; i <= partNumber; i++) {
            if(!read(currentMbr, currentLogicalMbr, 1)) {
                partLock.release();
                return READ_SECTOR_FAILED;
            }

            // Check MBR-Signature
            signature = *((uint16_t *)(&currentMbr[510]));
            if(signature != 0xaa55) {
                partLock.release();
                return INVALID_MBR_SIGNATURE;
            }

            PartitionTableEntry currentLogicalPartition = *((PartitionTableEntry *) &currentMbr[PARTITON_TABLE_START]);
            PartitionTableEntry nextLogicalPartition = *((PartitionTableEntry *) &currentMbr[PARTITON_TABLE_START + 0x10]);
            
            if(currentLogicalPartition.system_id == EMPTY || i == partNumber) {
                break;
            }
            
            if(nextLogicalPartition.system_id == EMPTY) {
                appendToList = true;
                break;
            }

            // Calculate address of the next MBR
            currentLogicalMbr = extPart.relative_sector + nextLogicalPartition.relative_sector;
        }

        if(appendToList) {
            // Append a new partition to the list
            partEntry.relative_sector = startSector - extPart.relative_sector;

            auto *partPtr = (PartitionTableEntry *) &currentMbr[PARTITON_TABLE_START + 0x10];
            *partPtr = partEntry;
            partPtr->system_id = EXTENDED_PARTITION_LBA;
            if(!write(currentMbr, currentLogicalMbr, 1)) {
                partLock.release();
                return WRITE_SECTOR_FAILED;
            }
            
            uint8_t newLogicalMbr[getSectorSize()];
            memset(newLogicalMbr, 0, getSectorSize());
            partEntry.relative_sector = 1;
            partEntry.sector_count -= 1;

            partPtr = (PartitionTableEntry *) &newLogicalMbr[PARTITON_TABLE_START];
            *partPtr = partEntry;
            *((uint16_t *)(&newLogicalMbr[510])) = 0xaa55;

            if(!write(newLogicalMbr, startSector, 1)) {
                partLock.release();
                return WRITE_SECTOR_FAILED;
            }

            // Register the new partition in storage service
            Kernel::getService<StorageService>()->registerDevice(new Partition(this, startSector, sectorCount, systemId,
                    String::format("%sp%u", static_cast<const char*>(getName()), i)));

            partLock.release();
            return SUCCESS;
        } else {
            // Edit an existing partition
            auto *partPtr = (PartitionTableEntry *) &currentMbr[PARTITON_TABLE_START];
            partPtr->relative_sector = partPtr->relative_sector == 0 ? 1 : partPtr->relative_sector;
            partPtr->sector_count = sectorCount;
            partPtr->system_id = systemId;
            partPtr->active_flag = static_cast<uint8_t>(active ? 0x80 : 0x00);

            // Write partition entry
            if(!write(currentMbr, currentLogicalMbr, 1)) {
                partLock.release();
                return WRITE_SECTOR_FAILED;
            }

            // Register the new partition in storage service
            Kernel::getService<StorageService>()->registerDevice(new Partition(this, startSector, sectorCount, systemId,
                    String::format("%sp%u", static_cast<const char*>(getName()), i)));

            partLock.release();
            return SUCCESS;
        }
    }
}

uint32_t StorageDevice::deletePartition(uint8_t partNumber) {
    if(partNumber < 1) {
        return NON_EXISTENT_PARITION;
    }

    partLock.acquire();

    uint8_t mbr[getSectorSize()];
    if(!read(mbr, 0, 1)) {
        partLock.release();
        return WRITE_SECTOR_FAILED;
    }

    PartitionTableEntry partEntry{0, 0, 0, 0, 0, 0, 0, 0};

    if(partNumber <= 4) {
        // Primary parition
        auto *partPtr = (PartitionTableEntry *) &mbr[PARTITON_TABLE_START + 0x10 * (partNumber - 1)];

        bool extendedPartition = false;

        if(partPtr->system_id == EXTENDED_PARTITION || partPtr->system_id == EXTENDED_PARTITION_LBA) {
            extendedPartition = true;
        }
        
        // Write partition table
        *partPtr = partEntry;
        if(!write(mbr, 0, 1)) {
            partLock.release();
            return WRITE_SECTOR_FAILED;
        }

        // Delete the partition from storage service
        Kernel::getService<StorageService>()->removeDevice(String::format("%sp%u", static_cast<const char*>(getName()), partNumber));

        if(extendedPartition) {
            // Delete all logical partitions on this device from storage service
            Directory &dir = *Directory::open("/dev/storage");
            auto *storageService = Kernel::getService<StorageService>();

            for (const String &name : dir.getChildren()) {
                if (name != getName() && name.beginsWith(getName())) {
                    String numberString = name.substring(getName().length() + 1, name.length());

                    if(strtoint(static_cast<const char*>(numberString)) > 4) {
                        storageService->removeDevice(name);
                    }
                }
            }
        }

        partLock.release();
        return SUCCESS;
    }
    
    // Logical partition
    PartitionTableEntry extPart{};
    uint8_t extPartIndex;
    
    // Search for extended partition
    auto *partitions = (PartitionTableEntry *) &mbr[PARTITON_TABLE_START];
    for(extPartIndex = 0; extPartIndex < 4; extPartIndex++) {
        PartitionTableEntry currentPartition = partitions[extPartIndex];

        if(currentPartition.system_id == EXTENDED_PARTITION || currentPartition.system_id == EXTENDED_PARTITION_LBA) {
            extPart = currentPartition;
            break;
        }

        if(extPartIndex == 3) {
            partLock.release();
            return EXTENDED_PARTITION_NOT_FOUND;
        }
    }

    if(partNumber == 5) {
        // Special case for first logical partition
        uint8_t firstLogicalMbr[getSectorSize()];
        if(!read(firstLogicalMbr, extPart.relative_sector, 1)) {
            partLock.release();
            return READ_SECTOR_FAILED;
        }
        
        PartitionTableEntry *firstMbrFirstLogicalPartition = ((PartitionTableEntry *) &firstLogicalMbr[PARTITON_TABLE_START]);
        PartitionTableEntry *firstMbrSecondLogicalPartition = ((PartitionTableEntry *) &firstLogicalMbr[PARTITON_TABLE_START + 0x10]);
        
        if(firstMbrSecondLogicalPartition->system_id == EMPTY) {
            // The first logical partition is the only one
            memset(firstMbrFirstLogicalPartition, 0, sizeof(PartitionTableEntry));
        } else {
            // There is more than one logical partition
            // We need to read the second one and let the linked list start with it
            uint8_t secondLogicalMbr[getSectorSize()];
            if(!read(secondLogicalMbr, extPart.relative_sector + firstMbrSecondLogicalPartition->relative_sector, 1)) {
                partLock.release();
                return READ_SECTOR_FAILED;
            }

            PartitionTableEntry *secondMbrFirstLogicalPartition = ((PartitionTableEntry *) &secondLogicalMbr[PARTITON_TABLE_START]);
            PartitionTableEntry *secondMbrSecondLogicalPartition = ((PartitionTableEntry *) &secondLogicalMbr[PARTITON_TABLE_START + 0x10]);

            firstMbrFirstLogicalPartition->relative_sector = firstMbrSecondLogicalPartition->relative_sector + secondMbrFirstLogicalPartition->relative_sector;
            firstMbrFirstLogicalPartition->sector_count = secondMbrFirstLogicalPartition->sector_count;
            *firstMbrSecondLogicalPartition = *secondMbrSecondLogicalPartition;
        }

        if(!write(firstLogicalMbr, extPart.relative_sector, 1)) {
            partLock.release();
            return WRITE_SECTOR_FAILED;
        }

        // Delete the partition from storage service
        Kernel::getService<StorageService>()->removeDevice(String::format("%sp%u", static_cast<const char*>(getName()), partNumber));

        partLock.release();
        return SUCCESS;
    }

    // Iterate through logical partitions to the desired partition number or until the end of the list
    uint8_t currentMbr[getSectorSize()];
    uint32_t lastLogicalMbr = 0;
    uint32_t currentLogicalMbr = extPart.relative_sector;
    PartitionTableEntry currentLogicalPartition{};
    PartitionTableEntry nextLogicalPartition{};

    for(uint8_t i = 5; i <= partNumber; i++) {
        if(!read(currentMbr, currentLogicalMbr, 1)) {
            partLock.release();
            return READ_SECTOR_FAILED;
        }

        // Check MBR-Signature
        uint16_t signature = *((uint16_t *)(&currentMbr[510]));
        if(signature != 0xaa55) {
            partLock.release();
            return INVALID_MBR_SIGNATURE;
        }

        currentLogicalPartition = *((PartitionTableEntry *) &currentMbr[PARTITON_TABLE_START]);
        nextLogicalPartition = *((PartitionTableEntry *) &currentMbr[PARTITON_TABLE_START + 0x10]);

        // Check system ID
        // 0 --> Unused partition
        if(currentLogicalPartition.system_id == EMPTY) {
            partLock.release();
            return UNUSED_PARTITION;
        }
        
        if(nextLogicalPartition.system_id == EMPTY || i == partNumber) {
            if(i < partNumber) {
                partLock.release();
                return NON_EXISTENT_PARITION;
            }

            break;
        }

        // Calculate address of the next MBR
        lastLogicalMbr = currentLogicalMbr;
        currentLogicalMbr = extPart.relative_sector + nextLogicalPartition.relative_sector;
    }

    uint8_t lastMbr[getSectorSize()];
    if(!read(lastMbr, lastLogicalMbr, 1)) {
        partLock.release();
        return READ_SECTOR_FAILED;
    }

    // Delete partition
    auto *partPtr = (PartitionTableEntry *) &currentMbr[PARTITON_TABLE_START];
    *partPtr = partEntry;
    partPtr = (PartitionTableEntry *) &currentMbr[PARTITON_TABLE_START + 0x10];
    *partPtr = partEntry;

    // Let predecessor point to successor
    partPtr = (PartitionTableEntry *) &lastMbr[PARTITON_TABLE_START + 0x10];
    *partPtr = nextLogicalPartition;
    
    if(!write(currentMbr, currentLogicalMbr, 1) || !write(lastMbr, lastLogicalMbr, 1)) {
        partLock.release();
        return WRITE_SECTOR_FAILED;
    }

    // Delete the partition from storage service
    Kernel::getService<StorageService>()->removeDevice(String::format("%sp%u", static_cast<const char*>(getName()), partNumber));

    partLock.release();
    return SUCCESS;
}


uint32_t StorageDevice::createPartitionTable() {

    partLock.acquire();

    uint8_t mbr[getSectorSize()];

    // Create new empty MBR
    memset(mbr, 0, 510);

    // Write MBR-signature
    *((uint16_t *)(&mbr[510])) = 0xaa55;

    if(!write(mbr, 0, 1)) {
        partLock.release();
        return WRITE_SECTOR_FAILED;
    }

    // Remove all partitions on this device from storage service
    Directory &dir = *Directory::open("/dev/storage");
    auto *storageService = Kernel::getService<StorageService>();

    for (const String &name : dir.getChildren()) {
        if (name != getName() && name.beginsWith(getName())) {
            storageService->removeDevice(name);
        }
    }

    partLock.release();
    return SUCCESS;
}

uint8_t StorageDevice::getSystemId() {
    return 0;
}
