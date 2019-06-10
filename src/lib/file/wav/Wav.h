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

#ifndef HHUOS_WAV_H
#define HHUOS_WAV_H

#include "lib/file/File.h"
#include "lib/sound/Pcm.h"

class Wav : public Pcm {

private:

    struct FormatChunk {
        char formatSignature[4];
        uint32_t chunkSize;
        Pcm::AudioFormat audioFormat;
        uint16_t numChannels;
        uint32_t samplesPerSecond;
        uint32_t bytesPerSecond;
        uint16_t frameSize;
        uint16_t bitsPerSample;
    } __attribute__ ((packed));

    struct DataChunk {
        char dataSignature[4];
        uint32_t chunkSize;
    } __attribute__ ((packed));

    struct RiffChunk {
        char riffSignature[4];
        uint32_t chunkSize;
        char waveSignature[4];
        FormatChunk formatChunk;
        DataChunk dataChunk;
    } __attribute__ ((packed));

private:

    char *rawData = nullptr;

private:

    explicit Wav(File *file);

    void processData();

public:

    static Wav *load(const String &path);

    Wav(const Wav &copy) = delete;

    ~Wav();

};

#endif
