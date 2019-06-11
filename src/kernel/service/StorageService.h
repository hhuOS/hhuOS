/*
 * Copyright (C) 2018 Burak Akguel, Christian Gesse, Fabian Ruhland, Filip Krakowski, Michael Schoettner
 * Heinrich-Heine University
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

#ifndef __StorageService_include__
#define __StorageService_include__

#include "kernel/event/storage/StorageAddEvent.h"
#include "kernel/event/storage/StorageRemoveEvent.h"
#include "lib/util/RingBuffer.h"
#include "KernelService.h"
#include "kernel/core/Symbols.h"
#include "device/storage/device/StorageDevice.h"
#include "kernel/log/Logger.h"

namespace Kernel {

/**
 * Manages all storage devices.
 * When a devices is added or removed, an event is published via the event bus.
 */
class StorageService : public KernelService {

private:
    Util::HashMap<String, StorageDevice *> devices;

public:
    /**
     * Constructor.
     */
    StorageService() = default;

    /**
     * Copy-constructor.
     */
    StorageService(const StorageService &other) = delete;

    /**
     * Destructor.
     */
    ~StorageService() = default;

    /**
     * Get a storage device for its name.
     *
     * CAUTION: If the name does not match a registered device, the return value is nullptr.
     *
     * @param name The device's name
     * @return The device
     */
    StorageDevice *getDevice(const String &name);

    /**
     * Register a storage device.
     *
     * @param device The device
     */
    void registerDevice(StorageDevice *device);

    /**
     * Remove a device.
     *
     * @param name The device's name
     */
    void removeDevice(const String &name);

    static constexpr const char *SERVICE_NAME = "StorageService";

    static Logger &log;
};

}

#endif