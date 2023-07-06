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
/**
 * Driver for the original SoundBlaster (and compatible) cards (DSP version 1.xx).
 * The IRQ-number and DMA-channel, which the card uses, have to be set by using jumpers on the card.
 * If not specified else in the constructor, this driver assumes that IRQ 10 and DMA-channel 1 are used.
 *
 * Supported audio formats:
 *      -Mono PCM, 4000-23000 Hz, 8-bit samples
 */
namespace Device {

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
     * Turn the speaker on (Unnecessary on SoundBlaster16).
     */
    void turnSpeakerOn();

    /**
     * Turn the speaker off (Unnecessary on SoundBlaster16).
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
     * @param bits The amount of bits per sample
     */
    bool setAudioParameters(uint16_t sampleRate, uint8_t channels, uint8_t bits);

    [[nodiscard]] uint32_t getDmaBufferSize() const;

    [[nodiscard]] uint8_t* getDmaBuffer() const;

    void play(uint32_t offset, uint32_t size);

    void plugin() override;

private:
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
     * @param value The byte to be sent.
     */
    void writeToDAC(uint8_t value);

    /**
     * Set the sample rate, that the DSP shall use to play the next samples.
     *
     * @param sampleRate The sample rate
     * @param channels The amount of channels
     * @param bits The amount of bits per sample
     */
    bool setSampleRate(uint16_t sampleRate, uint8_t channels, uint8_t bits);

    /**
     * Prepare the DMA-controller for a data transfer to the sound card.
     *
     * @param offset Offset to add to the base address, that is used for DMA-transfer (SoundBlaster::dmaMemory)
     * @param size The size of the buffer to be transferred
     */
    void prepareDma(uint32_t offset, uint32_t size) const;

    /**
     * Set the size of the buffer, that the DSP awaits to play.
     *
     * @param size The size
     */
    void setBufferSize(uint32_t size);

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

    Util::Async::Spinlock lock;

    IoPort resetPort;
    IoPort readDataPort;
    IoPort writeDataPort;
    IoPort readBufferStatusPort;

    uint8_t irqNumber;
    uint8_t dmaChannel;

    uint32_t dmaBufferSize = 0;
    uint8_t *dmaBuffer = nullptr;
    uint8_t *physicalDmaAddress = nullptr;

    bool receivedInterrupt = false;

    static Kernel::Logger log;

private:

    static const constexpr uint16_t FIRST_BASE_ADDRESS = 0x220;
    static const constexpr uint16_t LAST_BASE_ADDRESS = 0x280;
    static const constexpr uint32_t TIMEOUT = 10;

    static const constexpr double AUDIO_BUFFER_SIZE = 0.1;
};

}

#endif
