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

#include <cstdint>

#include "lib/util/base/System.h"
#include "lib/util/time/Timestamp.h"
#include "lib/util/async/Thread.h"
#include "lib/util/async/FunctionPointerRunnable.h"
#include "lib/util/base/ArgumentParser.h"
#include "lib/util/collection/Array.h"
#include "lib/util/io/file/File.h"
#include "lib/util/graphic/Ansi.h"
#include "lib/util/base/String.h"
#include "lib/util/io/stream/FileOutputStream.h"
#include "lib/util/io/stream/FileInputStream.h"
#include "lib/util/io/stream/BufferedInputStream.h"
#include "lib/util/io/stream/InputStream.h"
#include "lib/util/io/stream/PrintStream.h"
#include "lib/util/sound/SoundBlaster.h"
#include "lib/util/sound/WaveFile.h"

static const constexpr uint32_t BUFFER_SIZE = 8192;

int32_t main(int32_t argc, char *argv[]) {
    auto argumentParser = Util::ArgumentParser();
    argumentParser.setHelpText("Play .wav-files via a sound blaster card.\n"
                               "Usage: play [FILE]\n"
                               "Options:\n"
                               "  -h, --help: Show this help message");

    if (!argumentParser.parse(argc, argv)) {
        Util::System::error << argumentParser.getErrorString() << Util::Io::PrintStream::endl << Util::Io::PrintStream::flush;
        return -1;
    }

    auto arguments = argumentParser.getUnnamedArguments();
    if (arguments.length() == 0) {
        Util::System::error << "play: No arguments provided!" << Util::Io::PrintStream::endl << Util::Io::PrintStream::flush;
        return -1;
    }

    auto inputFile = Util::Io::File(arguments[0]);
    if (!inputFile.exists() || inputFile.isDirectory()) {
        Util::System::error << "play: '" << arguments[0] << "' could not be opened!" << Util::Io::PrintStream::endl << Util::Io::PrintStream::flush;
        return -1;
    }

    auto soundBlasterFile = Util::Io::File("/device/soundblaster");
    if (!soundBlasterFile.exists() || soundBlasterFile.isDirectory()) {
        Util::System::error << "play: '/device/soundblaster' could not be opened!" << Util::Io::PrintStream::endl << Util::Io::PrintStream::flush;
        return -1;
    }

    auto waveFile = Util::Sound::WaveFile(inputFile);
    auto outputStream = Util::Io::FileOutputStream(soundBlasterFile);

    if (!soundBlasterFile.control(Util::Sound::SoundBlaster::SET_AUDIO_PARAMETERS, Util::Array<uint32_t>({waveFile.getSamplesPerSecond(), waveFile.getNumChannels(), waveFile.getBitsPerSample()}))) {
        Util::System::error << "play: Failed to set sample rate!" << Util::Io::PrintStream::endl << Util::Io::PrintStream::flush;
        return -1;
    }

    auto *fileBuffer = new uint8_t[BUFFER_SIZE];

    uint32_t remaining = waveFile.getDataSize();
    while (remaining > 0) {
        uint32_t toWrite = remaining >= BUFFER_SIZE ? BUFFER_SIZE : remaining;
        waveFile.read(fileBuffer, 0, toWrite);
        outputStream.write(fileBuffer, 0, toWrite);

        remaining -= toWrite;
    }

    return 0;
}