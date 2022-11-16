/*
 * Copyright (C) 2018-2022 Heinrich-Heine-Universitaet Duesseldorf,
 * Institute of Computer Science, Department Operating Systems
 * Burak Akguel, Christian Gesse, Fabian Ruhland, Filip Krakowski, Hannes Feil, Michael Schoettner
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

#include "NetworkDevice.h"
#include "kernel/system/System.h"
#include "kernel/service/NetworkService.h"

namespace Device::Network {

NetworkDevice::NetworkDevice() : Util::Stream::FilterInputStream(inputStream) {
    outputStream.connect(inputStream);
}

void NetworkDevice::write(uint8_t c) {
    Util::Exception::throwException(Util::Exception::UNSUPPORTED_OPERATION, "Network packets must be written wholly at once!");
}

void NetworkDevice::handlePacket(const uint8_t *packet, uint32_t length) {
    auto &ethernetModule = Kernel::System::getService<Kernel::NetworkService>().getEthernetModule();
    if (ethernetModule.checkPacket(packet, length)) {
        // Do not write checksum, since it has already been handled by checkPacket()
        outputStream.write(packet, 0, length - 4);
    }
}

}