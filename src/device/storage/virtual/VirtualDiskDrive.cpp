/*
 * Copyright (C) 2017-2025 Heinrich Heine University Düsseldorf,
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

#include "VirtualDiskDrive.h"
#include "lib/util/base/Panic.h"

namespace Device::Storage {

VirtualDiskDrive::VirtualDiskDrive(uint32_t sectorSize, uint32_t sectorCount) :
        address(new uint8_t[sectorSize * sectorCount]), freeAddress(true), sectorSize(sectorSize), sectorCount(sectorCount) {}

VirtualDiskDrive::VirtualDiskDrive(uint8_t *address, uint32_t sectorSize, uint32_t sectorCount) :
        address(address), freeAddress(false), sectorSize(sectorSize), sectorCount(sectorCount) {}

VirtualDiskDrive::~VirtualDiskDrive() {
    if (freeAddress) {
        delete reinterpret_cast<uint8_t*>(address.get());
    }
}

uint32_t VirtualDiskDrive::getSectorSize() {
    return sectorSize;
}

uint64_t VirtualDiskDrive::getSectorCount() {
    return sectorCount;
}

uint32_t VirtualDiskDrive::read(uint8_t *buffer, uint32_t startSector, uint32_t sectorCount) {
    if (startSector + sectorCount > VirtualDiskDrive::sectorCount) {
        Util::Panic::fire(Util::Panic::OUT_OF_BOUNDS, "VirtualDiskDrive: Trying to read out of bounds!");
    }

    auto byteCount = sectorSize * sectorCount;
    auto source = address.add(sectorSize * startSector);
    auto target = Util::Address(buffer);

    ioLock.acquire();
    target.copyRange(source, byteCount);
    ioLock.release();

    return sectorCount;
}

uint32_t VirtualDiskDrive::write(const uint8_t *buffer, uint32_t startSector, uint32_t sectorCount) {
    if (startSector + sectorCount > VirtualDiskDrive::sectorCount) {
        Util::Panic::fire(Util::Panic::OUT_OF_BOUNDS, "VirtualDiskDrive: Trying to write out of bounds!");
    }

    auto byteCount = sectorSize * sectorCount;
    auto source = Util::Address(buffer);
    auto target = address.add(sectorSize * startSector);

    ioLock.acquire();
    target.copyRange(source, byteCount);
    ioLock.release();

    return sectorCount;
}

}