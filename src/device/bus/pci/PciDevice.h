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

#ifndef HHUOS_PCIDEVICE_H
#define HHUOS_PCIDEVICE_H

#include <stdint.h>

#include "Pci.h"
#include "lib/util/collection/Array.h"

namespace Device {
enum InterruptRequest : uint8_t;

class PciDevice {

public:
    /**
     * Constructor.
     */
     PciDevice() = default;

    /**
     * Constructor.
     */
    PciDevice(uint8_t bus, uint8_t device, uint8_t function);

    /**
     * Copy Constructor.
     */
    PciDevice(const PciDevice &other) = default;

    /**
     * Assignment operator.
     */
    PciDevice &operator=(const PciDevice &other) = default;

    /**
     * Destructor.
     */
    ~PciDevice() = default;

    bool operator!=(const PciDevice &other) const;

    uint8_t readByte(uint8_t reg) const;

    uint16_t readWord(uint8_t reg) const;

    uint32_t readDoubleWord(uint8_t reg) const;

    void writeByte(uint8_t reg, uint8_t value) const;

    void writeWord(uint8_t reg, uint16_t value) const;

    void writeDoubleWord(uint8_t reg, uint32_t value) const;

    Util::Array<Pci::Command> readCommand() const;

    Util::Array<Pci::Status> readStatus() const;

    Util::Array<uint8_t> readCapabilities() const;

    void writeCommand(const Util::Array<Pci::Command> &commands) const;

    void overwriteCommand(const Util::Array<Pci::Command> &commands) const;

    uint16_t getVendorId() const;

    uint16_t getDeviceId() const;

    uint8_t getRevision() const;

    uint8_t getProgrammingInterface() const;

    uint8_t getBaseClass() const;

    uint8_t getSubclass() const;

    uint16_t getSubsystemVendorId() const;

    uint16_t getSubsystemId() const;

    Device::InterruptRequest getInterruptLine() const;

private:

    uint8_t bus{};
    uint8_t device{};
    uint8_t function{};
    uint16_t vendorId{};
    uint16_t deviceId{};
    uint8_t revision{};
    uint8_t programmingInterface{};
    uint8_t baseClass{};
    uint8_t subclass{};
    uint16_t subsystemVendorId{};
    uint16_t subsystemId{};
    uint8_t capabilitiesPointer{};
    Device::InterruptRequest interruptLine{};
};

}

#endif
