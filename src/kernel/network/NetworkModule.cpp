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
 *
 * The UDP/IP stack is based on a bachelor's thesis, written by Hannes Feil.
 * The original source code can be found here: https://github.com/hhuOS/hhuOS/tree/legacy/network
 */

#include "NetworkModule.h"

namespace Device {
namespace Network {
class NetworkDevice;
}  // namespace Network
}  // namespace Device
namespace Util {
namespace Stream {
class ByteArrayInputStream;
}  // namespace Stream
}  // namespace Util

namespace Kernel::Network {
class Socket;

bool NetworkModule::registerSocket(Socket &socket) {
    socketLock.acquire();
    socketList.add(&socket);
    return socketLock.releaseAndReturn(true);
}

void NetworkModule::deregisterSocket(Socket &socket) {
    socketLock.acquire();
    socketList.remove(&socket);
    socketLock.release();
}

void NetworkModule::registerNextLayerModule(uint32_t protocolId, NetworkModule &module) {
    nextLayerModules.put(protocolId, &module);
}

bool NetworkModule::isNextLayerTypeSupported(uint32_t protocolId) {
    return nextLayerModules.containsKey(protocolId);
}

void NetworkModule::invokeNextLayerModule(uint32_t protocolId, LayerInformation information, Util::Stream::ByteArrayInputStream &stream, Device::Network::NetworkDevice &device) {
    if (isNextLayerTypeSupported(protocolId)) {
        auto *module = nextLayerModules.get(protocolId);
        module->readPacket(stream, information, device);
    }
}

}