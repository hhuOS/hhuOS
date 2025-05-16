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
 *
 * The AHCI driver is based on a bachelor's thesis, written by Manuel Demetrio Angelescu.
 * The original source code can be found here: https://github.com/m8nu/hhuOS
 */

#include "AhciDevice.h"

#include "device/storage/ahci/AhciController.h"

namespace Device::Storage {

AhciDevice::AhciDevice(uint32_t portNumber, AhciController::DeviceSignature type, AhciController::DeviceInfo *deviceInfo, AhciController &controller) : portNumber(portNumber), type(type), info(*deviceInfo), controller(controller) {}

AhciDevice::~AhciDevice() {
    delete &info;
}

uint32_t AhciDevice::getSectorSize() {
    return info.bytesPerSector;
}

uint64_t AhciDevice::getSectorCount() {
    return info.lbaCapacity;
}

uint32_t AhciDevice::read(uint8_t *buffer, uint32_t startSector, uint32_t sectorCount) {
    if (type == AhciController::ATAPI) {
        return controller.performAtapiIO(portNumber, info, AhciController::READ, buffer, startSector, sectorCount);
    }

    return controller.performAtaIO(portNumber, info, AhciController::READ, buffer, startSector, sectorCount);
}

uint32_t AhciDevice::write(const uint8_t *buffer, uint32_t startSector, uint32_t sectorCount) {
    return controller.performAtaIO(portNumber, info, AhciController::WRITE, const_cast<uint8_t*>(buffer), startSector, sectorCount);
}

}