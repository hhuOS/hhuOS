/*
 * Copyright (C) 2018-2023 Heinrich-Heine-Universitaet Duesseldorf,
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

#ifndef HHUOS_RTL8139_H
#define HHUOS_RTL8139_H

#include "device/network/NetworkDevice.h"
#include "device/pci/PciDevice.h"
#include "kernel/interrupt/InterruptHandler.h"

namespace Device::Network {

class Rtl8139 : public NetworkDevice, Kernel::InterruptHandler {

public:
    /**
     * Default Constructor.
     */
    Rtl8139(const Util::Memory::String &identifier, const PciDevice &pciDevice);

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

    [[nodiscard]] Util::Network::MacAddress getMacAddress() const override;

    void plugin() override;

    void trigger(const Kernel::InterruptFrame &frame) override;

protected:

    void handleOutgoingPacket(const uint8_t *packet, uint32_t length) override;

private:
    
    enum Register : uint8_t {
        ID = 0x00,
        COMMAND = 0x37,
        RECEIVE_BUFFER_START = 0x30,
        INTERRUPT_MASK = 0x3c,
        RECEIVE_CONFIGURATION = 0x44,
        CONFIG_1 = 0x52,
    };

    enum Command : uint8_t {
        BUFFER_EMPTY = 0x00,
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

    PciDevice pciDevice;
    uint8_t *receiveBuffer{};
    IoPort baseRegister = IoPort(0x00);

    static Kernel::Logger log;

    static const constexpr uint16_t VENDOR_ID = 0x10ec;
    static const constexpr uint16_t DEVICE_ID = 0x8139;
    static const constexpr uint32_t BUFFER_SIZE = 8 * 1024 + 16 + 1500;
};

}

#endif
