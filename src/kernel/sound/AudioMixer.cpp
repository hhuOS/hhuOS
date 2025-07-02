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

#include "AudioMixer.h"


#include "AudioChannelNode.h"
#include "AudioMixerRunnable.h"
#include "AudioMixerNode.h"
#include "kernel/service/FilesystemService.h"
#include "kernel/service/ProcessService.h"
#include "filesystem/memory/MemoryDriver.h"
#include "lib/util/sound/AudioChannel.h"
#include "lib/util/sound/SoundBlaster.h"

namespace Kernel {

AudioMixer::AudioMixer(const Util::String &masterOutputPath) : idGenerator(256), streams(256),
        masterOutputFile(masterOutputPath), masterOutputStream(masterOutputPath), runnable(new AudioMixerRunnable(*this, masterOutputStream, BUFFER_SIZE)),
        thread(Thread::createKernelProcessThread("Audio-Mixer", runnable)) {
    auto &processService = Service::getService<ProcessService>();
    processService.getScheduler().ready(thread);

    auto &filesystemService = Service::getService<FilesystemService>();
    auto &deviceFilesystem = filesystemService.getFilesystem().getVirtualDriver("/device");
    deviceFilesystem.addNode("/", new AudioMixerNode(*this, *runnable, thread));

    masterOutputFile.controlFile(Util::Sound::SoundBlaster::SET_AUDIO_PARAMETERS, Util::Array<uint32_t>({
        SAMPLES_PER_SECOND, NUM_CHANNELS, BITS_PER_SAMPLE
    }));
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
    streams[id] = new Util::Io::Pipe();

    // Create filesystem node for the channel
    auto &filesystemService = Service::getService<FilesystemService>();
    auto &deviceFilesystem = filesystemService.getFilesystem().getVirtualDriver("/device");
    auto *channelNode = new AudioChannelNode(id, *streams[id], *this);
    deviceFilesystem.addNode("/", channelNode);

    return streamLock.releaseAndReturn(true);
}

bool AudioMixer::deleteChannel(const uint32_t &id) {
    streamLock.acquire();

    auto *pipe = streams[id];
    if (pipe == nullptr) {
        streamLock.release();
        return false;
    }

    // Remove pipe
    activeStreams.remove(pipe);
    streams[id] = nullptr;

    streamLock.release();

    // Remove channel node from filesystem (also delete the pipe)
    auto &filesystemService = Service::getService<FilesystemService>();
    filesystemService.deleteFile(Util::String::format("channel%u", id));

    idGenerator.unset(id);
    return true;
}

bool AudioMixer::controlPlayback(const uint32_t &request, const uint32_t &id) {
    streamLock.acquire();
    auto *pipe = streams[id];
    switch (request) {
        case Util::Sound::AudioChannel::PLAY:
            if (!activeStreams.contains(pipe))
                activeStreams.add(pipe);
            break;
        case Util::Sound::AudioChannel::STOP:
            activeStreams.remove(pipe);
            break;
        default:
            break;
    }
    streamLock.release();
    return true;
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
        auto toRead = channel->getReadableBytes();
        // Limit maximum read bytes by half the buffer size so the whole pipe can't be emptied in one go.
        toRead = toRead > BUFFER_SIZE / 2 ? BUFFER_SIZE / 2 : toRead;
        toRead = toRead > length ? length : toRead;

        // Read from single channel pipe
        const auto readBytes = channel->read(streamBuffer, 0, toRead);
        if (readBytes == 0) {
            continue;
        }

        // Adjust the maximum number of read bytes so far
        maxReadBytes = maxReadBytes > readBytes ? maxReadBytes : readBytes;
        // Add channel data to mixed buffer
        sum(readBytes);
    }

    streamLock.release();

    // Remove offset shift and clamp signal within valid range
    clamp(targetBuffer, maxReadBytes);

    return maxReadBytes;
}

int16_t AudioMixer::peek() {
    Util::Panic::fire(Util::Panic::UNSUPPORTED_OPERATION, "AudioMixer: Peek operation is not supported!");
}

bool AudioMixer::isReadyToRead() {
    for (auto *channel : activeStreams) {
        if (channel->getReadableBytes() > 0) {
            return true; // At least one channel has data to read
        }
    }

    return false;
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

void AudioMixer::writeToOutput(const uint8_t *sourceBuffer, const uint32_t length) {
    masterOutputStream.write(sourceBuffer, 0, length);
}

}