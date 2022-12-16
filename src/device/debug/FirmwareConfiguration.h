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

#ifndef HHUOS_FIRMWARECONFIGURATION_H
#define HHUOS_FIRMWARECONFIGURATION_H

#include <cstdint>

#include "device/cpu/IoPort.h"
#include "lib/util/data/Array.h"
#include "lib/util/async/Spinlock.h"

namespace Device {

class FirmwareConfiguration {

public:

    struct File {
        uint32_t size;
        uint16_t selector;
        uint16_t reserved;
        char name[56];
    };

    /**
     * Default Constructor.
     */
    FirmwareConfiguration();

    /**
     * Copy Constructor.
     */
    FirmwareConfiguration(const FirmwareConfiguration &other) = delete;

    /**
     * Assignment operator.
     */
    FirmwareConfiguration &operator=(const FirmwareConfiguration &other) = delete;

    /**
     * Destructor.
     */
    ~FirmwareConfiguration() = default;

    static bool isAvailable();

    bool isDmaSupported() const;

    [[nodiscard]] Util::Data::Array<File> getFiles() const;

    uint64_t readFile(File &file, uint8_t *targetBuffer, uint32_t pos, uint32_t numBytes);

    uint64_t writeFile(File &file, const uint8_t *sourceBuffer, uint32_t pos, uint32_t numBytes);

private:

    enum Feature : uint32_t {
        TRADITIONAL = 1,
        DMA = 2
    };

    enum Selector : uint16_t {
        SIGNATURE = 0x0000,
        ID = 0x0001,
        ROOT_DIRECTORY = 0x0019
    };

    enum DmaCommand : uint16_t {
        ERROR = 0x0001,
        READ = 0x0002,
        SKIP = 0x0004,
        SELECT = 0x0008,
        WRITE = 0x0010
    };

    struct DmaAccess {
        uint32_t control;
        uint32_t length;
        uint64_t address;
    };

    uint64_t readTraditional(File &file, uint8_t *targetBuffer, uint32_t pos, uint32_t numBytes);

    uint64_t readDma(File &file, uint8_t *targetBuffer, uint32_t pos, uint32_t numBytes);

    uint64_t writeDma(File &file, const uint8_t *sourceBuffer, uint32_t pos, uint32_t numBytes);

    uint64_t performDmaAccess(File &file, uint8_t *targetBuffer, uint32_t pos, uint32_t numBytes, DmaCommand command);

    static bool waitDma(const DmaAccess &dmaAccess);

    IoPort selectorPort = IoPort(0x510);
    IoPort dataPort = IoPort(0x511);
    IoPort dmaPort = IoPort(0x518);

    Util::Async::Spinlock readWriteLock;
    uint32_t features;
};

}

#endif
