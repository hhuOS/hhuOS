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

#ifndef HHUOS_PCI_H
#define HHUOS_PCI_H

#include <cstdint>

#include "lib/util/data/Array.h"

namespace Kernel {
class Logger;
}  // namespace Kernel
namespace Util {
namespace Data {
template <typename T> class ArrayList;
}  // namespace Data
}  // namespace Util

namespace Device {

class PciDevice;
class IoPort;

class Pci {

public:

    friend class PciDevice;

    enum Register : uint8_t {
        VENDOR_ID = 0x00,
        DEVICE_ID = 0x02,
        COMMAND = 0x04,
        STATUS = 0x06,
        REVISION = 0x08,
        PROGRAMMING_INTERFACE = 0x09,
        SUBCLASS = 0x0A,
        CLASS = 0x0B,
        CACHE_LINE_SIZE = 0x0C,
        MASTER_LATENCY_TIMER = 0x0D,
        HEADER_TYPE = 0x0E,
        BIST = 0x0F,
        BASE_ADDRESS_0 = 0x10,
        BASE_ADDRESS_1 = 0x14,
        BASE_ADDRESS_2 = 0x18,
        BASE_ADDRESS_3 = 0x1C,
        BASE_ADDRESS_4 = 0x20,
        BASE_ADDRESS_5 = 0x24,
        CARDBUS_CIS_POINTER = 0x28,
        SUBSYSTEM_VENDOR_ID = 0x2C,
        SUBSYSTEM_ID = 0x2E,
        CAPABILITIES_POINTER = 0x34,
        INTERRUPT_LINE = 0x3C,
        SECONDARY_BUS = 0x19,
    };

    enum Command : uint16_t {
        IO_SPACE = 0x0001,
        MEMORY_SPACE = 0x0002,
        BUS_MASTER = 0x0004,
        SPECIAL_CYCLES = 0x0008,
        MEMORY_WRITE_AND_INVALIDATE = 0x0010,
        VGA_PALETTE_SNOOP = 0x0020,
        PARITY_ERROR_RESPONSE = 0x0040,
        SERR_ENABLE = 0x0100,
        FAST_BACK_TO_BACK = 0x0200,
        INTERRUPT_DISABLE = 0x0400
    };

    enum Status : uint16_t {
        INTERRUPT = 0x0008,
        CAPABILITIES_LIST = 0x0010,
        CAPABLE_OF_66_MHZ = 0x0020,
        FAST_BACK_TO_BACK_CAPABLE = 0x0080,
        MASTER_DATA_PARITY_ERROR = 0x0100,
        DEVSEL_TIMING = 0x0200,
        SIGNALED_TARGET_ABORT = 0x0800,
        RECEIVED_TARGET_ABORT = 0x1000,
        RECEIVED_MASTER_ABORT = 0x2000,
        SIGNALED_SYSTEM_ERROR = 0x4000,
        DETECTED_PARITY_ERROR = 0x8000
    };

    enum Class : uint8_t {
        UNCLASSIFIED = 0x00,
        MASS_STORAGE = 0x01,
        NETWORK_CONTROLLER = 0x02,
        DISPLAY_CONTROLLER = 0x03,
        MULTIMEDIA_CONTROLLER = 0x04,
        MEMORY_CONTROLLER = 0x05,
        BRIDGE = 0x06,
        SIMPLE_COMMUNICATION_CONTROLLER = 0x07,
        BASE_SYSTEM_PERIPHERAL = 0x08,
        INPUT_DEVICE_CONTROLLER = 0x09,
        DOCKING_STATION = 0x0a,
        PROCESSOR = 0x0b,
        SERIAL_BUS_CONTROLLER = 0x0c,
        WIRELESS_CONTROLLER = 0x0d,
        INTELLIGENT_CONTROLLER = 0x0e,
        SATELLITE_COMMUNICATION_CONTROLLER = 0x0f,
        ENCRYPTION_CONTROLLER = 0x10,
        SIGNAL_PROCESSING_CONTROLLER = 0x11,
        PROCESSING_ACCELERATOR = 0x12,
        NON_ESSENTIAL_INSTRUMENTATION = 0x13,
        CO_PROCESSOR = 0x40,
        UNASSIGNED = 0xff
    };
    
    enum Bridge : uint8_t {
        HOST = 0x00,
        ISA = 0x01,
        EISA = 0x02,
        MCA = 0x03,
        PCI_TO_PCI = 0x04,
        PCMCIA = 0x05,
        NUBUS = 0x06,
        CARDBUS = 0x07,
        RACEWAY = 0x08,
        PCI_TO_PCI_HOST = 0x09,
        INFINIBAND_TO_PCI_HOST = 0x0a,
        OTHER = 0x80
    };

    /**
     * Constructor.
     * Deleted, as this class has only static members.
     */
    Pci() = delete;

    /**
     * Copy Constructor.
     */
    Pci(const Pci &other) = delete;

    /**
     * Assignment operator.
     */
    Pci &operator=(const Pci &other) = delete;

    /**
     * Destructor.
     */
    ~Pci() = delete;

    static void scan();

    static Util::Data::Array<PciDevice> search(uint16_t vendorId);

    static Util::Data::Array<PciDevice> search(uint16_t vendorId, uint16_t deviceId);

    static Util::Data::Array<PciDevice> search(uint16_t vendorId, uint16_t deviceId, uint8_t programmingInterface);

    static Util::Data::Array<PciDevice> search(Class baseClass);

    static Util::Data::Array<PciDevice> search(Class baseClass, uint8_t subclass);

    static Util::Data::Array<PciDevice> search(Class baseClass, uint8_t subclass, uint8_t programmingInterface);

private:

    static void prepareRegister(uint8_t bus, uint8_t device, uint8_t function, uint8_t offset);

    static uint32_t readDoubleWord(uint8_t bus, uint8_t device, uint8_t function, uint8_t offset);

    static uint16_t readWord(uint8_t bus, uint8_t device, uint8_t function, uint8_t offset);

    static uint8_t readByte(uint8_t bus, uint8_t device, uint8_t function, uint8_t offset);

    static void writeDoubleWord(uint8_t bus, uint8_t device, uint8_t function, uint8_t offset, uint32_t value);

    static void writeWord(uint8_t bus, uint8_t device, uint8_t function, uint8_t offset, uint16_t value);

    static void writeByte(uint8_t bus, uint8_t device, uint8_t function, uint8_t offset, uint8_t value);

    static PciDevice readDevice(uint8_t bus, uint8_t device, uint8_t function);

    static void checkFunction(uint8_t bus, uint8_t device, uint8_t function);

    static void checkDevice(uint8_t bus, uint8_t device);

    static void scanBus(uint8_t bus);

    static const IoPort configAddressPort;
    static const IoPort configDataPort;
    
    static Kernel::Logger log;
    static Util::Data::ArrayList<PciDevice> devices;

    static const constexpr uint16_t CONFIG_ADDRESS = 0xcf8;
    static const constexpr uint16_t CONFIG_DATA = 0xcfc;

    static const constexpr uint8_t MAX_DEVICES_PER_BUS = 32;
    static const constexpr uint8_t MAX_FUNCTIONS_PER_DEVICE = 8;
    static const constexpr uint16_t INVALID_VENDOR = 0xFFFF;
    static const constexpr uint8_t HEADER_TYPE_MULTIFUNCTION_BIT = 0x80;
};

}

#endif