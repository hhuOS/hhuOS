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

#include <devices/isa/Isa.h>
#include <kernel/memory/SystemManagement.h>
#include <kernel/interrupts/IntDispatcher.h>
#include <kernel/interrupts/Pic.h>
#include "SoundBlasterPro.h"

SoundBlasterPro::SoundBlasterPro(uint16_t baseAddress, uint8_t irqNumber, uint8_t dmaChannel) : SoundBlaster(baseAddress),
        mixerAddressPort(static_cast<uint16_t>(baseAddress + 0x04)),
        mixerDataPort(static_cast<uint16_t>(baseAddress + 0x05)),
        irqNumber(irqNumber), dmaChannel(dmaChannel) {
    plugin();
}
void SoundBlasterPro::setSamplingRate(uint16_t samplingRate, bool stereo) {
    auto timeConstant = static_cast<uint16_t>(65536 - (256000000 / (samplingRate * (stereo ? 2 : 1))));

    writeToDSP(0x40);
    writeToDSP(static_cast<uint8_t>((timeConstant & 0xff00) >> 8));
}

void SoundBlasterPro::setBufferSize(uint32_t bufferSize) {
    writeToDSP(0x48);

    writeToDSP(static_cast<uint8_t>((bufferSize - 1) & 0x00ff));
    writeToDSP(static_cast<uint8_t>(((bufferSize - 1) & 0xff00) >> 8));
}

void SoundBlasterPro::prepareDma(uint16_t addressOffset, uint32_t bufferSize, bool autoInitialize) {
    Isa::selectChannel(dmaChannel);
    Isa::setMode(dmaChannel, Isa::TRANSFER_MODE_READ, autoInitialize, false, Isa::DMA_MODE_SINGLE_TRANSFER);
    Isa::setAddress(dmaChannel, (uint32_t) SystemManagement::getInstance()->getPhysicalAddress(dmaMemory) + addressOffset);
    Isa::setCount(dmaChannel, static_cast<uint16_t>(bufferSize - 1));
    Isa::deselectChannel(dmaChannel);
}

void SoundBlasterPro::stopAutoInitialize() {
    writeToDSP(0xda);
}

void SoundBlasterPro::enableLowPassFilter() {
    mixerAddressPort.outb(0x0e);

    uint8_t oldValue = mixerDataPort.inb();

    mixerDataPort.outb(static_cast<uint8_t>(oldValue & 0xdf));
}

void SoundBlasterPro::disableLowPassFilter() {
    mixerAddressPort.outb(0x0e);

    uint8_t oldValue = mixerDataPort.inb();

    mixerDataPort.outb(static_cast<uint8_t>(oldValue | 0x20));
}

void SoundBlasterPro::enableStereo() {
    // First, we set the mixer to stereo mode
    mixerAddressPort.outb(0x0e);

    char oldValue = mixerDataPort.inb();

    mixerDataPort.outb(static_cast<uint8_t>(oldValue | 0x02));

    // Now it is necessary to let the DSP output a single silent byte
    reinterpret_cast<char*>(dmaMemory)[0] = 0;
    prepareDma(0, 2, false);

    writeToDSP(0x14);
    writeToDSP(0);
    writeToDSP(0);

    // Now wait for an interrupt. The DSP should then be able to output stereo sound
    receivedInterrupt = false;
    while(!receivedInterrupt);
    ackInterrupt();
}

void SoundBlasterPro::disableStereo() {
    mixerAddressPort.outb(0x0e);

    uint8_t oldValue = mixerDataPort.inb();

    mixerDataPort.outb(static_cast<uint8_t>(oldValue & 0xfd));
}

void SoundBlasterPro::ackInterrupt() {
    readBufferStatusPort.inb();
}

void SoundBlasterPro::playPcmData(const Pcm &pcm) {
    if(pcm.getAudioFormat() != Pcm::PCM || pcm.getNumChannels() > 2 || pcm.getBitsPerSample() != 8 ||
       pcm.getSamplesPerSecond() < 4000 || pcm.getSamplesPerSecond() > 44100) {
        return;
    }

    if(pcm.getNumChannels() == 2 && pcm.getSamplesPerSecond() != 11025 && pcm.getSamplesPerSecond() != 22050) {
        return;
    }

    uint8_t commandByte;

    soundLock.acquire();

    if(pcm.getSamplesPerSecond() <= 23000 && pcm.getNumChannels() == 1) {
        commandByte = 0x1c;
        enableLowPassFilter();
    } else {
        commandByte = 0x1c;
        disableLowPassFilter();
    }

    uint32_t dataSize = pcm.getFrameSize() * pcm.getSampleCount();

    stopPlaying = false;

    turnSpeakerOn();

    if(pcm.getNumChannels() == 2) {
        enableStereo();
    }

    setSamplingRate(static_cast<uint16_t>(pcm.getSamplesPerSecond()), pcm.getNumChannels() == 2);

    bool firstBlock = true;
    uint32_t count = ((dataSize) >= 0x10000) ? 0x10000 : dataSize;
    uint16_t addressOffset = 0;
    memcpy(dmaMemory, pcm.getPcmData(), count);

    prepareDma(addressOffset, dataSize < 0x10000 ? dataSize : 0x10000);
    setBufferSize(dataSize < 0x10000 ? dataSize : 0x8000);

    writeToDSP(commandByte);

    bool stop = false;

    for(uint32_t i = 0x10000; i < dataSize && !stop; i += 0x8000) {
        if(i + 0x8000 >= dataSize || stopPlaying) {
            stopAutoInitialize();
            stop = true;
        }

        receivedInterrupt = false;
        while(!receivedInterrupt);

        count = ((dataSize - i) >= 0x8000) ? 0x8000 : dataSize - i;
        addressOffset = static_cast<uint16_t>(firstBlock ? 0 : 0x8000);
        memcpy(reinterpret_cast<char*>(dmaMemory) + addressOffset, pcm.getPcmData() + i, count);
        memset(reinterpret_cast<char*>(dmaMemory) + addressOffset + count, 0, static_cast<uint32_t>(0x8000 - count));

        firstBlock = !firstBlock;

        ackInterrupt();
    }

    stopAutoInitialize();

    turnSpeakerOff();

    if(pcm.getNumChannels() == 2) {
        disableStereo();
    }

    soundLock.release();
}

void SoundBlasterPro::stopPlayback() {
    stopPlaying = true;
}

void SoundBlasterPro::plugin() {
    // Older DSPs (version < 4) don't support manual IRQ- and DMA-configuration.
    // They must be configured via jumpers and there is no real way to get the IRQ- and DMA-numbers in software.
    // We just assume the DSP to use IRQ 10 and DMA channel 1, if not specified else in the constructor.

    IntDispatcher::getInstance().assign(static_cast<uint8_t>(32 + irqNumber), *this);
    Pic::getInstance()->allow(static_cast<Pic::Interrupt>(irqNumber));
}

void SoundBlasterPro::trigger(InterruptFrame &frame) {
    receivedInterrupt = true;
}
