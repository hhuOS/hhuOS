/*
 * Copyright (C) 2018-2025 Heinrich-Heine-Universitaet Duesseldorf,
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
 *
 * The audio mixer is based on a bachelor's thesis, written by Andreas Lüpertz.
 * The original source code can be found here: https://git.hhu.de/bsinfo/thesis/ba-ANLU89
 */

#ifndef HHUOS_AUDIOMIXER_H
#define HHUOS_AUDIOMIXER_H

#include "async/AtomicBitmap.h"
#include "device/sound/PcmDevice.h"
#include "kernel/process/Thread.h"
#include "lib/util/io/file/File.h"
#include "lib/util/io/stream/FileOutputStream.h"
#include "lib/util/io/stream/PipedInputStream.h"
#include "io/stream/Pipe.h"

namespace Kernel {

class AudioMixerRunnable;

class AudioMixer : public Util::Io::InputStream {

public:
    /**
     * Constructor.
     */
    explicit AudioMixer();

    /**
     * Copy Constructor.
     */
    AudioMixer(const AudioMixer &other) = delete;

    /**
     * Assignment operator.
     */
    AudioMixer &operator=(const AudioMixer &other) = delete;

    /**
     * Destructor.
     */
    ~AudioMixer() override;

    bool createChannel(uint8_t &id);

    bool deleteChannel(const uint32_t &id);

    bool controlPlayback(const uint32_t &request, const uint32_t &id);

    void setMasterOutputDevice(Device::PcmDevice &device) const;

    int16_t read() override;

    int32_t read(uint8_t *targetBuffer, uint32_t offset, uint32_t length) override;

    int16_t peek() override;

    bool isReadyToRead() override;

private:

    void clamp(uint8_t *targetBuffer, uint32_t numBytes) const;

    void sum(uint32_t numBytes) const;

    uint8_t *streamBuffer = new uint8_t[BUFFER_SIZE]();
    uint16_t *mixBuffer = new uint16_t[BUFFER_SIZE]();
    uint8_t *playing = new uint8_t[BUFFER_SIZE]();

    Util::Async::Spinlock streamLock;
    Util::Async::AtomicBitmap idGenerator;
    Util::Array<Util::Io::Pipe*> streams;
    Util::ArrayList<Util::Io::Pipe*> activeStreams;

    AudioMixerRunnable *runnable;
    Thread &thread;

    static constexpr uint32_t AUDIO_BUFFER_SIZE_MS = 100;
    static constexpr uint32_t SAMPLES_PER_SECOND = 22050;
    static constexpr uint8_t BITS_PER_SAMPLE = 8;
    static constexpr uint8_t NUM_CHANNELS = 1;
    static constexpr uint32_t BUFFER_SIZE = (AUDIO_BUFFER_SIZE_MS * SAMPLES_PER_SECOND * (BITS_PER_SAMPLE / 8) * NUM_CHANNELS) / 1000;
};

}

#endif
