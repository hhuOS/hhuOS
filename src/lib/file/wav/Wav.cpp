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

#include "Wav.h"

Wav::Wav(File *file) {
    *file >> rawData;

    processData();

    delete rawData;
}

Wav::~Wav() {
    delete pcmData;
}

Wav *Wav::load(const String &path) {
    File *file = File::open(path, "r");

    if(file == nullptr) {
        return nullptr;
    }

    auto ret = new Wav(file);

    delete file;

    return ret;
}

void Wav::processData() {
    RiffChunk riffChunk = *((RiffChunk*) rawData);

    audioFormat = riffChunk.formatChunk.audioFormat;
    bitsPerSample = riffChunk.formatChunk.bitsPerSample;
    numChannels = riffChunk.formatChunk.numChannels;
    samplesPerSecond = riffChunk.formatChunk.samplesPerSecond;
    bytesPerSecond = riffChunk.formatChunk.bytesPerSecond;
    bitsPerSample = riffChunk.formatChunk.bitsPerSample;
    frameSize = static_cast<uint16_t>(riffChunk.formatChunk.numChannels * ((bitsPerSample + 7) / 8));
    sampleCount = riffChunk.dataChunk.chunkSize / frameSize;

    pcmData = new uint8_t[riffChunk.dataChunk.chunkSize];
    memcpy(pcmData, &rawData[sizeof(RiffChunk)], riffChunk.dataChunk.chunkSize);
}
