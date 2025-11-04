/*
 * Copyright (C) 2017-2025 Heinrich Heine University Düsseldorf,
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

#ifndef PCMDEVICE_H
#define PCMDEVICE_H

#include "util/io/stream/FilterOutputStream.h"
#include "util/io/stream/PipedInputStream.h"
#include "util/io/stream/PipedOutputStream.h"

namespace Device {

class PcmDevice : public Util::Io::FilterOutputStream {

public:
    /**
     * Default Constructor.
     */
    PcmDevice();

    /**
     * Copy Constructor.
     */
    PcmDevice(const PcmDevice &other) = delete;

    /**
     * Assignment operator.
     */
    PcmDevice &operator=(const PcmDevice &other) = delete;

    /**
     * Destructor.
     */
    ~PcmDevice() override = default;

    virtual bool setPlaybackParameters(uint32_t sampleRate, uint8_t channels, uint8_t bitsPerSample) = 0;

    [[nodiscard]] virtual uint32_t getSampleRate() const = 0;

    [[nodiscard]] virtual uint8_t getChannels() const = 0;

    [[nodiscard]] virtual uint8_t getBitsPerSample() const = 0;

    virtual void play(const uint8_t *samples, uint32_t size) = 0;

    virtual void sourceDrained() = 0;

    void processWrittenSamples();

private:

    Util::Io::PipedInputStream inputStream;
    Util::Io::PipedOutputStream outputStream;

    uint8_t *buffer = new uint8_t[4096];
};

}

#endif
