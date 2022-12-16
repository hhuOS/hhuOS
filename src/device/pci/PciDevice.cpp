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

#include "device/pci/Pci.h"
#include "lib/util/data/ArrayList.h"
#include "lib/util/data/Collection.h"
#include "lib/util/data/Iterator.h"

namespace Device {

PciDevice::PciDevice(uint8_t bus, uint8_t device, uint8_t function) : bus(bus), device(device), function(function),
        vendorId(readWord(Pci::VENDOR_ID)), deviceId(readWord(Pci::DEVICE_ID)), revision(readByte(Pci::REVISION)),
        programmingInterface(readByte(Pci::PROGRAMMING_INTERFACE)), baseClass(readByte(Pci::CLASS)), subclass(readByte(Pci::SUBCLASS)),
        subsystemVendorId(readWord(Pci::SUBSYSTEM_VENDOR_ID)), subsystemId(readWord(Pci::SUBSYSTEM_ID)), capabilitiesPointer(readByte(Pci::CAPABILITIES_POINTER)),
        interruptLine(static_cast<Pic::Interrupt>(readByte(Pci::INTERRUPT_LINE))) {}

bool PciDevice::operator!=(const PciDevice &other) const {
    return vendorId != other.vendorId && deviceId != other.deviceId;
}

uint8_t PciDevice::readByte(uint8_t reg) const {
    return Pci::readByte(bus, device, function, reg);
}

uint16_t PciDevice::readWord(uint8_t reg) const {
    return Pci::readWord(bus, device, function, reg);
}

uint32_t PciDevice::readDoubleWord(uint8_t reg) const {
    return Pci::readDoubleWord(bus, device, function, reg);
}

void PciDevice::writeByte(uint8_t reg, uint8_t value) const {
    Pci::writeByte(bus, device, function, reg, value);
}

void PciDevice::writeWord(uint8_t reg, uint16_t value) const {
    Pci::writeWord(bus, device, function, reg, value);
}

void PciDevice::writeDoubleWord(uint8_t reg, uint32_t value) const {
    Pci::writeDoubleWord(bus, device, function, reg, value);
}

void PciDevice::writeCommand(const Util::Data::Array<Pci::Command>& commands) const {
    uint16_t value = readWord(Pci::COMMAND);
    for (const auto &command : commands) {
        value |= command;
    }

    writeWord(Pci::COMMAND, value);
}

void PciDevice::overwriteCommand(const Util::Data::Array<Pci::Command>& commands) const {
    uint16_t value = 0;
    for (const auto &command : commands) {
        value |= command;
    }

    writeWord(Pci::COMMAND, value);
}

Util::Data::Array<Pci::Command> PciDevice::readCommand() const {
    auto command = Util::Data::ArrayList<Pci::Command>();
    uint16_t value = readWord(Pci::COMMAND);

    for (uint16_t i = 1; i > 0; i *= 2) {
        if ((value & i) != 0) {
            command.add(static_cast<const Pci::Command>(i));
        }
    }

    return command.toArray();
}

Util::Data::Array<Pci::Status> PciDevice::readStatus() const {
    auto status = Util::Data::ArrayList<Pci::Status>();
    uint16_t value = readWord(Pci::STATUS);

    for (uint16_t i = 1; i > 0; i *= 2) {
        if ((value & i) != 0) {
            status.add(static_cast<const Pci::Status>(i));
        }
    }

    return status.toArray();
}

Util::Data::Array<uint8_t> PciDevice::readCapabilities() const {
    if (!readStatus().contains(Pci::CAPABILITIES_LIST)) {
        return {};
    }

    auto capabilities = Util::Data::ArrayList<uint8_t>();
    auto currentRegister = capabilitiesPointer;

    while (currentRegister != 0x00) {
        capabilities.add(readByte(currentRegister));
        currentRegister = readByte(currentRegister + 1);
    }

    return capabilities.toArray();
}

uint16_t PciDevice::getVendorId() const {
    return vendorId;
}

uint16_t PciDevice::getDeviceId() const {
    return deviceId;
}

uint8_t PciDevice::getRevision() const {
    return revision;
}

uint8_t PciDevice::getProgrammingInterface() const {
    return programmingInterface;
}

uint8_t PciDevice::getBaseClass() const {
    return baseClass;
}

uint8_t PciDevice::getSubclass() const {
    return subclass;
}

uint16_t PciDevice::getSubsystemVendorId() const {
    return subsystemVendorId;
}

uint16_t PciDevice::getSubsystemId() const {
    return subsystemId;
}

Pic::Interrupt PciDevice::getInterruptLine() const {
    return interruptLine;
}

}