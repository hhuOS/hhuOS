/*
 * Copyright (C) 2018-2024 Heinrich-Heine-Universitaet Duesseldorf,
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
 *
 * The ISO9660 driver is based on a bachelor's thesis, written by Moritz Riefer.
 * The original source code can be found here: https://git.hhu.de/bsinfo/thesis/ba-morie103
 */

#include "IsoDriver.h"

#include "kernel/log/Log.h"
#include "IsoNode.h"
#include "lib/util/base/Address.h"
#include "device/storage/StorageDevice.h"
#include "lib/util/collection/Array.h"
#include "lib/util/collection/Iterator.h"

namespace Filesystem {
class Node;
}  // namespace Filesystem

namespace Filesystem::Iso {

bool IsoDriver::mount(Device::Storage::StorageDevice &device) {
    IsoDriver::device = &device;

    if (!initializePrimaryVolumeDescriptor()) {
        return false;
    }

    if (!initializePathTable()) {
        return false;
    }

    return true;
}

bool IsoDriver::createFilesystem([[maybe_unused]] Device::Storage::StorageDevice &device) {
    return false;
}

Node* IsoDriver::getNode(const Util::String &path) {
    // Directories are linked in reverse order (child to parent) in the path table list.
    // To find the directory, holding the searched file, we split the path and search for deepest directory
    auto pathSegments = path.split(Util::Io::File::SEPARATOR);
    auto nodeName = pathSegments.length() == 0 ? "" : pathSegments[pathSegments.length() - 1];

    const auto *entry = pathTableEntryList.get(0); // Root directory is always at index 0
    for (uint32_t i = 0; i < pathSegments.length(); i++) {
        const auto &currentName = pathSegments[i];

        for (const auto *currentEntry : pathTableEntryList) {
            if (currentEntry->getName() == currentName) {
                entry = currentEntry;
            }
        }
    }

    // Read single sector at extent address
    auto *buffer = new uint8_t[device->getSectorSize()];
    auto readSectors = device->read(buffer, entry->extentLba, 1);
    if (readSectors != 1) {
        delete[] buffer;
        return nullptr;
    }

    // First record describes the directory itself
    const auto &selfRecord = *reinterpret_cast<DirectoryRecord*>(buffer);
    if (pathSegments.length() == 0 || entry->getName() == pathSegments[pathSegments.length() - 1]) {
        // The requested node is the directory itself
        auto *node = new IsoNode(*device, selfRecord.createCopy());

        delete[] buffer;
        return node;
    }

    if (selfRecord.dataLengthLSB > device->getSectorSize()) {
        // Directory tables spans more than one sector -> Read whole table
        uint8_t sectorCount = (selfRecord.dataLengthLSB % device->getSectorSize() == 0) ? (selfRecord.dataLengthLSB / device->getSectorSize()) : (selfRecord.dataLengthLSB / device->getSectorSize() + 1);
        delete[] buffer;
        buffer = new uint8_t[sectorCount * device->getSectorSize()];

        readSectors = device->read(buffer, entry->extentLba, sectorCount);
        if (readSectors != sectorCount) {
            delete[] buffer;
            return nullptr;
        }
    }

    uint32_t index = 0;
    while (index < selfRecord.dataLengthLSB) {
        const auto &record = *reinterpret_cast<DirectoryRecord*>(buffer + index);
        if (record.recordLength == 0) {
            // Skip padding bytes by aligning index to next sector
            index = ((index + device->getSectorSize()) / device->getSectorSize()) * device->getSectorSize();
        } else {
            // Skip self and parent referencing records
            if (!(record.identifierLength == 1 && (record.identifier[0] == 0x00 || record.identifier[0] == 0x01))) {
                // Record is valid
                if (record.getName() == pathSegments[pathSegments.length() - 1]) {
                    // We found the searched record -> Create and return Node instance
                    auto *node = new IsoNode(*device, record.createCopy());

                    delete[] buffer;
                    return node;
                }
            }

            index += record.recordLength;
        }
    }

    return nullptr;
}

bool IsoDriver::createNode([[maybe_unused]] const Util::String &path, [[maybe_unused]] Util::Io::File::Type type) {
    return false;
}

bool IsoDriver::deleteNode([[maybe_unused]] const Util::String &path) {
    return false;
}

bool IsoDriver::initializePrimaryVolumeDescriptor() {
    LOG_INFO("Searching primary volume descriptor");
    auto *buffer = new uint8_t[device->getSectorSize()];

    for (uint16_t i = 0;; i++) {
        uint32_t startSector = VOLUME_DESCRIPTORS_START_SECTOR + i;
        uint16_t readSectors = device->read(buffer, startSector, 1);

        const auto &header = *reinterpret_cast<VolumeDescriptorHeader*>(buffer);
        if (readSectors != 1 || buffer[0] == VOLUME_DESCRIPTOR_SET_TERMINATOR) {
            delete[] buffer;
            return false;
        }

        if (header.type == PRIMARY_VOLUME_DESCRIPTOR) {
            break;
        }
    }

    primaryVolumeDescriptor = *reinterpret_cast<PrimaryVolumeDescriptor*>(buffer);

    auto identifier = Util::String(reinterpret_cast<uint8_t*>(primaryVolumeDescriptor.volumeIdentifier), sizeof(PrimaryVolumeDescriptor::volumeIdentifier));
    LOG_INFO("Found primary volume descriptor (Identifier: [%s])", static_cast<const char*>(identifier.strip()));

    delete[] buffer;
    return true;
}

bool IsoDriver::initializePathTable() {
    uint8_t sectorCount = (primaryVolumeDescriptor.pathTableSizeLSB % device->getSectorSize() == 0) ? (primaryVolumeDescriptor.pathTableSizeLSB / device->getSectorSize()) : (primaryVolumeDescriptor.pathTableSizeLSB / device->getSectorSize() + 1);
    auto *buffer = new uint8_t[sectorCount * device->getSectorSize()];

    auto readSectors = device->read(buffer, primaryVolumeDescriptor.pathTableLbaLSB, sectorCount);
    if (readSectors != sectorCount) {
        delete[] buffer;
        return false;
    }

    uint32_t index = 0;
    while (index < primaryVolumeDescriptor.pathTableSizeLSB) {
        const auto &entry = *reinterpret_cast<PathTableEntry*>(buffer + index);

        // Copy entry manually for use in ArrayList, because its size is dynamic
        pathTableEntryList.add(entry.createCopy());

        index += entry.getLength();
    }

    delete[] buffer;
    return true;
}

Util::String IsoDriver::PathTableEntry::getName() const {
    return Util::String(reinterpret_cast<const uint8_t*>(directoryIdentifier), directoryIdentifierLength);
}

bool IsoDriver::PathTableEntry::operator!=(const IsoDriver::PathTableEntry &other) const {
    return extentLba != other.extentLba;
}

uint32_t IsoDriver::PathTableEntry::getLength() const {
    return 8 + directoryIdentifierLength + (directoryIdentifierLength % 2 == 0 ? 0 : 1);
}

IsoDriver::PathTableEntry* IsoDriver::PathTableEntry::createCopy() const {
    auto copyBuffer = new uint8_t[getLength()];
    Util::Address<uint32_t>(copyBuffer).copyRange(Util::Address<uint32_t>(this), getLength());

    return reinterpret_cast<PathTableEntry*>(copyBuffer);
}

Util::String IsoDriver::DirectoryRecord::getName() const {
    auto name =  Util::String(reinterpret_cast<const uint8_t*>(identifier), identifierLength);
    if (name.contains(';')) {
        name = name.split(";")[0];
    }

    if (name.endsWith(".")) {
        name = name.substring(0, name.length() - 1);
    }

    return name;
}

IsoDriver::DirectoryRecord* IsoDriver::DirectoryRecord::createCopy() const {
    auto copyBuffer = new uint8_t[recordLength];
    Util::Address<uint32_t>(copyBuffer).copyRange(Util::Address<uint32_t>(this), recordLength);

    return reinterpret_cast<DirectoryRecord*>(copyBuffer);
}

bool IsoDriver::DirectoryRecord::isDirectory() const {
    return flags & FileFlags::DIRECTORY;
}

}