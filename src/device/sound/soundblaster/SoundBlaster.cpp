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
 */

#include "SoundBlaster.h"
#include "SoundBlasterRunnable.h"
#include "SoundBlasterNode.h"
#include "kernel/system/System.h"
#include "kernel/service/MemoryService.h"
#include "lib/util/base/Constants.h"
#include "lib/util/async/Thread.h"
#include "lib/util/time/Timestamp.h"
#include "kernel/service/InterruptService.h"
#include "device/isa/Isa.h"
#include "kernel/service/SchedulerService.h"
#include "kernel/service/ProcessService.h"
#include "kernel/service/FilesystemService.h"
#include "filesystem/memory/MemoryDriver.h"

namespace Device {

Kernel::Logger SoundBlaster::log = Kernel::Logger::get("SoundBlaster");

SoundBlaster::SoundBlaster(uint16_t baseAddress, uint8_t irqNumber, uint8_t dmaChannel) :
        resetPort(baseAddress + 0x06),
        readDataPort(baseAddress + 0x0a),
        writeDataPort(baseAddress + 0x0c),
        readBufferStatusPort(baseAddress + 0x0e),
        irqNumber(irqNumber),
        dmaChannel(dmaChannel) {}

SoundBlaster::~SoundBlaster() {
    Kernel::System::getService<Kernel::MemoryService>().freeLowerMemory(dmaBuffer);
}

bool SoundBlaster::isAvailable() {
    return getBasePort() != 0;
}

bool SoundBlaster::checkPort(uint16_t baseAddress) {
    auto resetPort = IoPort(static_cast<uint16_t>(baseAddress + 0x06));
    auto readDataPort = IoPort(static_cast<uint16_t>(baseAddress + 0x0a));
    auto readBufferStatusPort = IoPort(static_cast<uint16_t>(baseAddress + 0x0e));

    // Issue reset command
    resetPort.writeByte(0x01);
    Util::Async::Thread::sleep(Util::Time::Timestamp::ofMilliseconds(5));
    resetPort.writeByte(0x00);

    bool timeout = true;
    uint32_t timeoutTime = Util::Time::getSystemTime().toMilliseconds() + TIMEOUT;

    // Wait for read buffer to become ready
    do {
        uint8_t status = readBufferStatusPort.readByte();

        if ((status & 0x80) == 0x80) {
            timeout = false;
            break;
        }
    } while (Util::Time::getSystemTime().toMilliseconds() < timeoutTime);

    if (timeout) {
        return false;
    }

    timeoutTime = Util::Time::getSystemTime().toMilliseconds() + TIMEOUT;

    // Wait for ready code (represented by 0xaa) to appear in the read buffer
    do {
        uint8_t status = readDataPort.readByte();

        if(status == 0xaa) {
            return true;
        }
    } while (Util::Time::getSystemTime().toMilliseconds() < timeoutTime);

    return false;
}

uint16_t SoundBlaster::getBasePort() {
    for (uint16_t i = FIRST_BASE_ADDRESS; i <= LAST_BASE_ADDRESS; i += 0x10) {
        if (checkPort(i)) {
            return i;
        }
    }

    return 0;
}

bool SoundBlaster::initialize() {
    uint16_t baseAddress = getBasePort();
    if (baseAddress == 0) {
        return false;
    }

    log.info("Found base port at address [%x]", baseAddress);
    auto readDataPort = IoPort(baseAddress + 0x0a);
    auto readBufferStatusPort = IoPort(baseAddress + 0x0e);
    auto writeDataPort = IoPort(baseAddress + 0x0c);

    // Issue version command
    while((readBufferStatusPort.readByte() & 0x80) == 0x80);
    writeDataPort.writeByte(0xe1);

    // Get version
    while((readBufferStatusPort.readByte() & 0x80) != 0x80);
    uint8_t majorVersion = readDataPort.readByte();
    while((readBufferStatusPort.readByte() & 0x80) != 0x80);
    uint8_t minorVersion = readDataPort.readByte();
    log.info("Major version: [%x], Minor version: [%x]", majorVersion, minorVersion);

    auto &processService = Kernel::System::getService<Kernel::ProcessService>();
    auto &schedulerService = Kernel::System::getService<Kernel::SchedulerService>();
    auto &filesystemService = Kernel::System::getService<Kernel::FilesystemService>();

    auto *soundBlaster = new SoundBlaster(baseAddress);
    auto *soundBlasterRunnable = new SoundBlasterRunnable(*soundBlaster);
    auto &thread = Kernel::Thread::createKernelThread("Sound-Blaster", processService.getKernelProcess(), soundBlasterRunnable);
    auto *soundBlasterNode = new SoundBlasterNode(soundBlaster, *soundBlasterRunnable, thread);

    filesystemService.getFilesystem().getVirtualDriver("/device").addNode("/", soundBlasterNode);
    schedulerService.ready(thread);

    soundBlaster->plugin();
    return true;
}

bool SoundBlaster::reset() {
    // Issue reset command
    resetPort.writeByte(0x01);
    Util::Async::Thread::sleep(Util::Time::Timestamp::ofMilliseconds(5));
    resetPort.writeByte(0x00);

    bool timeout = true;
    uint32_t timeoutTime = Util::Time::getSystemTime().toMilliseconds() + TIMEOUT;

    do {
        uint8_t status = readBufferStatusPort.readByte();

        if ((status & 0x80) == 0x80) {
            timeout = false;
            break;
        }
    } while(Util::Time::getSystemTime().toMilliseconds() < timeoutTime);

    if (timeout) {
        return false;
    }

    timeoutTime = Util::Time::getSystemTime().toMilliseconds() + TIMEOUT;

    // Wait for ready code (represented by 0xaa) to appear in the read buffer
    do {
        uint8_t status = readDataPort.readByte();

        if(status == 0xaa) {
            return true;
        }
    } while(Util::Time::getSystemTime().toMilliseconds() < timeoutTime);

    return false;
}

void SoundBlaster::plugin() {
    // Older DSPs (version < 4) don't support manual IRQ- and DMA-configuration.
    // They must be configured via jumpers and there is no real way to get the IRQ- and DMA-numbers in software.
    // We just assume the DSP to use IRQ 10 and DMA channel 1, if not specified else in the constructor.
    auto &interruptService = Kernel::System::getService<Kernel::InterruptService>();
    interruptService.assignInterrupt(static_cast<Kernel::InterruptVector>(32 + irqNumber), *this);
    interruptService.allowHardwareInterrupt(static_cast<InterruptRequest>(irqNumber));
}

uint8_t SoundBlaster::readFromDSP() {
    while (!(readBufferStatusPort.readByte() & 0x80));
    return readDataPort.readByte();
}

void SoundBlaster::writeToDSP(uint8_t value) {
    while (readBufferStatusPort.readByte() & 0x80);
    writeDataPort.writeByte(value);
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
    lock.acquire();
    writeToDSP(0xd1);
    lock.release();
}

void SoundBlaster::turnSpeakerOff() {
    lock.acquire();
    writeToDSP(0xd3);
    lock.release();
}

void SoundBlaster::ackInterrupt() {
    readBufferStatusPort.readByte();
}

void SoundBlaster::waitForInterrupt() {
    while (!receivedInterrupt) {
        Util::Async::Thread::yield();
    }

    receivedInterrupt = false;
}

void SoundBlaster::trigger(const Kernel::InterruptFrame &frame) {
    receivedInterrupt = true;
    ackInterrupt();
}

uint8_t* SoundBlaster::getDmaBuffer() const {
    return dmaBuffer;
}

bool SoundBlaster::setAudioParameters(uint16_t sampleRate, uint8_t channels, uint8_t bits) {
    lock.acquire();
    if (channels > 1 || bits != 8) {
        lock.release();
        return false;
    }

    auto timeConstant = static_cast<uint16_t>(65536 - (256000000 / sampleRate));

    writeToDSP(0x40);
    writeToDSP(static_cast<uint8_t>((timeConstant & 0xff00) >> 8));

    auto &memoryService = Kernel::System::getService<Kernel::MemoryService>();
    dmaBufferSize = static_cast<uint32_t>(AUDIO_BUFFER_SIZE * sampleRate * (bits / 8.0) * channels);
    if (dmaBufferSize % 2 == 1) {
        dmaBufferSize++;
    }

    memoryService.freeLowerMemory(dmaBuffer);
    dmaBuffer = static_cast<uint8_t*>(memoryService.allocateLowerMemory(dmaBufferSize, Util::PAGESIZE));
    physicalDmaAddress = static_cast<uint8_t*>(memoryService.getPhysicalAddress(dmaBuffer));

    lock.release();
    return true;
}

uint32_t SoundBlaster::getDmaBufferSize() const {
    return dmaBufferSize;
}

void SoundBlaster::setBufferSize(uint32_t size) {
    writeToDSP(0x14);
    writeToDSP(static_cast<uint8_t>((size - 1) & 0x00ff));
    writeToDSP(static_cast<uint8_t>(((size - 1) & 0xff00) >> 8));
}

void SoundBlaster::prepareDma(uint32_t offset, uint32_t size) const {
    Isa::selectChannel(dmaChannel);
    Isa::setMode(dmaChannel, Isa::READ, false, false, Isa::SINGLE_TRANSFER);
    Isa::setAddress(dmaChannel, physicalDmaAddress + offset);
    Isa::setCount(dmaChannel, static_cast<uint16_t>(size - 1));
    Isa::deselectChannel(dmaChannel);
}

void SoundBlaster::play(uint32_t offset, uint32_t size) {
    lock.acquire();

    prepareDma(offset, size);
    setBufferSize(size);

    lock.release();
}

}