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

#include <kernel/Kernel.h>
#include <devices/isa/Isa.h>
#include "SoundBlaster.h"
#include "SoundBlaster1.h"
#include "SoundBlaster2.h"
#include "SoundBlasterPro.h"
#include "SoundBlaster16.h"

Logger &SoundBlaster::log = Logger::get("SOUNDBLASTER");

bool SoundBlaster::checkPort(uint16_t baseAddress) {
    auto *timeService = Kernel::getService<TimeService>();
    IOport resetPort(static_cast<uint16_t>(baseAddress + 0x06));
    IOport readDataPort(static_cast<uint16_t>(baseAddress + 0x0a));
    IOport readBufferStatusPort(static_cast<uint16_t>(baseAddress + 0x0e));

    // Issue reset command
    resetPort.outb(0x01);
    timeService->msleep(5);
    resetPort.outb(0x00);

    bool timeout = true;
    uint32_t timeoutTime = timeService->getSystemTime() + TIMEOUT;

    // Wait for read buffer to become ready
    do {
        uint8_t status = readBufferStatusPort.inb();

        if((status & 0x80) == 0x80) {
            timeout = false;
            break;
        }
    } while(timeService->getSystemTime() < timeoutTime);

    if(timeout) {
        return false;
    }

    timeoutTime = timeService->getSystemTime() + TIMEOUT;

    // Wait for ready code (represented by 0xaa) to appear in the read buffer
    do {
        uint8_t status = readDataPort.inb();

        if(status == 0xaa) {
            return true;
        }
    } while(timeService->getSystemTime() < timeoutTime);

    return false;
}

uint16_t SoundBlaster::getBasePort() {
    for(uint16_t i = FIRST_BASE_ADDRESS; i <= LAST_BASE_ADDRESS; i += 0x10) {
        if(checkPort(i)) {
            return i;
        }
    }

    return 0;
}

bool SoundBlaster::isAvailable() {
    return getBasePort() != 0;
}

SoundBlaster::SoundBlaster(uint16_t baseAddress) :
        baseAddress(baseAddress),
        resetPort(static_cast<uint16_t>(baseAddress + 0x06)),
        readDataPort(static_cast<uint16_t>(baseAddress + 0x0a)),
        writeDataPort(static_cast<uint16_t>(baseAddress + 0x0c)),
        readBufferStatusPort(static_cast<uint16_t>(baseAddress + 0x0e)),
        timeService(Kernel::getService<TimeService>()) {
    log.info("Found base port at address 0x%04x", baseAddress);

    // Reset card
    log.info("Resetting card...");
    bool ret = reset();
    log.info(ret ? "Successfully resetted card" : "Unable to reset card");

    // Get DSP Version
    writeToDSP(0xe1);
    majorVersion = readFromDSP();
    minorVersion = readFromDSP();

    log.info("Major version: 0x%02x, Minor version: 0x%02x", majorVersion, minorVersion);

    dmaMemory = Isa::allocDmaBuffer();
}

SoundBlaster *SoundBlaster::initialize() {
    if(!isAvailable()) {
        return nullptr;
    }

    uint16_t baseAddress = getBasePort();

    IOport readDataPort(static_cast<uint16_t>(baseAddress + 0x0a));
    IOport readBufferStatusPort(static_cast<uint16_t>(baseAddress + 0x0e));
    IOport writeDataPort(static_cast<uint16_t>(baseAddress + 0x0c));

    // Issue version command
    while((readBufferStatusPort.inb() & 0x80) == 0x80);
    writeDataPort.outb(0xe1);

    // Get version
    while((readBufferStatusPort.inb() & 0x80) != 0x80);
    uint8_t majorVersion = readDataPort.inb();

    if(majorVersion == 1) {
        return new SoundBlaster1(baseAddress);
    }

    if(majorVersion == 2) {
        return new SoundBlaster2(baseAddress);
    }

    if(majorVersion == 3) {
        return new SoundBlasterPro(baseAddress);
    }

    if(majorVersion >= 4) {
        return new SoundBlaster16(baseAddress);
    }

    return nullptr;
}

String SoundBlaster::getVendorName() {
    return VENDOR_NAME;
}

String SoundBlaster::getDeviceName() {
    if(majorVersion < 0x02) {
        return SOUND_BLASTER_1;
    }

    if(majorVersion < 0x03) {
        return SOUND_BLASTER_2;
    }

    if(majorVersion < 0x04) {
        return SOUND_BLASTER_PRO;
    }

    if(majorVersion == 0x04 && minorVersion < 0x0c) {
        return SOUND_BLASTER_16;
    }

    return SOUND_BLASTER_AWE32;
}

uint8_t SoundBlaster::readFromDSP() {
    while((readBufferStatusPort.inb() & 0x80) != 0x80);

    return readDataPort.inb();
}

void SoundBlaster::writeToDSP(uint8_t value) {
    while((readBufferStatusPort.inb() & 0x80) == 0x80);

    writeDataPort.outb(value);
}

uint8_t SoundBlaster::readFromADC() {
    writeToDSP(0x20);
    return readFromDSP();
}

void SoundBlaster::writeToDAC(uint8_t value) {
    writeToDSP(0x10);
    writeToDSP(value);
}

void SoundBlaster::turnSpeakerOn() {
    writeToDSP(0xd1);
}

void SoundBlaster::turnSpeakerOff() {
    writeToDSP(0xd3);
}

bool SoundBlaster::reset() {
    // Issue reset command
    resetPort.outb(0x01);
    timeService->msleep(5);
    resetPort.outb(0x00);

    bool timeout = true;
    uint32_t timeoutTime = timeService->getSystemTime() + TIMEOUT;

    // Wait for read buffer to become ready
    do {
        uint8_t status = readBufferStatusPort.inb();

        if((status & 0x80) == 0x80) {
            timeout = false;
            break;
        }
    } while(timeService->getSystemTime() < timeoutTime);

    if(timeout) {
        return false;
    }

    timeoutTime = timeService->getSystemTime() + TIMEOUT;

    // Wait for ready code (represented by 0xaa) to appear in the read buffer
    do {
        uint8_t status = readDataPort.inb();

        if(status == 0xaa) {
            return true;
        }
    } while(timeService->getSystemTime() < timeoutTime);

    return false;
}