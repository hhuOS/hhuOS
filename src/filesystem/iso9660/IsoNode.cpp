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

#include "IsoNode.h"

#include "lib/util/base/Address.h"
#include "device/storage/StorageDevice.h"
#include "filesystem/iso9660/IsoDriver.h"
#include "lib/util/collection/ArrayList.h"

namespace Filesystem::Iso {

IsoNode::IsoNode(Device::Storage::StorageDevice &device, const IsoDriver::DirectoryRecord *record) : device(device), record(*record) {}

IsoNode::~IsoNode() {
    delete &record;
}

Util::String IsoNode::getName() {
    return record.getName();
}

uint64_t IsoNode::getLength() {
    return record.dataLengthLSB;
}

Util::Io::File::Type IsoNode::getType() {
    return record.isDirectory() ? Util::Io::File::DIRECTORY : Util::Io::File::REGULAR;
}

Util::Array<Util::String> IsoNode::getChildren() {
    uint8_t sectorCount = (record.dataLengthLSB % device.getSectorSize() == 0) ? (record.dataLengthLSB / device.getSectorSize()) : (record.dataLengthLSB / device.getSectorSize() + 1);
    auto buffer = new uint8_t[sectorCount * device.getSectorSize()];

    auto readSectors = device.read(buffer, record.extentLbaLSB, sectorCount);
    if (readSectors != sectorCount) {
        delete[] buffer;
        return Util::Array<Util::String>(0);
    }

    auto names = Util::ArrayList<Util::String>();

    uint32_t index = 0;
    while (index < record.dataLengthLSB) {
        const auto &currentRecord = *reinterpret_cast<IsoDriver::DirectoryRecord*>(buffer + index);
        if (currentRecord.recordLength == 0) {
            // Skip padding bytes by aligning index to next sector
            index = ((index + device.getSectorSize()) / device.getSectorSize()) * device.getSectorSize();
        } else {
            // Skip self and parent referencing records
            if (!(currentRecord.identifierLength == 1 && (currentRecord.identifier[0] == 0x00 || currentRecord.identifier[0] == 0x01))) {
                // Record is valid -> Add name to list
                names.add(currentRecord.getName());
            }

            index += currentRecord.recordLength;
        }
    }

    return names.toArray();
}

uint64_t IsoNode::readData(uint8_t *targetBuffer, uint64_t pos, uint64_t numBytes) {
    if (pos >= record.dataLengthLSB) {
        return 0;
    }

    if (pos + numBytes > record.dataLengthLSB) {
        numBytes = (record.dataLengthLSB - pos);
    }

    uint32_t startSector = record.extentLbaLSB + (static_cast<uint32_t>(pos) / device.getSectorSize());
    uint32_t sectorCount = (static_cast<uint32_t>(numBytes) / device.getSectorSize()) + 1;

    auto *buffer = new uint8_t[sectorCount * device.getSectorSize()];
    auto readSectors = device.read(buffer, startSector, sectorCount);
    if (readSectors != sectorCount) {
        delete[] buffer;
        return 0;
    }

    auto sourceAddress = Util::Address<uint32_t>(buffer).add(static_cast<uint32_t>(pos) % device.getSectorSize());
    auto targetAddress = Util::Address<uint32_t>(targetBuffer);
    targetAddress.copyRange(sourceAddress, numBytes);

    delete[] buffer;
    return numBytes;
}

uint64_t IsoNode::writeData([[maybe_unused]] const uint8_t *sourceBuffer, [[maybe_unused]] uint64_t pos, [[maybe_unused]] uint64_t numBytes) {
    return 0;
}

}