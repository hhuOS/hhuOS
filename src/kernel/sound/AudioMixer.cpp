/*
 * Copyright (C) 2017-2026 Heinrich Heine University Düsseldorf,
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
 *
 * The audio mixer is based on a bachelor's thesis, written by Andreas Lüpertz.
 * The original source code can be found here: https://git.hhu.de/bsinfo/thesis/ba-ANLU89
 */

#include "AudioMixer.h"


#include "AudioChannelNode.h"
#include "AudioMixerRunnable.h"
#include "AudioMixerNode.h"
#include "kernel/service/FilesystemService.h"
#include "kernel/service/ProcessService.h"
#include "filesystem/memory/MemoryDriver.h"
#include "lib/util/sound/AudioChannel.h"

namespace Kernel {

AudioMixer::AudioMixer() : idGenerator(256), streams(256),
        runnable(new AudioMixerRunnable(*this, BUFFER_SIZE)),
        thread(Thread::createKernelProcessThread("Audio-Mixer", runnable)) {
    auto &processService = Service::getService<ProcessService>();
    processService.getScheduler().ready(thread);

    auto &filesystemService = Service::getService<FilesystemService>();
    auto &deviceFilesystem = filesystemService.getFilesystem().getVirtualDriver("/device");
    deviceFilesystem.addNode("/", new AudioMixerNode(*this, *runnable, thread));
}

AudioMixer::~AudioMixer() {
    delete[] mixBuffer;
    delete[] streamBuffer;
    delete[] playing;
}

bool AudioMixer::createChannel(uint8_t &id) {
    // Generate new channel ID
    const auto newId = idGenerator.findAndSet();
    if (newId == Util::Async::AtomicBitmap::INVALID_INDEX) {
        return false; // No free ID available
    }
    id = newId;

    streamLock.acquire();

    // Generate pipe
    streams[id] = new AudioChannel(BUFFER_SIZE);

    // Create filesystem node for the channel
    auto &filesystemService = Service::getService<FilesystemService>();
    auto &deviceFilesystem = filesystemService.getFilesystem().getVirtualDriver("/device");
    auto *channelNode = new AudioChannelNode(id, *streams[id], *this);
    bool success = deviceFilesystem.addNode("/", channelNode);

    return streamLock.releaseAndReturn(success);
}

bool AudioMixer::deleteChannel(const uint32_t id) {
    streamLock.acquire();

    auto *pipe = streams[id];
    if (pipe == nullptr) {
        streamLock.release();
        return false;
    }

    // Remove pipe
    activeStreams.remove(pipe);
    streams[id] = nullptr;
    delete pipe;

    streamLock.release();

    // Remove filesystem node for the channel
    auto &filesystemService = Service::getService<FilesystemService>();
    filesystemService.deleteFile(Util::String::format("/device/channel%u", id));

    idGenerator.unset(id);
    return true;
}

void AudioMixer::controlPlayback(const Util::Sound::AudioChannel::Request request, const uint32_t id) {
    streamLock.acquire();
    auto *pipe = streams[id];

    switch (request) {
        case Util::Sound::AudioChannel::PLAY:
            if (!activeStreams.contains(pipe)) {
                activeStreams.add(pipe);
            }

            pipe->setState(Util::Sound::AudioChannel::PLAYING);
            break;
        case Util::Sound::AudioChannel::STOP:
            if (activeStreams.contains(pipe)) {
                pipe->setState(Util::Sound::AudioChannel::FLUSHING);
            }
            break;
        default:
            break;
    }

    streamLock.release();
}

void AudioMixer::setMasterOutputDevice(Device::PcmDevice &device) const {
    device.setPlaybackParameters(SAMPLES_PER_SECOND, NUM_CHANNELS, BITS_PER_SAMPLE);
    runnable->setMasterOutputDevice(device);
}

int16_t AudioMixer::read() {
    uint8_t c = 0;
    read(&c, 0, 1);

    return c;
}

int32_t AudioMixer::read(uint8_t *targetBuffer, [[maybe_unused]] uint32_t offset, uint32_t length) {
    streamLock.acquire();

    int32_t maxReadBytes = 0;
    // Iterate over list of active channel streams
    for (auto *channel : activeStreams) {
        const auto readable = channel->getReadableBytes();
        auto toRead = readable > length ? length : readable;
        if (toRead > BUFFER_SIZE / 2) {
            toRead = BUFFER_SIZE / 2;
        }

        // Read from single channel pipe
        const auto readBytes = channel->read(streamBuffer, 0, toRead);
        if (readBytes == 0) {
            // Nothing read from channel -> Check if channel was flushing
            if (channel->getState() == Util::Sound::AudioChannel::FLUSHING) {
                flushedStreams.add(channel);
            }

            continue;
        }

        // Adjust the maximum number of read bytes so far
        maxReadBytes = maxReadBytes > readBytes ? maxReadBytes : readBytes;
        // Add channel data to mixed buffer
        sum(readBytes);
    }

    // Remove any flushed streams from the active streams list
    for (auto *channel : flushedStreams) {
        activeStreams.remove(channel);
        channel->setState(Util::Sound::AudioChannel::STOPPED);
    }
    flushedStreams.clear();

    streamLock.release();

    // Remove offset shift and clamp signal within valid range
    clamp(targetBuffer, maxReadBytes);

    return maxReadBytes;
}

int16_t AudioMixer::peek() {
    Util::Panic::fire(Util::Panic::UNSUPPORTED_OPERATION, "AudioMixer: Peek operation is not supported!");
}

bool AudioMixer::isReadyToRead() {
    streamLock.acquire();

    for (auto *channel : activeStreams) {
        if (channel->getReadableBytes() > 0) {
            return true; // At least one channel has data to read
        }
    }

    return streamLock.releaseAndReturn(false);
}

void AudioMixer::sum(uint32_t numBytes) const {
    for (uint32_t i = 0; i < numBytes; i++) {
        mixBuffer[i] += streamBuffer[i];
        playing[i]++;
    }
}

void AudioMixer::clamp(uint8_t *targetBuffer, uint32_t numBytes) const {
    for (uint32_t i = 0; i < numBytes; i++) {
        // calculate offset shift by number of streams involved per sample
        auto shift = (playing[i] - 1) * 0x80;
        auto sample = mixBuffer[i];
        // subtract shift if possible or set sample to 0
        sample -= sample > shift ? shift : sample;
        // clamp down sample if above upper limit
        sample = sample > UINT8_MAX ? UINT8_MAX : sample;
        // write sample to target buffer
        targetBuffer[i] = sample;
        // empty buffers for next mixing cycle
        mixBuffer[i] = 0;
        playing[i] = 0;
    }
}

}