/*
 * Copyright (C) 2018-2025 Heinrich-Heine-Universitaet Duesseldorf,
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

#include "MacAddressNode.h"
#include "NetworkFilesystemDriver.h"
#include "kernel/service/FilesystemService.h"
#include "device/network/NetworkDevice.h"
#include "filesystem/Filesystem.h"
#include "lib/util/base/String.h"
#include "kernel/service/Service.h"

namespace Device::Network {

NetworkFilesystemDriver::NetworkFilesystemDriver(NetworkDevice &device) : device(device) {
    addNode("/", new MacAddressNode(device));
}

bool NetworkFilesystemDriver::mount(NetworkDevice &device) {
    auto &filesystemService = Kernel::Service::getService<Kernel::FilesystemService>();
    auto path = "/device/" + device.getIdentifier();
    auto *driver = new NetworkFilesystemDriver(device);

    filesystemService.createDirectory(path);
    return filesystemService.getFilesystem().mountVirtualDriver(path, driver);
}

}