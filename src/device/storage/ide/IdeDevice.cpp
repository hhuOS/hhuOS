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

#include "IdeDevice.h"

#include "device/storage/ide/IdeController.h"

namespace Device::Storage {

IdeDevice::IdeDevice(IdeController &controller, const IdeController::DeviceInfo &deviceInfo) : controller(controller), info(deviceInfo) {}

uint32_t IdeDevice::getSectorSize() {
    return info.sectorSize;
}

uint64_t IdeDevice::getSectorCount() {
    return info.maxSectorsLba28;
}

uint32_t IdeDevice::read(uint8_t *buffer, uint32_t startSector, uint32_t sectorCount) {
    return controller.performIO(info, IdeController::READ, buffer, startSector, sectorCount);
}

uint32_t IdeDevice::write(const uint8_t *buffer, uint32_t startSector, uint32_t sectorCount) {
    return controller.performIO(info, IdeController::WRITE, const_cast<uint8_t*>(buffer), startSector, sectorCount);
}

}