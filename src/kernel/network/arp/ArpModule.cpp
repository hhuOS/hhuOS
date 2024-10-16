/*
 * Copyright (C) 2018-2024 Heinrich-Heine-Universitaet Duesseldorf,
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
 * The UDP/IP stack is based on a bachelor's thesis, written by Hannes Feil.
 * The original source code can be found here: https://github.com/hhuOS/hhuOS/tree/legacy/network
 */

#include "ArpModule.h"

#include "lib/util/async/Thread.h"
#include "device/network/NetworkDevice.h"
#include "kernel/log/Log.h"
#include "lib/util/base/Exception.h"
#include "lib/util/io/stream/ByteArrayInputStream.h"
#include "lib/util/io/stream/ByteArrayOutputStream.h"
#include "lib/util/time/Timestamp.h"
#include "lib/util/network/NetworkAddress.h"
#include "kernel/network/arp/ArpEntry.h"
#include "kernel/network/arp/ArpHeader.h"
#include "lib/util/network/ethernet/EthernetHeader.h"
#include "kernel/network/ethernet/EthernetModule.h"
#include "lib/util/network/ip4/Ip4Address.h"
#include "kernel/network/ip4/Ip4Interface.h"
#include "lib/util/collection/Iterator.h"

namespace Util {
namespace Io {
class OutputStream;
}  // namespace Stream
}  // namespace Util

