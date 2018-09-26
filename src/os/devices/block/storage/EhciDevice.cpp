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

#include "EhciDevice.h"

EhciDevice::EhciDevice(Ehci &controller, uint8_t ehciDiskNumber, String name) : StorageDevice(name), controller(controller), ehciDiskNumber(ehciDiskNumber) {

}

bool EhciDevice::read(uint8_t *buff, uint32_t sector, uint32_t count) {
	return controller.getDevice(ehciDiskNumber)->readData(0, sector, count, buff);
}

bool EhciDevice::write(const uint8_t *buff, uint32_t sector, uint32_t count) {
    return controller.getDevice(ehciDiskNumber)->writeData(0, sector, count, (uint8_t *) buff);
}

uint32_t EhciDevice::getSectorSize() {
    // TODO: Get actual sector size from controller.
    return 512;
}

uint64_t EhciDevice::getSectorCount() {
    // TODO: Get actual sector count from controller.
    return 0;
}

String EhciDevice::getHardwareName() {
    // TODO: Get actual device name from controller.
    return "";
}
