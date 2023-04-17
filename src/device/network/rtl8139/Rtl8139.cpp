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
 *
 * The RTL8139 driver is based on a bachelor's thesis, written by Alexander Hansen.
 * The original source code can be found here: https://git.hhu.de/bsinfo/thesis/ba-alhan100
 */

#include "Rtl8139.h"

#include "device/pci/Pci.h"
#include "device/pci/PciDevice.h"
#include "kernel/system/System.h"
#include "kernel/service/NetworkService.h"
#include "lib/util/time/Timestamp.h"
#include "lib/util/async/Thread.h"
#include "kernel/service/MemoryService.h"
#include "kernel/service/InterruptService.h"
#include "kernel/log/Logger.h"
#include "lib/util/collection/Array.h"
#include "lib/util/base/Address.h"

namespace Kernel {
struct InterruptFrame;
enum InterruptVector : uint8_t;
}  // namespace Kernel

namespace Device::Network {

Kernel::Logger Rtl8139::log = Kernel::Logger::get("RTL8139");

Rtl8139::Rtl8139(const PciDevice &pciDevice) : pciDevice(pciDevice) {
    log.info("Configuring PCI registers");
    uint16_t command = pciDevice.readWord(Pci::COMMAND);
    command |= Pci::BUS_MASTER | Pci::IO_SPACE;
    pciDevice.writeWord(Pci::COMMAND, command);

    uint16_t ioBaseAddress = pciDevice.readDoubleWord(Pci::BASE_ADDRESS_0) & ~0x0f;
    baseRegister = IoPort(ioBaseAddress);

    log.info("Powering on device");
    baseRegister.writeByte(CONFIG_1, 0x00);

    log.info("Performing software reset");
    baseRegister.writeByte(COMMAND, RESET);
    while (baseRegister.readByte(COMMAND) & RESET) {
        Util::Async::Thread::sleep(Util::Time::Timestamp::ofMilliseconds(1));
    }

    log.info("Masking interrupts");
    baseRegister.writeWord(INTERRUPT_MASK, RECEIVE_OK | RECEIVE_ERROR | TRANSMIT_OK | TRANSMIT_ERROR);

    log.info("Enabling receiver/transmitter");
    baseRegister.writeByte(COMMAND, ENABLE_RECEIVER | ENABLE_TRANSMITTER);

    log.info("Configuring receive buffer");
    auto &memoryService = Kernel::System::getService<Kernel::MemoryService>();
    receiveBuffer = static_cast<uint8_t*>(memoryService.mapIO(BUFFER_SIZE));

    auto physicalReceiveBufferAddress = reinterpret_cast<uint32_t>(memoryService.getPhysicalAddress(receiveBuffer));
    baseRegister.writeDoubleWord(RECEIVE_BUFFER_START, physicalReceiveBufferAddress);
    baseRegister.writeDoubleWord(RECEIVE_CONFIGURATION, WRAP | ACCEPT_PHYSICAL_MATCH | ACCEPT_BROADCAST | LENGTH_8K);
}

void Rtl8139::initializeAvailableCards() {
    auto &networkService = Kernel::System::getService<Kernel::NetworkService>();
    auto devices = Pci::search(VENDOR_ID, DEVICE_ID);
    for (const auto &device : devices) {
        auto *rtl8139 = new Rtl8139(device);
        rtl8139->plugin();
        networkService.registerNetworkDevice(rtl8139, "eth");
    }
}

Util::Network::MacAddress Rtl8139::getMacAddress() const {
    uint8_t buffer[6] = {
            baseRegister.readByte(ID),
            baseRegister.readByte(ID + 1),
            baseRegister.readByte(ID + 2),
            baseRegister.readByte(ID + 3),
            baseRegister.readByte(ID + 4),
            baseRegister.readByte(ID + 5),
    };

    return Util::Network::MacAddress(buffer);
}

void Rtl8139::handleOutgoingPacket(const uint8_t *packet, uint32_t length) {
    while (!isTransmitDescriptorAvailable()) {
        Util::Async::Thread::sleep(Util::Time::Timestamp::ofMilliseconds(1));
    }

    auto &memoryService = Kernel::System::getService<Kernel::MemoryService>();
    auto physicalAddress = memoryService.getPhysicalAddress(const_cast<uint8_t*>(packet));
    setTransmitAddress(physicalAddress);
    setPacketSize(length);

    transmitDescriptor = (transmitDescriptor + 1) % TRANSMIT_DESCRIPTOR_COUNT;
}

void Rtl8139::plugin() {
    auto &interruptService = Kernel::System::getService<Kernel::InterruptService>();
    interruptService.allowHardwareInterrupt(pciDevice.getInterruptLine());
    interruptService.assignInterrupt(static_cast<Kernel::InterruptVector>(pciDevice.getInterruptLine() + 32), *this);
}

void Rtl8139::trigger(const Kernel::InterruptFrame &frame) {
    auto interrupt = baseRegister.readWord(INTERRUPT_STATUS);
    if (interrupt & RECEIVE_OK) {
        while (!(baseRegister.readByte(COMMAND) & BUFFER_EMPTY)) {
            processIncomingPacket();
        }
        baseRegister.writeWord(INTERRUPT_STATUS, RECEIVE_OK);
    } else if (interrupt & TRANSMIT_OK) {
        freeLastSendBuffer();
        baseRegister.writeWord(INTERRUPT_STATUS, TRANSMIT_OK);
    } else if (interrupt & TRANSMIT_ERROR) {
        baseRegister.writeWord(INTERRUPT_STATUS, TRANSMIT_ERROR);
    } else if (interrupt & RECEIVE_ERROR) {
        baseRegister.writeWord(INTERRUPT_STATUS, RECEIVE_ERROR);
    }
}

bool Rtl8139::isTransmitDescriptorAvailable() {
    auto status = baseRegister.readDoubleWord(TRANSMIT_STATUS + transmitDescriptor * 4);
    return (status & OWN);
}

void Rtl8139::setTransmitAddress(void *buffer) {
    baseRegister.writeDoubleWord(TRANSMIT_ADDRESS + transmitDescriptor * 4, reinterpret_cast<uint32_t>(buffer));
}

void Rtl8139::setPacketSize(uint32_t size) {
    baseRegister.writeDoubleWord(TRANSMIT_STATUS + transmitDescriptor * 4, size);
}

void Rtl8139::processIncomingPacket() {
    auto &header = *reinterpret_cast<PacketHeader*>(receiveBuffer + receiveIndex);
    if (header.status & RECEIVE_OK) {
        handleIncomingPacket(receiveBuffer + receiveIndex + sizeof(PacketHeader), header.length);
        receiveIndex += header.length + sizeof (PacketHeader); // Add packet length
        receiveIndex = Util::Address<uint32_t>(receiveIndex).alignUp(4).get(); // Align to next double word
        if (receiveIndex >= 8192) receiveIndex %= 8192; // Wrap around
        baseRegister.writeWord(CURRENT_READ_ADDRESS, receiveIndex - 16);
    }
}

}