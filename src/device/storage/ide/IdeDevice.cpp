/*
 * Copyright (C) 2018-2025 Heinrich-Heine-Universitaet Duesseldorf,
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
 * The IDE driver is based on a bachelor's thesis, written by Tim Laurischkat.
 * The original source code can be found here: https://git.hhu.de/bsinfo/thesis/ba-tilau101
 *
 * The driver has been enhanced with ATAPI capabilities during a bachelor's thesis, written by Moritz Riefer.
 * The original source code can be found here: https://git.hhu.de/bsinfo/thesis/ba-morie103
 */

#include "IdeDevice.h"

#include "device/storage/ide/IdeController.h"

namespace Device::Storage {

IdeDevice::IdeDevice(IdeController &controller, const IdeController::DeviceInfo &deviceInfo) : controller(controller), info(deviceInfo) {}

uint32_t IdeDevice::getSectorSize() {
    return info.sectorSize;
}

uint64_t IdeDevice::getSectorCount() {
    if (info.type == IdeController::ATAPI) {
        return info.atapi.maxSectorsLba;
    }

    return info.maxSectorsLba28;
}

uint32_t IdeDevice::read(uint8_t *buffer, uint32_t startSector, uint32_t sectorCount) {
    if (info.type == IdeController::ATAPI) {
        return controller.performAtapiIO(info, IdeController::READ, buffer, startSector, sectorCount);
    }

    return controller.performAtaIO(info, IdeController::READ, buffer, startSector, sectorCount);
}

uint32_t IdeDevice::write(const uint8_t *buffer, uint32_t startSector, uint32_t sectorCount) {
    if (info.type == IdeController::ATAPI) {
        return 0;
    }

    return controller.performAtaIO(info, IdeController::WRITE, const_cast<uint8_t *>(buffer), startSector, sectorCount);
}

const IdeController::DeviceInfo &IdeDevice::getDeviceInfo() const {
    return info;
}

}