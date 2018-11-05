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
#include <kernel/threads/Scheduler.h>
#include "SoundBlaster16.h"

SoundBlaster16::SoundBlaster16(uint16_t baseAddress, uint8_t irqNumber, uint8_t dmaChannel8, uint8_t dmaChannel16)
        : SoundBlaster(baseAddress),
          mixerAddressPort(static_cast<uint16_t>(baseAddress + 0x04)),
          mixerDataPort(static_cast<uint16_t>(baseAddress + 0x05)),
          irqNumber(irqNumber), dmaChannel8(dmaChannel8), dmaChannel16(dmaChannel16) {
    plugin();
}

void SoundBlaster16::setSamplingRate(uint16_t samplingRate) {
    writeToDSP(0x41);

    writeToDSP(static_cast<uint8_t>((samplingRate & 0xff00) >> 8));
    writeToDSP(static_cast<uint8_t>(samplingRate & 0x00ff));
}

void SoundBlaster16::setBufferSize(uint32_t bufferSize, bool stereo, bool bits16) {
    uint8_t mode;

    if(!stereo) {
        if(!bits16) {
            mode = 0x00;
        } else {
            mode = 0x10;
        }
    } else {
        if(!bits16) {
            mode = 0x20;
        } else {
            mode = 0x30;
        }
    }

    if(bits16) {
        bufferSize /= 2;
    }

    writeToDSP(mode);

    writeToDSP(static_cast<uint8_t>((bufferSize - 1) & 0x00ff));
    writeToDSP(static_cast<uint8_t>(((bufferSize - 1) & 0xff00) >> 8));
}

void SoundBlaster16::prepareDma(uint16_t addressOffset, uint32_t bufferSize, bool bits16, bool autoInitialize) {
    auto channel = static_cast<uint8_t>(bits16 ? 5 : 1);

    if(bits16) {
        bufferSize /= 2;
    }

    Isa::selectChannel(channel);
    Isa::setMode(channel, Isa::TRANSFER_MODE_READ, autoInitialize, false, Isa::DMA_MODE_SINGLE_TRANSFER);
    Isa::setAddress(channel, (uint32_t) SystemManagement::getInstance()->getPhysicalAddress(dmaMemory) + addressOffset);
    Isa::setCount(channel, static_cast<uint16_t>(bufferSize - 1));
    Isa::deselectChannel(channel);
}

void SoundBlaster16::stopAutoInitialize(bool bits16) {
    auto command = static_cast<uint8_t>(bits16 ? 0xd9 : 0xda);

    writeToDSP(command);
}

void SoundBlaster16::ackInterrupt() {
    readBufferStatusPort.inb();
}

void SoundBlaster16::playPcmData(const Pcm &pcm) {
    if(pcm.getAudioFormat() != Pcm::PCM || pcm.getNumChannels() > 2 || (pcm.getBitsPerSample() != 8 &&
       pcm.getBitsPerSample() != 16) || pcm.getSamplesPerSecond() < 5000 || pcm.getSamplesPerSecond() > 44100) {
        return;
    }

    uint8_t commandByte;

    if(pcm.getBitsPerSample() == 8) {
        commandByte = 0xc6;
    } else {
        commandByte = 0xb6;
    }

    uint32_t dataSize = pcm.getFrameSize() * pcm.getSampleCount();

    soundLock.acquire();

    stopPlaying = false;

    setSamplingRate(static_cast<uint16_t>(pcm.getSamplesPerSecond()));

    bool firstBlock = true;
    uint32_t count = ((dataSize) >= 0x10000) ? 0x10000 : dataSize;
    uint16_t addressOffset = 0;
    memcpy(dmaMemory, pcm.getPcmData(), count);

    prepareDma(addressOffset, dataSize < 0x10000 ? dataSize : 0x10000, pcm.getBitsPerSample() == 16);

    writeToDSP(commandByte);
    setBufferSize(static_cast<uint16_t>(dataSize < 0x10000 ? dataSize : 0x8000),
            pcm.getNumChannels() == 2, pcm.getBitsPerSample() == 16);

    bool stop = false;

    for(uint32_t i = 0x10000; i < dataSize && !stop; i += 0x8000) {
        if(i + 0x8000 >= dataSize || stopPlaying) {
            stopAutoInitialize(pcm.getBitsPerSample() == 16);
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

    soundLock.release();
}

void SoundBlaster16::stopPlayback() {
    stopPlaying = true;
}

void SoundBlaster16::plugin() {
    // Manually configure the DSP to use the specified DMA channels.
    if(dmaChannel8 > 3 || dmaChannel8 == 2) {
        dmaChannel8 = 1;
    }

    if(dmaChannel16 > 7 || dmaChannel16 == 4) {
        dmaChannel16 = 5;
    }

    mixerAddressPort.outb(0x81);
    mixerDataPort.outb(static_cast<uint8_t>((1u << dmaChannel8) | (1u << dmaChannel16)));

    // Manually configure the DSP to use the specified IRQ number.
    mixerAddressPort.outb(0x80);

    switch(irqNumber) {
        case 2 :
            mixerAddressPort.outb(0x01);
            break;
        case 5 :
            mixerAddressPort.outb(0x02);
            break;
        case 7 :
            mixerAddressPort.outb(0x04);
            break;
        case 10 :
            mixerAddressPort.outb(0x08);
            break;
        default :
            irqNumber = 10;

            mixerAddressPort.outb(0x08);
            break;

    }

    IntDispatcher::getInstance().assign(static_cast<uint8_t>(32 + irqNumber), *this);
    Pic::getInstance()->allow(static_cast<Pic::Interrupt>(irqNumber));
}

void SoundBlaster16::trigger(InterruptFrame &frame) {
    receivedInterrupt = true;
}
