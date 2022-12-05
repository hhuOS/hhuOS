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

#include "NetworkService.h"
#include "device/network/NetworkFilesystemDriver.h"

void Kernel::NetworkService::registerNetworkDevice(Device::Network::NetworkDevice *device) {
    devices.add(device);
    Device::Network::NetworkFilesystemDriver::mount(*device);
}

Device::Network::NetworkDevice& Kernel::NetworkService::getNetworkDevice(const Util::Memory::String &identifier) {
    for (auto *device : devices) {
        if (device->getIdentifier() == identifier) {
            return *device;
        }
    }

    Util::Exception::throwException(Util::Exception::INVALID_ARGUMENT, "NetworkService: Device not found!");
}

::Network::NetworkStack &Kernel::NetworkService::getNetworkStack() {
    return networkStack;
}

void Kernel::NetworkService::registerIp4Route(const Network::Ip4::Ip4Route &route) {
    networkStack.getIp4Module().getRoutingModule().addRoute(route);
}

void Kernel::NetworkService::setDefaultRoute(const Network::Ip4::Ip4Route &route) {
    networkStack.getIp4Module().getRoutingModule().setDefaultRoute(route);
}
