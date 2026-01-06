/*
 * Copyright (C) 2017-2026 Heinrich Heine University Düsseldorf,
 * Institute of Computer Science, Department Operating Systems
 * Main developers: Christian Gesse <christian.gesse@hhu.de>, Fabian Ruhland <ruhland@hhu.de>
 * Original development team: Burak Akguel, Christian Gesse, Fabian Ruhland, Filip Krakowski, Michael Schöttner
 * This project has been supported by several students.
 * A full list of integrated student theses can be found here: https://github.com/hhuOS/hhuOS/wiki/Student-theses
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

#include "kernel/service/MemoryService.h"
#include "lib/util/async/Thread.h"
#include "lib/util/time/Timestamp.h"
#include "kernel/service/InterruptService.h"
#include "device/bus/isa/Isa.h"
#include "kernel/service/ProcessService.h"
#include "kernel/service/FilesystemService.h"
#include "filesystem/memory/MemoryDriver.h"
#include "filesystem/Filesystem.h"
#include "kernel/log/Log.h"
#include "kernel/process/Thread.h"
#include "kernel/service/Service.h"
#include "lib/util/base/Constants.h"
#include "kernel/process/Scheduler.h"
#include "kernel/service/SoundService.h"

namespace Kernel {
enum InterruptVector : uint8_t;
struct InterruptFrame;
}  // namespace Kernel

namespace Device {
enum InterruptRequest : uint8_t;

SoundBlaster::SoundBlaster(uint16_t baseAddress, uint8_t irqNumber, uint8_t dmaChannel) :
        resetPort(baseAddress + 0x06), readDataPort(baseAddress + 0x0a),
        writeDataPort(baseAddress + 0x0c), readBufferStatusPort(baseAddress + 0x0e),
        mixerAddressPort(baseAddress + 0x04), mixerDataPort(baseAddress + 0x05),
        irqNumber(irqNumber), dmaChannel(dmaChannel) {
    // Get version
    while((readBufferStatusPort.readByte() & 0x80) != 0x80);
    uint8_t majorVersion = readDataPort.readByte();
    while((readBufferStatusPort.readByte() & 0x80) != 0x80);
    uint8_t minorVersion = readDataPort.readByte();

    dspVersion = (majorVersion << 8) | minorVersion;
    LOG_INFO("DSP version: [%u.%02u]", majorVersion, minorVersion);

    auto &memoryService = Kernel::Service::getService<Kernel::MemoryService>();
    dmaBuffer = static_cast<uint8_t*>(memoryService.allocateIsaMemory(DMA_BUFFER_SIZE / Util::PAGESIZE));
    physicalDmaAddress = static_cast<uint8_t*>(memoryService.getPhysicalAddress(dmaBuffer));
}

SoundBlaster::~SoundBlaster() {
    delete dmaBuffer;
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
    uint32_t timeoutTime = Util::Time::Timestamp::getSystemTime().toMilliseconds() + TIMEOUT;

    // Wait for read buffer to become ready
    do {
        uint8_t status = readBufferStatusPort.readByte();

        if ((status & 0x80) == 0x80) {
            timeout = false;
            break;
        }
    } while (Util::Time::Timestamp::getSystemTime().toMilliseconds() < timeoutTime);

    if (timeout) {
        return false;
    }

    timeoutTime = Util::Time::Timestamp::getSystemTime().toMilliseconds() + TIMEOUT;

    // Wait for ready code (represented by 0xaa) to appear in the read buffer
    do {
        uint8_t status = readDataPort.readByte();

        if(status == 0xaa) {
            return true;
        }
    } while (Util::Time::Timestamp::getSystemTime().toMilliseconds() < timeoutTime);

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

void SoundBlaster::initialize() {
    uint16_t baseAddress = getBasePort();
    if (baseAddress == 0) {
        return;
    }

    LOG_INFO("Found base port at address [0x%x]", baseAddress);
    auto readDataPort = IoPort(baseAddress + 0x0a);
    auto readBufferStatusPort = IoPort(baseAddress + 0x0e);
    auto writeDataPort = IoPort(baseAddress + 0x0c);

    // Issue version command
    while((readBufferStatusPort.readByte() & 0x80) == 0x80);
    writeDataPort.writeByte(0xe1);

    auto *soundBlaster = new SoundBlaster(baseAddress);
    soundBlaster->plugin();

    auto &soundService = Kernel::Service::getService<Kernel::SoundService>();
    soundService.setMasterOutputDevice(*soundBlaster);
}

bool SoundBlaster::reset() {
    // Issue reset command
    resetPort.writeByte(0x01);
    Util::Async::Thread::sleep(Util::Time::Timestamp::ofMilliseconds(5));
    resetPort.writeByte(0x00);

    bool timeout = true;
    uint32_t timeoutTime = Util::Time::Timestamp::getSystemTime().toMilliseconds() + TIMEOUT;

    do {
        uint8_t status = readBufferStatusPort.readByte();

        if ((status & 0x80) == 0x80) {
            timeout = false;
            break;
        }
    } while(Util::Time::Timestamp::getSystemTime().toMilliseconds() < timeoutTime);

    if (timeout) {
        return false;
    }

    timeoutTime = Util::Time::Timestamp::getSystemTime().toMilliseconds() + TIMEOUT;

    // Wait for ready code (represented by 0xaa) to appear in the read buffer
    do {
        uint8_t status = readDataPort.readByte();

        if(status == 0xaa) {
            return true;
        }
    } while(Util::Time::Timestamp::getSystemTime().toMilliseconds() < timeoutTime);

    return false;
}

void SoundBlaster::plugin() {
    // Older DSPs (version < 4) don't support manual IRQ- and DMA-configuration.
    // They must be configured via jumpers and there is no real way to get the IRQ- and DMA-numbers in software.
    // We just assume the DSP to use IRQ 10 and DMA channel 1, if not specified else in the constructor.
    auto &interruptService = Kernel::Service::getService<Kernel::InterruptService>();
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

void SoundBlaster::turnSpeakerOn() {
    writeToDSP(TURN_SPEAKER_ON);
}

void SoundBlaster::turnSpeakerOff() {
    writeToDSP(TURN_SPEAKER_OFF);
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

void SoundBlaster::trigger([[maybe_unused]] const Kernel::InterruptFrame &frame, [[maybe_unused]] Kernel::InterruptVector slot) {
    receivedInterrupt = true;
    ackInterrupt();
}

void SoundBlaster::prepareDma(uint32_t offset, uint32_t size) const {
    Isa::selectChannel(dmaChannel);
    Isa::setMode(dmaChannel, Isa::READ, false, false, Isa::SINGLE_TRANSFER);
    Isa::setAddress(dmaChannel, physicalDmaAddress + offset);
    Isa::setCount(dmaChannel, static_cast<uint16_t>(size - 1));
    Isa::deselectChannel(dmaChannel);
}

void SoundBlaster::enableLowPassFilter() {
    mixerAddressPort.writeByte(OUTPUT_CONTROL);
    mixerDataPort.writeByte(mixerDataPort.readByte() & ~LOW_PASS_FILTER);
    lowPassFilterEnabled = true;
}

void SoundBlaster::disableLowPassFilter() {
    mixerAddressPort.writeByte(OUTPUT_CONTROL);
    mixerDataPort.writeByte(mixerDataPort.readByte() | LOW_PASS_FILTER);
    lowPassFilterEnabled = false;
}

void SoundBlaster::enableStereo() {
    mixerAddressPort.writeByte(OUTPUT_CONTROL);
    mixerDataPort.writeByte(mixerDataPort.readByte() | STEREO);

    // To enable stereo mode, the DSP needs to output a single silent byte
    dmaBuffer[0] = 0x80;
    dmaBuffer[1] = 0x80;

    prepareDma(0, 2);
    writeToDSP(EIGHT_BIT_SINGLE_CYCLE_DMA_OUTPUT);
    writeBufferSize(1);

    waitForInterrupt();
    stereoEnabled = true;
}

void SoundBlaster::disableStereo() {
    mixerAddressPort.writeByte(OUTPUT_CONTROL);
    mixerDataPort.writeByte(mixerDataPort.readByte() & ~STEREO);
    stereoEnabled = false;
}

void SoundBlaster::writeTimeConstant() {
    writeToDSP(0x40);
    writeToDSP(static_cast<uint8_t>((timeConstant & 0xff00) >> 8));
}

void SoundBlaster::writeBufferSize(uint32_t size) {
    writeToDSP(static_cast<uint8_t>((size - 1) & 0x00ff));
    writeToDSP(static_cast<uint8_t>(((size - 1) & 0xff00) >> 8));
}

void SoundBlaster::startDmaTransfer(uint32_t offset, uint32_t size) {
    prepareDma(offset, size);

    if (numChannels == 1 && samplesPerSecond <= 23000) {
        writeToDSP(EIGHT_BIT_SINGLE_CYCLE_DMA_OUTPUT);
        writeBufferSize(size);
    } else {
        writeToDSP(SET_BLOCK_TRANSFER_SIZE);
        writeBufferSize(size);
        writeToDSP(EIGHT_BIT_SINGLE_CYCLE_HIGH_SPEED_DMA_OUTPUT);
    }
}

bool SoundBlaster::setPlaybackParameters(uint32_t sampleRate, uint8_t channels, uint8_t bitsPerSample) {
    if (dspVersion < 0x0201) {
        if (channels > 1 || bitsPerSample != 8 || sampleRate < 4000 || sampleRate > 23000) {
            return false;
        }
    } else if (dspVersion < 0x0300) {
        if (channels > 1 || bitsPerSample != 8 || sampleRate < 4000 || sampleRate > 44100) {
            return false;
        }
    } else if (channels == 1) {
        if (bitsPerSample != 8 || sampleRate < 4000 || sampleRate > 44100) {
            return false;
        }
    } else if (channels == 2) {
        if (bitsPerSample != 8 || (sampleRate != 11025 && sampleRate != 22050)) {
            return false;
        }
    }

    samplesPerSecond = sampleRate;
    timeConstant = static_cast<uint16_t>(65536 - (256000000 / (sampleRate * channels)));
    numChannels = channels;
    SoundBlaster::bitsPerSample = bitsPerSample;

    if (dspVersion >= 0x0300) {
        if (numChannels == 1) {
            disableStereo();
            enableLowPassFilter();
        } else if (numChannels == 2) {
            enableStereo();
            disableLowPassFilter();
        }
    }

    writeTimeConstant();
    return true;
}

uint32_t SoundBlaster::getSampleRate() const {
    return samplesPerSecond;
}

uint8_t SoundBlaster::getChannels() const {
    return numChannels;
}

uint8_t SoundBlaster::getBitsPerSample() const {
    return bitsPerSample;
}

void SoundBlaster::sourceDrained() {
    if (isPlaying) {
        waitForInterrupt();
        turnSpeakerOff();
        isPlaying = false;
        dmaOffset = 0;
    }
}

void SoundBlaster::play(const uint8_t *samples, uint32_t size) {
    if (size == 0) {
        return;
    }

    // Copy samples to the DMA buffer
    const auto toCopy = DMA_BUFFER_SIZE - dmaOffset < size ? DMA_BUFFER_SIZE - dmaOffset : size;
    Util::Address(dmaBuffer).add(dmaOffset).copyRange(Util::Address(samples), toCopy);

    // Wait for current DMA transfer to finish (if one is in progress)
    if (isPlaying) {
        waitForInterrupt();
    } else {
        isPlaying = true;
        turnSpeakerOn();
    }

    // Start the DMA transfer
    startDmaTransfer(dmaOffset, toCopy);

    // Increase offset for next write (wrap around if necessary)
    if (dmaOffset + toCopy >= DMA_BUFFER_SIZE) {
        dmaOffset = 0;
    } else {
        dmaOffset += toCopy;
    }

    if (toCopy < size) {
        play(samples + toCopy, size - toCopy);
    }
}

}
