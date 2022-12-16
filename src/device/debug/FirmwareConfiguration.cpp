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

#include <endian.h>

#include "FirmwareConfiguration.h"
#include "kernel/system/System.h"
#include "kernel/service/MemoryService.h"
#include "lib/util/memory/Address.h"

Device::FirmwareConfiguration::FirmwareConfiguration() {
    selectorPort.writeWord(ID);
    features = dataPort.readByte() | dataPort.readByte() << 8 | dataPort.readByte() << 16 | dataPort.readByte() << 24;
}

bool Device::FirmwareConfiguration::isAvailable() {
    IoPort selectorPort(0x510);
    IoPort dataPort(0x511);

    selectorPort.writeWord(SIGNATURE);
    char id[4]{static_cast<char>(dataPort.readByte()), static_cast<char>(dataPort.readByte()), static_cast<char>(dataPort.readByte()), static_cast<char>(dataPort.readByte())};

    return id[0] == 'Q' && id[1] == 'E' && id[2] == 'M' && id[3] == 'U';
}

bool Device::FirmwareConfiguration::isDmaSupported() const {
    return (features & DMA) == DMA;
}

Util::Data::Array<Device::FirmwareConfiguration::File> Device::FirmwareConfiguration::getFiles() const {
    selectorPort.writeWord(ROOT_DIRECTORY);
    uint32_t fileCount = dataPort.readByte() << 24 | dataPort.readByte() << 16 | dataPort.readByte() << 8 | dataPort.readByte();
    auto files = Util::Data::Array<File>(fileCount);

    for (uint32_t i = 0; i < fileCount; i++) {
        File *file = &files[i];
        file->size = dataPort.readByte() << 24 | dataPort.readByte() << 16 | dataPort.readByte() << 8 | dataPort.readByte();
        file->selector = dataPort.readByte() << 8 | dataPort.readByte();
        file->reserved = dataPort.readByte() << 8 | dataPort.readByte();

        for (char &j : file->name) {
            j = dataPort.readByte();
        }
    }

    return files;
}

uint64_t Device::FirmwareConfiguration::readFile(File &file, uint8_t *targetBuffer, uint32_t pos, uint32_t numBytes) {
    uint64_t ret;
    readWriteLock.acquire();

    if (isDmaSupported()) {
        ret = readDma(file, targetBuffer, pos, numBytes);
    } else {
        ret = readTraditional(file, targetBuffer, pos, numBytes);
    }

    readWriteLock.release();
    return ret;
}

uint64_t Device::FirmwareConfiguration::writeFile(Device::FirmwareConfiguration::File &file, const uint8_t *sourceBuffer, uint32_t pos, uint32_t numBytes) {
    uint64_t ret;
    readWriteLock.acquire();

    if (isDmaSupported()) {
        ret = writeDma(file, sourceBuffer, pos, numBytes);
    } else {
        ret = 0;
    }

    readWriteLock.release();
    return ret;
}

uint64_t Device::FirmwareConfiguration::readTraditional(Device::FirmwareConfiguration::File &file, uint8_t *targetBuffer, uint32_t pos, uint32_t numBytes) {
    if (pos >= file.size) {
        return 0;
    }

    selectorPort.writeWord(file.selector);

    // Discard data until pos
    for (uint64_t i = 0; i < pos; i++) {
        dataPort.readByte();
    }

    // Read wanted data
    uint64_t max = pos + numBytes > file.size ? file.size : pos + numBytes;
    uint64_t read;
    for (read = 0; read < max; read++) {
        targetBuffer[read] = dataPort.readByte();
    }

    // Discard data after numBytes
    for (uint64_t i = pos + read; i < file.size; i++) {
        dataPort.readByte();
    }

    return read;
}

uint64_t Device::FirmwareConfiguration::readDma(Device::FirmwareConfiguration::File &file, uint8_t *targetBuffer, uint32_t pos, uint32_t numBytes) {
    return performDmaAccess(file, targetBuffer, pos, numBytes, READ);
}

uint64_t Device::FirmwareConfiguration::writeDma(Device::FirmwareConfiguration::File &file, const uint8_t *sourceBuffer, uint32_t pos, uint32_t numBytes) {
    return performDmaAccess(file, const_cast<uint8_t*>(sourceBuffer), pos, numBytes, WRITE);
}

uint64_t Device::FirmwareConfiguration::performDmaAccess(Device::FirmwareConfiguration::File &file, uint8_t *targetBuffer, uint32_t pos, uint32_t numBytes, Device::FirmwareConfiguration::DmaCommand command) {
    if (pos >= file.size) {
        return 0;
    }

    auto &memoryService = Kernel::System::getService<Kernel::MemoryService>();
    auto *dmaAccess = static_cast<DmaAccess*>(memoryService.mapIO(sizeof(DmaAccess)));
    void *physicalDmaAccessAddress = memoryService.getPhysicalAddress(dmaAccess);

    // Select file
    dmaAccess->control = htobe32(file.selector << 16 | SELECT);
    dmaAccess->length = 0;
    dmaAccess->address = 0;

    dmaPort.writeDoubleWord(htobe32(reinterpret_cast<uint32_t>(physicalDmaAccessAddress)));
    if (!waitDma(*dmaAccess)) {
        delete dmaAccess;
        return 0;
    }

    // Skip to pos
    dmaAccess->control = htobe32(SKIP);
    dmaAccess->length = htobe32(pos);
    dmaAccess->address = 0;

    dmaPort.writeDoubleWord(htobe32(reinterpret_cast<uint32_t>(physicalDmaAccessAddress)));
    if (!waitDma(*dmaAccess)) {
        delete dmaAccess;
        return 0;
    }

    // Read/Write data
    void *virtualAddress = memoryService.mapIO(numBytes);
    void *physicalAddress = memoryService.getPhysicalAddress(virtualAddress);
    uint64_t max = pos + numBytes > file.size ? file.size : pos + numBytes;

    if (command == WRITE) {
        auto source = Util::Memory::Address<uint32_t>(targetBuffer);
        auto target = Util::Memory::Address<uint32_t>(virtualAddress);
        target.copyRange(source, max);
    }

    dmaAccess->control = htobe32(command);
    dmaAccess->length = htobe32(max);
    dmaAccess->address = htobe64(reinterpret_cast<uint64_t>(physicalAddress));

    dmaPort.writeDoubleWord(htobe32(reinterpret_cast<uint32_t>(physicalDmaAccessAddress)));
    if (!waitDma(*dmaAccess)) {
        delete dmaAccess;
        delete reinterpret_cast<uint8_t*>(virtualAddress);
        return 0;
    }

    if (command == READ) {
        auto source = Util::Memory::Address<uint32_t>(virtualAddress);
        auto target = Util::Memory::Address<uint32_t>(targetBuffer);
        target.copyRange(source, max);
    }

    delete dmaAccess;
    delete reinterpret_cast<uint8_t*>(virtualAddress);
    return max;
}

bool Device::FirmwareConfiguration::waitDma(const DmaAccess &dmaAccess) {
    uint32_t control;

    do {
        control = be32toh(dmaAccess.control);
        if (control == ERROR) {
            return false;
        }
    } while (control != 0);

    return true;
}
