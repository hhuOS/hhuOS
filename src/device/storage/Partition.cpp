/*
 * Copyright (C) 2017-2026 Heinrich Heine University Düsseldorf,
 * Institute of Computer Science, Department Operating Systems
 * Main developers: Christian Gesse <christian.gesse@hhu.de>, Fabian Ruhland <ruhland@hhu.de>
 * Original development team: Burak Akguel, Christian Gesse, Fabian Ruhland, Filip Krakowski, Michael Schöttner
 * This project has been supported by several students.
 * A full list of integrated student theses can be found here: https://github.com/hhuOS/hhuOS/wiki/Student-theses
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

#include "Partition.h"

#include "device/storage/StorageDevice.h"

namespace Device::Storage {

Partition::Partition(StorageDevice &parentDevice, uint32_t startSector, uint32_t sectorCount) : parentDevice(parentDevice), startSector(startSector), sectorCount(sectorCount) {}

uint32_t Partition::getSectorSize() {
    return parentDevice.getSectorSize();
}

uint64_t Partition::getSectorCount() {
    return sectorCount;
}

uint32_t Partition::read(uint8_t *buffer, uint32_t startSector, uint32_t sectorCount) {
    return parentDevice.read(buffer, this->startSector + startSector, sectorCount);
}

uint32_t Partition::write(const uint8_t *buffer, uint32_t startSector, uint32_t sectorCount) {
    return parentDevice.write(buffer, this->startSector + startSector, sectorCount);
}

}