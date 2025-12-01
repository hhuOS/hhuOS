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
 * The RTL8139 driver is based on a bachelor's thesis, written by Alexander Hansen.
 * The original source code can be found here: https://git.hhu.de/bsinfo/thesis/ba-alhan100
 */

#ifndef HHUOS_RTL8139_H
#define HHUOS_RTL8139_H

#include <stdint.h>

#include "device/network/NetworkDevice.h"
#include "device/bus/pci/PciDevice.h"
#include "kernel/interrupt/InterruptHandler.h"
#include "device/cpu/IoPort.h"
#include "lib/util/network/MacAddress.h"
#include "lib/util/base/Constants.h"

namespace Kernel {
enum InterruptVector : uint8_t;
struct InterruptFrame;
}  // namespace Kernel

namespace Device::Network {

class Rtl8139 : public NetworkDevice, Kernel::InterruptHandler {

public:
    /**
     * Default Constructor.
     */
    explicit Rtl8139(const PciDevice &pciDevice);

    /**
     * Copy Constructor.
     */
    Rtl8139(const Rtl8139 &other) = delete;

    /**
     * Assignment operator.
     */
    Rtl8139 &operator=(const Rtl8139 &other) = delete;

    /**
     * Destructor.
     */
    ~Rtl8139() override = default;

    static void initializeAvailableCards();

    Util::Network::MacAddress getMacAddress() const override;

    void plugin() override;

    void trigger(const Kernel::InterruptFrame &frame, Kernel::InterruptVector slot) override;

protected:

    void handleOutgoingPacket(const uint8_t *packet, uint32_t length) override;

private:
    
    enum Register : uint8_t {
        ID = 0x00,
        TRANSMIT_STATUS = 0x10,
        TRANSMIT_ADDRESS = 0x20,
        COMMAND = 0x37,
        RECEIVE_BUFFER_START = 0x30,
        CURRENT_READ_ADDRESS = 0x38,
        INTERRUPT_MASK = 0x3c,
        INTERRUPT_STATUS = 0x3e,
        RECEIVE_CONFIGURATION = 0x44,
        CONFIG_1 = 0x52,
    };

    enum Command : uint8_t {
        BUFFER_EMPTY = 0x01,
        ENABLE_TRANSMITTER = 0x04,
        ENABLE_RECEIVER = 0x08,
        RESET = 0x10
    };

    enum Interrupt : uint16_t {
        RECEIVE_OK = 0x0001,
        RECEIVE_ERROR = 0x0002,
        TRANSMIT_OK = 0x0004,
        TRANSMIT_ERROR = 0x0008,
        RX_BUFFER_OVERFLOW = 0x0010,
        PACKET_UNDERRUN_LINK_CHANGE = 0x0020,
        RX_FIFO_OVERFLOW = 0x0040,
        CABLE_LENGTH_CHANGE = 0x2000,
        TIMEOUT = 0x4000,
        SYSTEM_ERROR = 0x8000
    };

    enum ReceiveFlag : uint32_t {
        ACCEPT_ALL = 0x0001,
        ACCEPT_PHYSICAL_MATCH = 0x0002,
        ACCEPT_MULTICAST = 0x0004,
        ACCEPT_BROADCAST = 0x0008,
        ACCEPT_RUNT = 0x0010,
        ACCEPT_ERROR = 0x0020,
        WRAP = 0x0080,
        LENGTH_8K = 0x0000,
        LENGTH_16K = 0x0800,
        LENGTH_32K = 0x1000,
        LENGTH_64K = 0x1800
    };

    enum TransmitStatus : uint32_t {
        OWN = 0x2000,
        FIFO_UNDERRUN = 0x4000,
        TRANSMIT_STATUS_OK = 0x8000,
        EARLY_TX_THRESHOLD = 0x10000,
        TRANSMIT_STATUS_ABORT = 0x40000000,
        CARRIER_SENSE_LOST = 0x80000000
    };

    enum ReceiveStatus : uint16_t {
        OK = 0x0001,
        FRAME_ALIGNMENT_ERROR = 0x0002,
        CHECKSUM_ERROR = 0x0004,
        LONG_PACKET = 0x0008,
        RUNT_PACKET = 0x0010,
        INVALID_SYMBOL = 0x0020,
        BROADCAST = 0x2000,
        PHYSICAL_ADDRESS = 0x4000,
        MULTICAST = 0x8000
    };

    struct PacketHeader {
        uint16_t status;
        uint16_t length;
    };

    bool isTransmitDescriptorAvailable();

    void setTransmitAddress(void *buffer);

    void setPacketSize(uint32_t size);

    void processIncomingPacket();

    PciDevice pciDevice;
    uint8_t transmitDescriptor = 0;
    uint16_t receiveIndex = 0;
    uint8_t *receiveBuffer{};
    IoPort baseRegister = IoPort(0x00);

    static const constexpr uint16_t VENDOR_ID = 0x10ec;
    static const constexpr uint16_t DEVICE_ID = 0x8139;
    static const constexpr uint32_t BUFFER_SIZE = 8 * 1024 + 16 + 1500;
    static const constexpr uint32_t BUFFER_PAGES = BUFFER_SIZE % Util::PAGESIZE == 0 ? (BUFFER_SIZE / Util::PAGESIZE) : (BUFFER_SIZE / Util::PAGESIZE + 1);
    static const constexpr uint8_t TRANSMIT_DESCRIPTOR_COUNT = 4;
};

}

#endif
