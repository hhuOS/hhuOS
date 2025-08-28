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

#include <stdint.h>

#include "lib/util/async/Thread.h"
#include "lib/util/base/System.h"
#include "lib/util/base/ArgumentParser.h"
#include "lib/util/collection/Array.h"
#include "lib/util/io/file/File.h"
#include "lib/util/graphic/Ansi.h"
#include "lib/util/base/String.h"
#include "lib/util/io/stream/InputStream.h"
#include "lib/util/io/stream/PrintStream.h"
#include "lib/util/sound/WaveFile.h"
#include "lib/util/sound/AudioChannel.h"

static const constexpr uint32_t BUFFER_SIZE = 8192;
static const constexpr uint8_t BAR_LENGTH = 25;

void printStatusLine(const Util::Sound::WaveFile &waveFile, uint32_t remainingBytes) {
    auto passedSeconds = (waveFile.getDataSize() - remainingBytes) / waveFile.getBytesPerSecond();
    auto passedMinutes = passedSeconds / 60;
    auto totalSeconds = waveFile.getSampleCount() / waveFile.getSamplesPerSecond();
    auto totalMinutes = totalSeconds / 60;

    auto timeString = Util::String::format("%u:%02um/%u:%02um", passedMinutes, passedSeconds - passedMinutes * 60, totalMinutes, totalSeconds - totalMinutes * 60);
    auto percentage = static_cast<double>(passedSeconds) / totalSeconds;
    auto filledBar = static_cast<uint32_t>((BAR_LENGTH - 2) * percentage);

    Util::System::out << "[";
    for (uint32_t i = 0; i < filledBar; i++) {
        Util::System::out << "#";
    }
    for (uint32_t i = 0; i < (BAR_LENGTH - 2) - filledBar; i++) {
        Util::System::out << "-";
    }
    Util::System::out << "] " << timeString << Util::Io::PrintStream::flush;
}

int32_t main(int32_t argc, char *argv[]) {
    auto argumentParser = Util::ArgumentParser();
    argumentParser.setHelpText("Play .wav-files via a sound blaster card.\n"
                               "Usage: play [FILE]\n"
                               "Options:\n"
                               "  -h, --help: Show this help message");

    if (!argumentParser.parse(argc, argv)) {
        Util::System::error << argumentParser.getErrorString() << Util::Io::PrintStream::ln << Util::Io::PrintStream::flush;
        return -1;
    }

    auto arguments = argumentParser.getUnnamedArguments();
    if (arguments.length() == 0) {
        Util::System::error << "play: No arguments provided!" << Util::Io::PrintStream::ln << Util::Io::PrintStream::flush;
        return -1;
    }

    auto inputFile = Util::Io::File(arguments[0]);
    if (!inputFile.exists() || inputFile.isDirectory()) {
        Util::System::error << "play: '" << arguments[0] << "' could not be opened!" << Util::Io::PrintStream::ln << Util::Io::PrintStream::flush;
        return -1;
    }

    auto waveFile = Util::Sound::WaveFile(inputFile);
    auto audioChannel = Util::Sound::AudioChannel();

    Util::Graphic::Ansi::disableCursor();
    Util::Io::File::setAccessMode(Util::Io::STANDARD_INPUT, Util::Io::File::NON_BLOCKING);

    Util::System::out << "Playing '" << inputFile.getName() << "'... Press <ENTER> to stop." << Util::Io::PrintStream::ln;
    audioChannel.play();

    auto *fileBuffer = new uint8_t[BUFFER_SIZE];
    uint32_t remaining = waveFile.getDataSize();

    while (remaining > 0) {
        if (Util::System::in.read() > 0) {
            break;
        }

        Util::Graphic::Ansi::saveCursorPosition();
        printStatusLine(waveFile, remaining);
        Util::Graphic::Ansi::restoreCursorPosition();

        uint32_t toWrite = remaining >= BUFFER_SIZE ? BUFFER_SIZE : remaining;
        waveFile.read(fileBuffer, 0, toWrite);
        audioChannel.write(fileBuffer, 0, toWrite);

        remaining -= toWrite;
    }

    audioChannel.stop();
    while (audioChannel.getState() != Util::Sound::AudioChannel::STOPPED) {
        Util::Async::Thread::yield();
    }

    Util::Graphic::Ansi::clearLine();
    Util::Graphic::Ansi::moveCursorToBeginningOfPreviousLine(0);
    Util::Graphic::Ansi::clearLine();
    Util::Graphic::Ansi::moveCursorToBeginningOfPreviousLine(0);
    Util::Graphic::Ansi::clearLine();
    Util::Graphic::Ansi::enableCursor();

    return 0;
}