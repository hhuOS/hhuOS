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

#include "PciDevice.h"
#include "Pci.h"
#include "device/cpu/IoPort.h"
#include "kernel/log/Logger.h"
#include "lib/util/data/ArrayList.h"
#include "lib/util/data/Collection.h"
#include "lib/util/data/Iterator.h"

namespace Device {

const IoPort Pci::configAddressPort = IoPort(CONFIG_ADDRESS);
const IoPort Pci::configDataPort = IoPort(CONFIG_DATA);
Kernel::Logger Pci::log = Kernel::Logger::get("PCI");
Util::Data::ArrayList<PciDevice> Pci::devices = Util::Data::ArrayList<PciDevice>();

void Pci::prepareRegister(uint8_t bus, uint8_t device, uint8_t function, uint8_t offset) {
    uint32_t address = 0x80000000 | (bus << 16) | (device << 11) | (function << 8) | (offset & 0xfc);
    configAddressPort.writeDoubleWord(address);
}

uint32_t Pci::readDoubleWord(uint8_t bus, uint8_t device, uint8_t function, uint8_t offset) {
    prepareRegister(bus, device, function, offset);
    return configDataPort.readDoubleWord();
}

uint16_t Pci::readWord(uint8_t bus, uint8_t device, uint8_t function, uint8_t offset) {
    return (readDoubleWord(bus, device, function, offset) >> ((offset & 0x02) * 8)) & 0xffff;
}

uint8_t Pci::readByte(uint8_t bus, uint8_t device, uint8_t function, uint8_t offset) {
    return (readDoubleWord(bus, device, function, offset) >> ((offset & 0x03) * 8)) & 0xff;
}

void Pci::writeDoubleWord(uint8_t bus, uint8_t device, uint8_t function, uint8_t offset, uint32_t value) {
    prepareRegister(bus, device, function, offset);
    configDataPort.writeDoubleWord(value);
}

void Pci::writeWord(uint8_t bus, uint8_t device, uint8_t function, uint8_t offset, uint16_t value) {
    prepareRegister(bus, device, function, offset);
    configDataPort.writeWord(offset & 0x02, value);
}

void Pci::writeByte(uint8_t bus, uint8_t device, uint8_t function, uint8_t offset, uint8_t value) {
    prepareRegister(bus, device, function, offset);
    configDataPort.writeByte(offset & 0x03, value);
}

void Pci::scan() {
    // Check header type of host controller
    // If it is a multi-function device, there multiple host controllers available at bus 0, device 0, function 0-7
    // Otherwise, there is only one host controller at bus 0, device 0, function 0
    auto headerType = readByte(0, 0, 0, HEADER_TYPE);
    if ((headerType & 0x80) == 0) {
        scanBus(0);
    } else {
        for (uint8_t i = 0; i < MAX_FUNCTIONS_PER_DEVICE; i++) {
            auto vendorId = readWord(0, 0, i, VENDOR_ID);
            if (vendorId != INVALID_VENDOR) {
                break;
            }

            scanBus(i);
        }
    }
}

PciDevice Pci::readDevice(uint8_t bus, uint8_t device, uint8_t function) {
    return {bus, device, function};
}

void Pci::checkFunction(uint8_t bus, uint8_t device, uint8_t function) {
    auto baseClass = readByte(bus, device, function, CLASS);
    auto subClass = readByte(bus, device, function, SUBCLASS);

    if (baseClass == BRIDGE && subClass == PCI_TO_PCI) {
        log.info("Found PCI-to-PCI bridge on bus [%u]", bus);
        uint8_t secondaryBus = readByte(bus, device, function, SECONDARY_BUS);
        scanBus(secondaryBus);
    } else {
        auto pciDevice = readDevice(bus, device, function);
        log.info("Found PCI device [0x%04x:0x%04x] on bus [%u]", pciDevice.getVendorId(), pciDevice.getDeviceId(), bus);
        devices.add(pciDevice);
    }
}

void Pci::checkDevice(uint8_t bus, uint8_t device) {
    auto vendorId = readWord(bus, device, 0, VENDOR_ID);
    if (vendorId == INVALID_VENDOR) {
        return;
    }

    checkFunction(bus, device, 0);

    auto headerType = readByte(bus, device, 0, HEADER_TYPE);
    if ((headerType & HEADER_TYPE_MULTIFUNCTION_BIT) != 0) {
        for (uint8_t i = 1; i < MAX_FUNCTIONS_PER_DEVICE; i++) {
            vendorId = readWord(bus, device, i, VENDOR_ID);
            if (vendorId != INVALID_VENDOR) {
                checkFunction(bus, device, i);
            }
        }
    }
}

void Pci::scanBus(uint8_t bus) {
    for (uint8_t i = 0; i < MAX_DEVICES_PER_BUS; i++) {
        checkDevice(bus, i);
    }
}

Util::Data::Array<PciDevice> Pci::search(uint16_t vendorId) {
    Util::Data::ArrayList<PciDevice> found = Util::Data::ArrayList<PciDevice>();
    for (const auto &device : devices) {
        if (device.getVendorId() == vendorId) {
            found.add(device);
        }
    }

    return found.toArray();
}

Util::Data::Array<PciDevice> Pci::search(uint16_t vendorId, uint16_t deviceId) {
    Util::Data::ArrayList<PciDevice> found = Util::Data::ArrayList<PciDevice>();
    for (const auto &device : devices) {
        if (device.getVendorId() == vendorId && device.getDeviceId() == deviceId) {
            found.add(device);
        }
    }

    return found.toArray();
}

Util::Data::Array<PciDevice> Pci::search(uint16_t vendorId, uint16_t deviceId, uint8_t programmingInterface) {
    Util::Data::ArrayList<PciDevice> found = Util::Data::ArrayList<PciDevice>();
    for (const auto &device : devices) {
        if (device.getVendorId() == vendorId && device.getDeviceId() == deviceId && device.getProgrammingInterface() == programmingInterface) {
            found.add(device);
        }
    }

    return found.toArray();
}

Util::Data::Array<PciDevice> Pci::search(Pci::Class baseClass) {
    Util::Data::ArrayList<PciDevice> found = Util::Data::ArrayList<PciDevice>();
    for (const auto &device : devices) {
        if (device.getBaseClass() == baseClass) {
            found.add(device);
        }
    }

    return found.toArray();
}

Util::Data::Array<PciDevice> Pci::search(Pci::Class baseClass, uint8_t subclass) {
    Util::Data::ArrayList<PciDevice> found = Util::Data::ArrayList<PciDevice>();
    for (const auto &device : devices) {
        if (device.getBaseClass() == baseClass && device.getSubclass() == subclass) {
            found.add(device);
        }
    }

    return found.toArray();
}

Util::Data::Array<PciDevice> Pci::search(Pci::Class baseClass, uint8_t subclass, uint8_t programmingInterface) {
    Util::Data::ArrayList<PciDevice> found = Util::Data::ArrayList<PciDevice>();
    for (const auto &device : devices) {
        if (device.getBaseClass() == baseClass && device.getSubclass() == subclass && device.getProgrammingInterface() == programmingInterface) {
            found.add(device);
        }
    }

    return found.toArray();
}

}