namespace Kernel::Network::Arp {

void ArpModule::readPacket(Util::Io::ByteArrayInputStream &stream, [[maybe_unused]] LayerInformation information, Device::Network::NetworkDevice &device) {
    auto arpHeader = ArpHeader();
    arpHeader.read(stream);

    if (arpHeader.getHardwareAddressType() != ArpHeader::ETHERNET) {
        LOG_WARN("Discarding packet, because of unsupported hardware address type 0x%04x", arpHeader.getHardwareAddressType());
        return;
    }

    if (arpHeader.getProtocolAddressType() != ArpHeader::IP4) {
        LOG_WARN("Discarding packet, because of unsupported protocol address type 0x%04x", arpHeader.getProtocolAddressType());
        return;
    }

    auto sourceMacAddress = Util::Network::MacAddress();
    auto targetMacAddress = Util::Network::MacAddress();
    auto sourceIpAddress = Util::Network::Ip4::Ip4Address();
    auto targetIpAddress = Util::Network::Ip4::Ip4Address();

    sourceMacAddress.read(stream);
    sourceIpAddress.read(stream);
    targetMacAddress.read(stream);
    targetIpAddress.read(stream);

    switch (arpHeader.getOperation()) {
        case ArpHeader::REQUEST:
            handleRequest(sourceMacAddress, sourceIpAddress, targetIpAddress, device);
            break;
        case ArpHeader::REPLY:
            handleReply(sourceMacAddress, sourceIpAddress, targetMacAddress, targetIpAddress);
            break;
        default:
            LOG_WARN("Discarding packet, because of unsupported operation type 0x%04x", arpHeader.getOperation());
    }
}

bool ArpModule::resolveAddress(const Util::Network::Ip4::Ip4Address &protocolAddress, Util::Network::MacAddress &hardwareAddress, const Ip4::Ip4Interface &interface) {
    for (uint32_t i = 0; i < MAX_REQUEST_RETRIES; i++) {
        lock.acquire();
        if (hasHardwareAddress(protocolAddress)) {
            hardwareAddress = getHardwareAddress(protocolAddress);
            return lock.releaseAndReturn(true);
        }
        lock.release();

        auto &device = interface.getDevice();
        auto ipAddress = interface.getIp4Address();
        auto packet = Util::Io::ByteArrayOutputStream();
        writeHeader(packet, ArpHeader::REQUEST, interface.getDevice(), Util::Network::MacAddress::createBroadcastAddress());

        device.getMacAddress().write(packet);
        interface.getIp4Address().write(packet);
        Util::Network::MacAddress().write(packet);
        protocolAddress.write(packet);

        Ethernet::EthernetModule::finalizePacket(packet);
        device.sendPacket(packet.getBuffer(), packet.getLength());

        Util::Async::Thread::sleep(Util::Time::Timestamp::ofMicroseconds(100));
    }

    return false;
}

void ArpModule::setEntry(const Util::Network::Ip4::Ip4Address &protocolAddress, const Util::Network::MacAddress &hardwareAddress) {
    lock.acquire();
    for (auto &entry : arpCache) {
        if (entry.getProtocolAddress() == protocolAddress) {
            entry.setHardwareAddress(hardwareAddress);
            lock.release();
            return;
        }
    }

    arpCache.add(ArpEntry{protocolAddress, hardwareAddress});
    lock.release();
}

void ArpModule::removeEntry(const Util::Network::Ip4::Ip4Address &protocolAddress) {
    for (auto &entry : arpCache) {
        if (entry.getProtocolAddress() == protocolAddress) {
            arpCache.remove(entry);
            break;
        }
    }

    lock.release();
}

Util::Network::MacAddress ArpModule::getHardwareAddress(const Util::Network::Ip4::Ip4Address &protocolAddress) {
    lock.acquire();

    for (const auto &entry : arpCache) {
        if (entry.getProtocolAddress() == protocolAddress) {
            return lock.releaseAndReturn(entry.getHardwareAddress());
        }
    }

    lock.release();
    Util::Exception::throwException(Util::Exception::INVALID_ARGUMENT, "ArpModule: Protocol address not found!");
}

bool ArpModule::hasHardwareAddress(const Util::Network::Ip4::Ip4Address &protocolAddress) {
    lock.acquire();

    for (const auto &entry : arpCache) {
        if (entry.getProtocolAddress() == protocolAddress) {
            return lock.releaseAndReturn(true);
        }
    }

    return lock.releaseAndReturn(false);
}

void ArpModule::handleRequest(const Util::Network::MacAddress &sourceHardwareAddress, const Util::Network::Ip4::Ip4Address &sourceAddress,
                              const Util::Network::Ip4::Ip4Address &targetProtocolAddress, Device::Network::NetworkDevice &device) {
    setEntry(sourceAddress, sourceHardwareAddress);

    lock.acquire();
    if (hasHardwareAddress(targetProtocolAddress)) {
        auto targetHardwareAddress = getHardwareAddress(targetProtocolAddress);
        lock.release();

        auto packet = Util::Io::ByteArrayOutputStream();
        writeHeader(packet, ArpHeader::REPLY, device, targetHardwareAddress);

        device.getMacAddress().write(packet);
        targetProtocolAddress.write(packet);
        sourceHardwareAddress.write(packet);
        sourceAddress.write(packet);

        Ethernet::EthernetModule::finalizePacket(packet);
        device.sendPacket(packet.getBuffer(), packet.getLength());
    } else {
        lock.release();
    }
}

void ArpModule::handleReply(const Util::Network::MacAddress &sourceHardwareAddress, const Util::Network::Ip4::Ip4Address &sourceAddress,
                            const Util::Network::MacAddress &targetHardwareAddress, const Util::Network::Ip4::Ip4Address &targetProtocolAddress) {
    setEntry(sourceAddress, sourceHardwareAddress);

    //Learn own addresses if not broadcast
    if (!targetHardwareAddress.isBroadcastAddress()) {
        setEntry(targetProtocolAddress, targetHardwareAddress);
    }
}

void ArpModule::writeHeader(Util::Io::OutputStream &stream, ArpHeader::Operation operation, Device::Network::NetworkDevice &device, const Util::Network::MacAddress &destinationAddress) {
    Ethernet::EthernetModule::writeHeader(stream, device, destinationAddress, Util::Network::Ethernet::EthernetHeader::ARP);

    auto header = ArpHeader();
    header.setOperation(operation);
    header.write(stream);
}

}