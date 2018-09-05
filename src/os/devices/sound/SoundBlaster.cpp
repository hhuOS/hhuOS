#include <kernel/Kernel.h>
#include <kernel/interrupts/IntDispatcher.h>
#include "SoundBlaster.h"

Logger &SoundBlaster::log = Logger::get("SOUNDBLASTER");

bool SoundBlaster::checkPort(uint16_t baseAddress) {
    SoundBlaster tmp(baseAddress);

    return tmp.reset();
}

uint16_t SoundBlaster::getBasePort() {
    for(uint16_t i = 0x210; i < 0x290; i += 0x10) {
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
    uint32_t timeoutTime = timeService->getSystemTime() + RESET_TIMEOUT;

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

    timeoutTime = timeService->getSystemTime() + RESET_TIMEOUT;

    // Wait ready code (represented by 0xaa) to appear in the read buffer
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

void SoundBlaster::playPcmData(const Pcm &pcm) {
    // TODO: Implement playback of pcm data via ISA DMA
}

void SoundBlaster::plugin() {
    // Older DSPs don't support IRQ-configuration.
    // They must be configured via jumpers and there is no real way to get the IRQ-number.
    // We just assume the DSP to use IRQ10.

    if(majorVersion > 4) {
        // Manually configure the DSP to use IRQ10 on SoundBlaster 16 or newer cards.
        mixerAddressPort.outb(0x80);
        mixerDataPort.outb(0x08);
    }

    IntDispatcher::getInstance().assign(42, *this);
    Pic::getInstance()->allow(Pic::Interrupt::FREE2);
}

void SoundBlaster::trigger() {

}