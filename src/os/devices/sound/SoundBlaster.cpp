#include <kernel/Kernel.h>
#include <kernel/interrupts/IntDispatcher.h>
#include <kernel/memory/SystemManagement.h>
#include "SoundBlaster.h"

Logger &SoundBlaster::log = Logger::get("SOUNDBLASTER");

bool SoundBlaster::checkPort(uint16_t baseAddress) {
    SoundBlaster tmp(baseAddress);

    return tmp.reset();
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
        resetPort(static_cast<uint16_t>(baseAddress + 0x06)),
        readDataPort(static_cast<uint16_t>(baseAddress + 0x0a)),
        writeDataPort(static_cast<uint16_t>(baseAddress + 0x0c)),
        readBufferStatusPort(static_cast<uint16_t>(baseAddress + 0x0e)),
        mixerAddressPort(static_cast<uint16_t>(baseAddress + 0x04)),
        mixerDataPort(static_cast<uint16_t>(baseAddress + 0x05)),
        timeService(Kernel::getService<TimeService>()) {

}

SoundBlaster::SoundBlaster() : SoundBlaster(getBasePort())  {
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

    featureSet = FeatureSet(majorVersion, minorVersion);

    dmaMemory = Isa::allocDmaBuffer();
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

void SoundBlaster::setup() {
    plugin();
    turnSpeakerOn();
}

void SoundBlaster::turnSpeakerOn() {
    writeToDSP(0xd1);
}

void SoundBlaster::turnSpeakerOff() {
    writeToDSP(0xd3);
}

uint8_t SoundBlaster::ackInterrupt() {
    return readBufferStatusPort.inb();
}

void SoundBlaster::prepareDma(Isa::TransferMode transferMode, uint16_t dataSize) {
    Isa::selectChannel(1);
    Isa::setMode(1, transferMode, false, false, Isa::DMA_MODE_SINGLE_TRANSFER);
    Isa::setAddress(1, (uint32_t) SystemManagement::getInstance()->getPhysicalAddress(dmaMemory));
    Isa::setCount(1, static_cast<uint16_t>(dataSize));
    Isa::deselectChannel(1);
}

void SoundBlaster::dspSetSampleRate(uint16_t numChannels, uint32_t samplesPerSecond) {
    auto timeConstant = static_cast<uint16_t>( - (256000000 / (numChannels * samplesPerSecond)));

    writeToDSP(0x40);
    writeToDSP(static_cast<uint8_t>((timeConstant & 0xff00) >> 8));
}

void SoundBlaster::dspSetBufferSize(uint16_t dataSize) {
    writeToDSP(0x14);
    writeToDSP(static_cast<uint8_t>((dataSize - 1) & 0x00ff));
    writeToDSP(static_cast<uint8_t>(((dataSize - 1) & 0xff00) >> 8));
}

void SoundBlaster::play8BitPcm(const Pcm &pcm) {
    if(pcm.getNumChannels() == 1 && !featureSet.monoPcm8Bit) {
        // Should never occur
        return;
    }

    if(pcm.getNumChannels() == 2 && !featureSet.stereoPcm8Bit) {
        return;
    }

    if(pcm.getNumChannels() > 2) {
        return;
    }

    if(pcm.getSamplesPerSecond() > 23000) {
        // TODO: Implement High Speed transfer mode

        return;
    }

    if(!featureSet.dmaSingleCycle) {
        // Should never occur
        return;
    }

    uint32_t dataSize = pcm.getFrameSize() * pcm.getSampleCount();

    turnSpeakerOn();

    stopPlaying = false;

    for(uint32_t i = 0; i < dataSize && !stopPlaying; i += 0xffff) {
        cycleLock.acquire();

        auto count = static_cast<uint16_t>(((dataSize - i) >= 0xffff) ? 0xffff : dataSize - i);
        memcpy(dmaMemory, &pcm.getPcmData()[i], count);

        if(i > 0) {
            ackInterrupt();
        }

        prepareDma(Isa::TRANSFER_MODE_READ, count);

        dspSetSampleRate(pcm.getNumChannels(), pcm.getSamplesPerSecond());
        dspSetBufferSize(count);

        receivedInterrupt = false;
        while(!receivedInterrupt);

        cycleLock.release();
    }

    ackInterrupt();

    turnSpeakerOff();
}

void SoundBlaster::play16BitPcm(const Pcm &pcm) {
    if(pcm.getNumChannels() == 1 && !featureSet.monoPcm16Bit) {
        return;
    }

    if(pcm.getNumChannels() == 2 && !featureSet.stereoPcm16Bit) {
        return;
    }

    if(pcm.getNumChannels() > 2) {
        return;
    }

    // TODO: Implement playback of 16-Bit PCM
}

void SoundBlaster::playPcmData(const Pcm &pcm) {
    if(pcm.getAudioFormat() != Pcm::PCM) {
        // TODO: Implement support for ADPCM

        return;
    }

    if(pcm.getSamplesPerSecond() > 44100) {
        return;
    }

    if(pcm.getBitsPerSample() == 8) {
        play8BitPcm(pcm);
    } else if(pcm.getBitsPerSample() == 16) {
        play16BitPcm(pcm);
    }
}

void SoundBlaster::stopPlayback() {
    stopPlaying = true;
}

void SoundBlaster::plugin() {
    // Older DSPs don't support IRQ- and DMA-configuration.
    // They must be configured via jumpers and there is no real way to get the IRQ-number.
    // We just assume the DSP to use IRQ10 and DMA channel 1.

    if(majorVersion >= 4) {
        // Manually configure the DSP to use IRQ10 on SoundBlaster 16 or newer cards.
        mixerAddressPort.outb(0x80);
        mixerDataPort.outb(0x08);


        // Manually configure the DSP to use DMA channel 1 on SoundBlaster 16 or newer cards.
        mixerAddressPort.outb(0x81);
        mixerDataPort.outb(0x02);
    }

    IntDispatcher::getInstance().assign(42, *this);
    Pic::getInstance()->allow(Pic::Interrupt::FREE2);
}

bool SoundBlaster::checkForData() {
    if(majorVersion >= 4) {
        mixerAddressPort.outb(0x82);
        if((mixerDataPort.inb() & 0x01) == 0x01) {
            return true;
        }
    }

    return false;
}

void SoundBlaster::trigger() {
    receivedInterrupt = true;
}
