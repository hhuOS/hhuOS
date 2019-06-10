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

#include "AhciDevice.h"

AhciDevice::AhciDevice(AhciController &controller, uint8_t ahciDiskNumber, String name) :
        StorageDevice(name), controller(controller), ahciDiskNumber(ahciDiskNumber) {
    deviceInfo = controller.getDeviceInfo(ahciDiskNumber);
}

bool AhciDevice::read(uint8_t *buff, uint32_t sector, uint32_t count) {
	return controller.read(ahciDiskNumber, sector, 0, count, (uint16_t *) buff);
}

bool AhciDevice::write(const uint8_t *buff, uint32_t sector, uint32_t count) {
    return controller.write(ahciDiskNumber, sector, 0, count, (uint16_t *) buff);
}

String AhciDevice::getHardwareName() {
    return deviceInfo.name;
}

uint32_t AhciDevice::getSectorSize() {
    // TODO: Get actual sector size from controller.
    return 512;
}

uint64_t AhciDevice::getSectorCount() {
    return deviceInfo.sectorCount;
}