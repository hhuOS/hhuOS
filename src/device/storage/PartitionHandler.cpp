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

#include "PartitionHandler.h"

#include "lib/util/memory/Address.h"
#include "lib/util/data/ArrayList.h"
#include "device/storage/StorageDevice.h"
#include "kernel/log/Logger.h"
#include "lib/util/Exception.h"
#include "lib/util/data/Collection.h"
#include "lib/util/data/Iterator.h"

namespace Device::Storage {

Kernel::Logger PartitionHandler::log = Kernel::Logger::get("PartitionHandler");

PartitionHandler::PartitionHandler(StorageDevice &device) : device(device) {
    if (device.getSectorSize() < 512) {
        Util::Exception::throwException(Util::Exception::INVALID_ARGUMENT, "PartitionHandler: Sector size must be at least 512 bytes!");
    }
}

void PartitionHandler::createPartitionTable() {
    createBootRecord(0);
}

Util::Data::Array<PartitionHandler::PartitionInfo> PartitionHandler::readPartitionTable() {
    // Read MBR
    auto *mbr = readBootRecord(0);
    if (mbr == nullptr) {
        return Util::Data::Array<PartitionInfo>(0);
    }

    auto *partitions = reinterpret_cast<PartitionTableEntry*>(&mbr[PARTITION_TABLE_START]);

    // Stores information about found partitions
    auto partitionList = Util::Data::ArrayList<PartitionInfo>();

    // Cycle through all four primary partitions
    for (uint32_t i = 0; i < 4; i++) {
        const auto &currentPrimaryPartition = partitions[i];

        // Check system ID
        // 0x00 --> Unused partition
        if (currentPrimaryPartition.systemId == EMPTY) {
            continue;
        }

        // Add partition to the list
        PartitionInfo info = {
                static_cast<uint8_t>(i + 1),
                currentPrimaryPartition.systemId == SystemId::EXTENDED_PARTITION || currentPrimaryPartition.systemId == SystemId::EXTENDED_PARTITION_LBA ? EXTENDED : PRIMARY,
                currentPrimaryPartition.activeFlag == 0x80,
                currentPrimaryPartition.systemId,
                currentPrimaryPartition.relativeSector,
                currentPrimaryPartition.sectorCount
        };
        partitionList.add(info);

        log.debug("Partition found (Number: [%u], type: [%c], Active: [%B], System ID: [0x%02x], Sector: [%u], Count: [%u])",
                  info.number, info.type, info.active, info.systemId, info.startSector, info.sectorCount);

        // 0x05 or 0x0f --> Extended currentPrimaryPartition
        if (currentPrimaryPartition.systemId == EXTENDED_PARTITION || currentPrimaryPartition.systemId == EXTENDED_PARTITION_LBA) {
            // Extended partition numbers start after primary partition number (1-4)
            uint8_t partitionNumber = 5;
            uint32_t ebrSector = currentPrimaryPartition.relativeSector;

            // An extended partition contains a linked list of logical partitions.
            // Each partition is preceded by its own boot record, called an 'Extended Boot Record' (EBR), which has two entries:
            // 1. The first entry contains information about the current logical partition.
            // 2. The second entry points to the next EBR which works exactly the same.
            while (true) {
                // Read EBR
                auto *ebr = readBootRecord(ebrSector);
                auto &currentLogicalPartition = *reinterpret_cast<PartitionTableEntry*>(&ebr[PARTITION_TABLE_START]);
                auto &nextLogicalPartition = *reinterpret_cast<PartitionTableEntry*>(&ebr[PARTITION_TABLE_START + sizeof(PartitionTableEntry)]);

                // Check system ID of current partition
                // 0 --> Unused partition (End of linked list has been reached)
                if (currentLogicalPartition.systemId == EMPTY) {
                    delete[] ebr;
                    break;
                }

                // Add partition to the list
                info = {
                        partitionNumber,
                        LOGICAL,
                        currentLogicalPartition.activeFlag == 0x80,
                        currentLogicalPartition.systemId,
                        ebrSector + currentLogicalPartition.relativeSector,
                        currentLogicalPartition.sectorCount
                };
                partitionList.add(info);

                log.debug("Partition found (Number: [%u], type: [%c], Active: [%B], System ID: [0x%02x], Sector: [%u], Count: [%u])",
                          info.number, info.type, info.active, info.systemId, info.startSector, info.sectorCount);

                // Check system ID of next partition
                // 0 --> Unused partition (End of linked list has been reached)
                if (nextLogicalPartition.systemId == EMPTY) {
                    log.debug("End of list reached");
                    delete[] ebr;
                    break;
                }

                ebrSector = currentPrimaryPartition.relativeSector + nextLogicalPartition.relativeSector;
                partitionNumber++;
                delete[] ebr;
            }
        }
    }

    delete[] mbr;
    return partitionList.toArray();
}

void PartitionHandler::writePartition(uint8_t partitionNumber, bool active, SystemId systemId, uint32_t startSector, uint32_t sectorCount) {
    if (partitionNumber < 1) {
        Util::Exception::throwException(Util::Exception::INVALID_ARGUMENT, "PartitionHandler: Partition number must be larger than 0!");
    }

    // Read MBR
    auto *mbr = readBootRecord(0);
    PartitionTableEntry entry = {
            static_cast<uint8_t>(active ? 0x80 : 0x00),
            0, 0, static_cast<uint8_t>(systemId),
            0,
            0,
            startSector,
            sectorCount
    };

    if (partitionNumber <= 4) {
        log.debug("Writing primary partition (Number: [%u], Active: [%B], System ID: [0x%02x], Sector: [%u], Count: [%u])", partitionNumber, active, systemId, startSector, sectorCount);
        auto *targetEntry = reinterpret_cast<PartitionTableEntry*>(&mbr[PARTITION_TABLE_START + sizeof(PartitionTableEntry) * (partitionNumber - 1)]);

        if (systemId == EXTENDED_PARTITION || systemId == EXTENDED_PARTITION_LBA) {
            // Initialize first EBR
            log.debug("Initializing first extended boot record");
            createBootRecord(startSector);
        }

        // Write partition entry
        *targetEntry = entry;
        uint32_t writtenSectors = device.write(mbr, 0, 1);
        delete[] mbr;
        if (writtenSectors < 1) {
            Util::Exception::throwException(Util::Exception::ILLEGAL_STATE, "PartitionHandler: Unable to write boot record to disk!");
        }
    } else {
        log.debug("Writing logical partition");
        PartitionTableEntry extendedPartition{};
        auto *partitions = reinterpret_cast<PartitionTableEntry*>(&mbr[PARTITION_TABLE_START]);

        // Search extended partition
        for (uint32_t i = 0; i < 4; i++) {
            auto &partition = partitions[i];
            if (partition.systemId == EXTENDED_PARTITION || partition.systemId == EXTENDED_PARTITION_LBA) {
                extendedPartition = partition;
                break;
            }
        }

        // No longer needed
        delete[] mbr;

        if (extendedPartition.systemId == EMPTY) {
            Util::Exception::throwException(Util::Exception::INVALID_ARGUMENT, "PartitionHandler: Trying to add a logical partition, but there is no extended partition!");
        }

        // Iterate through logical partitions to the desired partition number or the end of the list
        bool append = false;
        uint8_t *ebr;
        uint32_t ebrSector = extendedPartition.relativeSector;
        for (uint32_t i = 5; i <= partitionNumber; i++) {
            ebr = readBootRecord(ebrSector);
            auto &currentLogicalPartition = *reinterpret_cast<PartitionTableEntry*>(&ebr[PARTITION_TABLE_START]);
            auto &nextLogicalPartition = *reinterpret_cast<PartitionTableEntry*>(&ebr[PARTITION_TABLE_START + sizeof(PartitionTableEntry)]);

            if(currentLogicalPartition.systemId == EMPTY || i == partitionNumber) {
                break;
            }

            if(nextLogicalPartition.systemId == EMPTY) {
                append = true;
                break;
            }

            ebrSector = extendedPartition.relativeSector + nextLogicalPartition.relativeSector;
            delete[] ebr;
        }

        if (append) {
            // Append a new partition to the list
            entry.relativeSector = startSector - extendedPartition.relativeSector;
            auto *partition = reinterpret_cast<PartitionTableEntry*>(&ebr[PARTITION_TABLE_START + sizeof(PartitionTableEntry)]);
            *partition = entry;

            log.debug("Appending new logical partition (Number: [%u], Active: [%B], System ID: [0x%02x], Sector: [%u], Count: [%u])", partitionNumber, active, systemId, startSector, sectorCount);

            uint32_t writtenSectors = device.write(ebr, ebrSector, 1);
            delete[] ebr;
            if (writtenSectors < 1) {
                Util::Exception::throwException(Util::Exception::ILLEGAL_STATE, "PartitionHandler: Unable to write boot record to disk!");
            }

            // Let next entry of current boot record point to the boot record of the new partition
            auto *newEbr = new uint8_t[device.getSectorSize()];
            auto newEbrAddress = Util::Memory::Address<uint32_t>(newEbr);

            // Create new boot record for the new partition
            newEbrAddress.setRange(0, device.getSectorSize());

            // Set boot record signature
            newEbrAddress.setShort(BOOT_RECORD_SIGNATURE, 510);

            // Set start sector and sector count
            entry.relativeSector = 1;
            entry.sectorCount -= 1;

            partition = reinterpret_cast<PartitionTableEntry*>(&newEbr[PARTITION_TABLE_START]);
            *partition = entry;

            // Write boot record to disk
            writtenSectors = device.write(newEbr, startSector, 1) == device.getSectorSize();
            delete[] newEbr;
            if (writtenSectors < 1) {
                Util::Exception::throwException(Util::Exception::ILLEGAL_STATE, "PartitionHandler: Unable to write boot record to disk!");
            }
        } else {
            // Update an existing logical partition
            auto &partition = *reinterpret_cast<PartitionTableEntry*>(&ebr[PARTITION_TABLE_START]);
            partition.relativeSector = startSector - ebrSector;
            partition.sectorCount = sectorCount;
            partition.systemId = systemId;
            partition.activeFlag = static_cast<uint8_t>(active ? 0x80 : 0x00);

            log.debug("Updating existing logical partition (Number: [%u], Active: [%B], System ID: [0x%02x], Sector: [%u], Count: [%u])", partitionNumber, active, systemId, startSector, sectorCount);

            // Write boot record to disk
            auto writtenSectors = device.write(ebr, ebrSector, 1) == device.getSectorSize();
            delete[] ebr;
            if (writtenSectors < 1) {
                Util::Exception::throwException(Util::Exception::ILLEGAL_STATE, "PartitionHandler: Unable to write boot record to disk!");
            }
        }
    }
}

void PartitionHandler::deletePartition(uint8_t partitionNumber) {
    if (partitionNumber < 1) {
        Util::Exception::throwException(Util::Exception::INVALID_ARGUMENT, "PartitionHandler: Partition number must be larger than 0!");
    }

    // Read MBR
    auto *mbr = readBootRecord(0);
    if (mbr == nullptr) {
        return;
    }

    if (partitionNumber <= 4) {
        log.debug("Deleting primary partition (Number: [%u])", partitionNumber);
        auto *targetEntry = reinterpret_cast<PartitionTableEntry*>(&mbr[PARTITION_TABLE_START + sizeof(PartitionTableEntry) * (partitionNumber - 1)]);

        // Write empty partition entry
        *targetEntry = {};
        uint32_t writtenSectors = device.write(mbr, 0, 1);
        delete[] mbr;
        if (writtenSectors < 1) {
            Util::Exception::throwException(Util::Exception::ILLEGAL_STATE, "PartitionHandler: Unable to write boot record to disk!");
        }
    } else {
        log.debug("Deleting logical partition (Number: [%u]", partitionNumber);
        PartitionTableEntry extendedPartition{};
        auto *partitions = reinterpret_cast<PartitionTableEntry*>(&mbr[PARTITION_TABLE_START]);

        // Search extended partition
        for (uint32_t i = 0; i < 4; i++) {
            auto &partition = partitions[i];
            if (partition.systemId == EXTENDED_PARTITION || partition.systemId == EXTENDED_PARTITION_LBA) {
                extendedPartition = partition;
                break;
            }
        }

        // No longer needed
        delete[] mbr;

        // Special case for first logical partition
        if (partitionNumber == 5) {
            auto *ebr = readBootRecord(extendedPartition.relativeSector);
            auto &currentLogicalPartition = *reinterpret_cast<PartitionTableEntry*>(&ebr[PARTITION_TABLE_START]);
            auto &nextLogicalPartition = *reinterpret_cast<PartitionTableEntry*>(&ebr[PARTITION_TABLE_START + sizeof(PartitionTableEntry)]);

            if (nextLogicalPartition.systemId == EMPTY) {
                // If there is only one logical partition, we can just overwrite the first entry in the partition table
                currentLogicalPartition = {};
            } else {
                // There is more than one logical partition
                // We need to read the second one and let the linked list start with it
                auto *nextEbr = readBootRecord(extendedPartition.relativeSector + nextLogicalPartition.relativeSector);
                auto &nextEbrCurrentLogicalPartition = *reinterpret_cast<PartitionTableEntry*>(&nextEbr[PARTITION_TABLE_START]);
                auto &nextEbrNextLogicalPartition = *reinterpret_cast<PartitionTableEntry*>(&nextEbr[PARTITION_TABLE_START + sizeof(PartitionTableEntry)]);

                currentLogicalPartition.relativeSector = nextLogicalPartition.relativeSector + nextEbrCurrentLogicalPartition.relativeSector;
                currentLogicalPartition.sectorCount = nextEbrCurrentLogicalPartition.sectorCount;
                nextLogicalPartition = nextEbrNextLogicalPartition;
                delete[] nextEbr;
            }

            auto writtenBytes = device.write(ebr, extendedPartition.relativeSector, 1);
            delete[] ebr;
            if (writtenBytes < 1) {
                Util::Exception::throwException(Util::Exception::ILLEGAL_STATE, "PartitionHandler: Unable to write boot record to disk!");
            }
        } else {
            uint8_t *ebr;
            uint32_t lastEbrSector;
            uint32_t currentEbrSector = extendedPartition.relativeSector;
            PartitionTableEntry currentLogicalPartition{};
            PartitionTableEntry nextLogicalPartition{};

            // Iterate through logical partitions to the desired partition number or the end of the list
            for (uint32_t i = 5; i <= partitionNumber; i++) {
                ebr = readBootRecord(currentEbrSector);
                currentLogicalPartition = *reinterpret_cast<PartitionTableEntry*>(&ebr[PARTITION_TABLE_START]);
                nextLogicalPartition = *reinterpret_cast<PartitionTableEntry*>(&ebr[PARTITION_TABLE_START + sizeof(PartitionTableEntry)]);

                if (currentLogicalPartition.systemId == EMPTY || (nextLogicalPartition.systemId == EMPTY && i < partitionNumber)) {
                    Util::Exception::throwException(Util::Exception::INVALID_ARGUMENT, "PartitionHandler: Trying to delete a non-existent partition!");
                }

                if (i < partitionNumber) {
                    lastEbrSector = currentEbrSector;
                    currentEbrSector = extendedPartition.relativeSector + nextLogicalPartition.relativeSector;
                    delete[] ebr;
                }
            }

            // Delete partition
            auto *lastEbr = readBootRecord(lastEbrSector);
            auto &currentEntry = *reinterpret_cast<PartitionTableEntry*>(&ebr[PARTITION_TABLE_START]);
            auto &nextEntry = *reinterpret_cast<PartitionTableEntry*>(&ebr[PARTITION_TABLE_START + sizeof (PartitionTableEntry)]);
            currentEntry = {};
            nextEntry = {};

            // Let predecessor point to successor
            auto &predecessor = *reinterpret_cast<PartitionTableEntry*>(&lastEbr[PARTITION_TABLE_START + sizeof (PartitionTableEntry)]);
            predecessor = nextLogicalPartition;

            auto writtenSectors = device.write(ebr, currentEbrSector, 1);
            delete[] ebr;
            if (writtenSectors < 1) {
                Util::Exception::throwException(Util::Exception::ILLEGAL_STATE, "PartitionHandler: Unable to write boot record to disk!");
            }

            writtenSectors = device.write(lastEbr, lastEbrSector, 1);
            delete[] lastEbr;
            if (writtenSectors < 1) {
                Util::Exception::throwException(Util::Exception::ILLEGAL_STATE, "PartitionHandler: Unable to write boot record to disk!");
            }
        }
    }
}

void PartitionHandler::createBootRecord(uint32_t sector) {
    auto *mbr = new uint8_t[device.getSectorSize()];
    auto mbrAddress = Util::Memory::Address<uint32_t>(mbr);

    // Create new empty boot record
    mbrAddress.setRange(0, device.getSectorSize());

    // Set boot record signature
    mbrAddress.setShort(BOOT_RECORD_SIGNATURE, 510);

    // Write fresh boot record to disk
    auto writtenSectors = device.write(mbr, sector, 1) == device.getSectorSize();
    delete[] mbr;

    if (writtenSectors < 1) {
        Util::Exception::throwException(Util::Exception::ILLEGAL_STATE, "PartitionHandler: Unable to write boot record to disk!");
    }
}

uint8_t* PartitionHandler::readBootRecord(uint32_t sector) {
    // Read boot record
    auto *bootRecord = new uint8_t[device.getSectorSize()];
    if (device.read(bootRecord, sector, 1) < 1) {
        delete[] bootRecord;
        return nullptr;
    }

    // Check boot record signature
    uint16_t signature = *((uint16_t *)(&bootRecord[510]));
    if(signature != BOOT_RECORD_SIGNATURE) {
        delete[] bootRecord;
        return nullptr;
    }

    return bootRecord;
}

bool PartitionHandler::PartitionInfo::operator==(const PartitionHandler::PartitionInfo &other) const {
    return number == other.number && type == other.type && active == other.active &&
           systemId == other.systemId && startSector == other.startSector &&
           sectorCount == other.sectorCount;
}

bool PartitionHandler::PartitionInfo::operator!=(const PartitionHandler::PartitionInfo &other) const {
    return number != other.number || type != other.type || active != other.active ||
           systemId != other.systemId || startSector != other.startSector ||
           sectorCount != other.sectorCount;
}

}