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

#ifndef HHUOS_SOUNDBLASTER1_H
#define HHUOS_SOUNDBLASTER1_H

#include "SoundBlaster.h"

/**
 * Driver for SoundBlaster cards, which use a DSP with version number 1.xx.
 * The IRQ-number and DMA-channel, which the card use, have to be set by using jumpers on the card.
 * If not specified else in the constructor, this driver assumes that IRQ 10 and DMA-channel 1 are used.
 *
 * SoundBlaster cards with DSP version 1.xx only support the "single-cycle" DMA-mode, which means, that after
 * a buffer has been transferred, the DSP as well as the DMA-controller have to be set up again for the next
 * buffer. This may result in short clicking noises after each transfer, as the card may not have any data to play,
 * between to buffers. To eleviate this problem to some extent, this driver uses a double buffering mechanism,
 * where a 64KB DMA-region is split up into two 32K regions. While the card is receiving data from one of the
 * regions, the next 32KB of data are already being copied to the other region. This way, the delay between to buffers
 * is as short as possible.
 *
 * Supported audio formats:
 *      -Mono PCM, 4000-23000 Hz, 8-bit samples
 */
class SoundBlaster1 : public SoundBlaster, public InterruptHandler {

private:

    bool receivedInterrupt = false;

    bool stopPlaying = false;

    uint8_t irqNumber, dmaChannel;

private:

    /**
     * Prepare the DMA-controller for a data transfer to the sound card.
     *
     * @param addressOffset Offset to add to the base address, that is used for DMA-transfer (SoundBlaster::dmaMemory)
     * @param bufferSize The size of the buffer to be transferred
     */
    void prepareDma(uint16_t addressOffset, uint32_t bufferSize);

    /**
     * Set the sampling rate, that the DSP shall use to play the next samples.
     *
     * @param samplingRate The sampling rate
     */
    void setSamplingRate(uint16_t samplingRate);

    /**
     * Set the size of the buffer, that the DSP awaits to play.
     *
     * @param bufferSize The size
     */
    void setBufferSize(uint32_t bufferSize);

    /**
     * Acknowledge an interrupt.
     */
    void ackInterrupt();

public:

    /**
     * Constructor.
     *
     * @param baseAddress The IO-port's start address
     * @param irqNumber The IRQ number to use
     * @param dmaChannel The DMA channel to use
     */
    explicit SoundBlaster1(uint16_t baseAddress, uint8_t irqNumber = 10, uint8_t dmaChannel = 1);

    /**
     * Destructor.
     */
    ~SoundBlaster1() override = default;

    /**
     * Overriding function from SoundBlaster.
     */
    void playPcmData(const Pcm &pcm) override;

    /**
     * Overriding function from SoundBlaster.
     */
    void stopPlayback() override;

    /**
     * Enable interrupts for the sound card.
     */
    void plugin();

    /**
     * Overriding function from SoundBlaster.
     */
    void trigger() override;
};

#endif
