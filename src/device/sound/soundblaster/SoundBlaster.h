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

#ifndef HHUOS_SOUNDBLASTER_H
#define HHUOS_SOUNDBLASTER_H

#include "kernel/interrupt/InterruptHandler.h"
#include "device/cpu/IoPort.h"
#include "lib/util/async/Spinlock.h"
#include "kernel/log/Logger.h"
#include "device/isa/Isa.h"

namespace Device {

class SoundBlasterRunnable;

/**
 * Driver for the original SoundBlaster cards.
 * The IRQ-number and DMA-channel, which the card uses, have to be set by using jumpers on the card.
 * If not specified else in the constructor, this driver assumes that IRQ 10 and DMA-channel 1 are used.
 *
 * Supported audio formats:
 *      -Mono PCM, 4000-23000 Hz, 8-bit samples
 *      -Mono PCM, 23000-44100 Hz, 8-bit samples (only with DSP versions >= 2.01)
 *      -Stereo PCM, 11025 Hz or 22050 Hz, 8-bit samples (only with DSP version >= 3.00)
 */
class SoundBlaster : public Kernel::InterruptHandler {

public:
    /**
     * Copy Constructor.
     */
    SoundBlaster(const SoundBlaster &other) = delete;

    /**
     * Assignment operator.
     */
    SoundBlaster &operator=(const SoundBlaster &other) = delete;

    /**
     * Destructor.
     */
    ~SoundBlaster() override;

    /**
     * Check, if a SoundBlaster card is installed in the system.
     */
    static bool isAvailable();

    /**
     * Search for a SoundBlaster card and create a new instance of the respective driver.
     */
    static bool initialize();

    /**
     * Turn the speaker on.
     */
    void turnSpeakerOn();

    /**
     * Turn the speaker off.
     */
    void turnSpeakerOff();

    /**
     * Block until an interrupt occurs.
     */
    void waitForInterrupt();

    /**
     * Acknowledge an interrupt.
     */
    void ackInterrupt();

    /**
     * Overriding function from InterruptHandler.
     */
    void trigger(const Kernel::InterruptFrame &frame) override;

    /**
     * Set the audio playback parameters.
     *
     * @param sampleRate The sample rate
     * @param channels The amount of channels
     * @param bitsPerSample The amount of bits per sample
     */
    bool setAudioParameters(uint16_t sampleRate, uint8_t channels, uint8_t bitsPerSample);

    [[nodiscard]] uint32_t getDmaBufferSize() const;

    [[nodiscard]] uint8_t* getDmaBuffer() const;

    void play(uint32_t offset, uint32_t size);

    void plugin() override;

private:

    enum DspCommand {
        GET_VERSION = 0xe1,
        TURN_SPEAKER_ON = 0xd1,
        TURN_SPEAKER_OFF = 0xd3,
        TRANSFER_TIME_CONSTANT = 0x40,
        SET_BLOCK_TRANSFER_SIZE = 0x48,
        EIGHT_BIT_SINGLE_CYCLE_DMA_OUTPUT = 0x14,
        EIGHT_BIT_SINGLE_CYCLE_HIGH_SPEED_DMA_OUTPUT = 0x91
    };

    enum MixerRegister {
        RESET = 0x00,
        OUTPUT_CONTROL = 0x0e
    };

    enum MixerOutputOptions {
        STEREO = 0x02,
        LOW_PASS_FILTER = 0x20
    };

    /**
     * Constructor.
     */
    explicit SoundBlaster(uint16_t baseAddress, uint8_t irqNumber = 10, uint8_t dmaChannel = 1);

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
     * Prepare the DMA-controller for a data transfer to the sound card.
     *
     * @param offset Offset to add to the base address, that is used for DMA-transfer (SoundBlaster::dmaMemory)
     * @param size The size of the buffer to be transferred
     */
    void prepareDma(uint32_t offset, uint32_t size) const;

    /**
     * Enable the mixer's low-pass filter (Recommended for mono-samples, which are played at a rate of <= 23000 Hz).
     */
    void enableLowPassFilter();

    /**
     * Disable the mixer's low pass-filter (Recommended for all stereo-samples,
     * or mono-samples,which are played at a rate of > 23000 Hz).
     */
    void disableLowPassFilter();

    /**
     * Enable stereo-mode.
     */
    void enableStereo();

    /**
     * Disable stereo-mode.
     */
    void disableStereo();

    /**
     * Set the time constant, the DSP should use for the next transfer.
     *
     * @param timeConstant The time constant
     */
    void writeTimeConstant();

    /**
     * Set the size of the buffer, that the DSP awaits to play.
     *
     * @param size The size
     */
    void writeBufferSize(uint32_t size);

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

    IoPort resetPort;
    IoPort readDataPort;
    IoPort writeDataPort;
    IoPort readBufferStatusPort;

    IoPort mixerAddressPort;
    IoPort mixerDataPort;

    uint8_t irqNumber;
    uint8_t dmaChannel;

    uint16_t dspVersion;

    uint16_t samplesPerSecond = 0;
    uint16_t timeConstant = 0;
    uint8_t numChannels = 1;

    bool stereoEnabled = false;
    bool lowPassFilterEnabled = false;

    uint32_t dmaBufferSize = 0;
    uint8_t *dmaBuffer = nullptr;
    uint8_t *physicalDmaAddress = nullptr;

    bool receivedInterrupt = false;

    SoundBlasterRunnable *runnable;

    static Kernel::Logger log;

    static const constexpr uint16_t FIRST_BASE_ADDRESS = 0x220;
    static const constexpr uint16_t LAST_BASE_ADDRESS = 0x280;
    static const constexpr uint32_t TIMEOUT = 10;

    static const constexpr double AUDIO_BUFFER_SIZE = 0.2;
};

}

#endif
