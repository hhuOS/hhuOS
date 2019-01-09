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

#ifndef HHUOS_SOUNDBLASTER_H
#define HHUOS_SOUNDBLASTER_H

#include <cstdint>
#include <devices/cpu/IOport.h>
#include <kernel/services/TimeService.h>
#include <devices/sound/PcmAudioDevice.h>
#include <lib/lock/Spinlock.h>

/**
 * Base-class for any ISA-based SoundBlaster-cards.
 */
class SoundBlaster : public PcmAudioDevice {

protected:

    Logger *log;

    uint16_t baseAddress;

    uint8_t majorVersion;
    uint8_t minorVersion;

    Spinlock soundLock;

    void *dmaMemory = nullptr;

    IOport resetPort;
    IOport readDataPort;
    IOport writeDataPort;
    IOport readBufferStatusPort;

    TimeService *timeService = nullptr;

private:

    static const constexpr uint16_t FIRST_BASE_ADDRESS = 0x220;
    static const constexpr uint16_t LAST_BASE_ADDRESS = 0x280;
    static const constexpr uint32_t TIMEOUT = 10;

    static const constexpr char *VENDOR_NAME = "Creative Technology";

    static const constexpr char *SOUND_BLASTER_1 = "Sound Blaster 1.0";
    static const constexpr char *SOUND_BLASTER_2 = "Sound Blaster 2.0";
    static const constexpr char *SOUND_BLASTER_PRO = "Sound Blaster Pro";
    static const constexpr char *SOUND_BLASTER_16 = "Sound Blaster 16";
    static const constexpr char *SOUND_BLASTER_AWE32 = "Sound Blaster AWE32";

private:

    /**
     * Check, if SoundBlaster IO-ports are available at the given address.
     *
     * Usually, the ports start at at address 0x210, or 0x220, 0x230, ..., 0x290.
     *
     * @param baseAddress The address to be checked
     * @return true, if SoundBlaster IO-ports are found
     */
    static bool checkPort(uint16_t baseAddress);

    /**
     * Use checkPort() to search for the start address of SoundBlaster IO-ports.
     *
     * @return The found start address, or 0 if none is found.
     */
    static uint16_t getBasePort();

protected:

    /**
     * Constructor.
     *
     * @param baseAddress The IO-port's start address.
     */
    explicit SoundBlaster(uint16_t baseAddress);

    /**
     * Reset the device.
     *
     * @return true, after a successful reset
     */
    bool reset();

    /**
     * Read a byte from the digital signal processor (DSP).
     *
     * The DSP is used to communicate with and send commands to the sound card.
     *
     * @return The retrieved byte.
     */
    uint8_t readFromDSP();

    /**
     * Write a byte to the digital signal processor (DSP).
     *
     * The DSP is used to communicate with and send commands to the sound card.
     *
     * @param value The byte to be sent.
     */
    void writeToDSP(uint8_t value);

    /**
     * Directly read a byte from the Analog-to-Digital Converter (ADC).
     *
     * The ADC takes microphone input and converts it to digital samples. By reading directly from the ADC, one can
     * get these samples one-by-one. However, this takes up much processing time and needs precise timing.
     * It is much better to let the DMA-controller handle the communication with the ADC and retrieve larger chunks
     * of samples from it at once.
     * This functionality will be implemented in the SoundBlaster-subclasses.
     *
     * @return The retrieved byte
     */
    uint8_t readFromADC();

    /**
     * Directly write a byte to the Digital-to-Analog Converter (DAC).
     *
     * The DAC takes digital samples and converts them to analog sound, that can be output by a speaker.
     * By writing directly to the DAC, one can output these samples one-by-one. However, this takes up much
     * processing time and needs precise timing. It is much better to let the DMA-controller handle the communication
     * with the DAC and send larger chunks of samples to it at once.
     * This functionality is implemented in the SoundBlaster-subclasses.
     *
     * CAUTION: As it seems, writing directly to the DAC is currently not supported by QEMU.
     *
     * @param balue The byte to be sent.
     */
    void writeToDAC(uint8_t value);

    /**
     * Turn the speaker on (Unnecessary on SoundBlaster16).
     */
    void turnSpeakerOn();

    /**
     * Turn the speaker off (Unnecessary on SoundBlaster16).
     */
    void turnSpeakerOff();

public:

    /**
     * Check, if a SoundBlaster card is installed in the system.
     */
    static bool isAvailable();

    /**
     * Search for a SoundBlaster card and create a new instance of the respective driver.
     *
     * The instance is created on the heap and must be deleted manually, once it is not needed anymore.
     *
     * @return An instance of a SoundBlaster driver or nullptr, if no SoundBlaster card is available
     */
    static SoundBlaster *initialize();

    /**
     * Destructor.
     */
    ~SoundBlaster() = default;

    /**
     * Overriding function from PcmAudioDevice.
     */
    String getVendorName() override;

    /**
     * Overriding function from PcmAudioDevice.
     */
    String getDeviceName() override;

    /**
     * Overriding function from PcmAudioDevice.
     */
    void playPcmData(const Pcm &pcm) override = 0;

    /**
     * Overriding function from PcmAudioDevice.
     */
    void stopPlayback() override = 0;

};

#endif
