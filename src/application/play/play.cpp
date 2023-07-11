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

static const constexpr uint32_t BUFFER_SIZE = 4096;
static const constexpr uint8_t BAR_LENGTH = 25;

bool isRunning = true;

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
    Util::System::out << "]" << Util::Io::PrintStream::endl << timeString << Util::Io::PrintStream::flush;
}

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

    auto soundBlaster = Util::Sound::SoundBlaster(soundBlasterFile);
    auto waveFile = Util::Sound::WaveFile(inputFile);

    if (!soundBlaster.setAudioParameters(waveFile.getSamplesPerSecond(), waveFile.getNumChannels(), waveFile.getBitsPerSample())) {
        Util::System::error << "play: Failed to set sound card parameters!" << Util::Io::PrintStream::endl << Util::Io::PrintStream::flush;
        return -1;
    }

    Util::Async::Thread::createThread("Key-Listener", new Util::Async::FunctionPointerRunnable([]{
        Util::System::in.read();
        isRunning = false;
    }));

    Util::Graphic::Ansi::disableCursor();
    Util::System::out << "Playing '" << inputFile.getName() << "'... Press <ENTER> to stop." << Util::Io::PrintStream::endl;

    auto *fileBuffer = new uint8_t[BUFFER_SIZE];
    uint32_t remaining = waveFile.getDataSize();

    while (isRunning && remaining > 0) {
        printStatusLine(waveFile, remaining);
        Util::Graphic::Ansi::moveCursorToBeginningOfPreviousLine(0);

        uint32_t toWrite = remaining >= BUFFER_SIZE ? BUFFER_SIZE : remaining;
        waveFile.read(fileBuffer, 0, toWrite);
        soundBlaster.play(fileBuffer, toWrite);

        remaining -= toWrite;
    }

    Util::Graphic::Ansi::clearLine();
    Util::Graphic::Ansi::moveCursorToBeginningOfPreviousLine(0);
    Util::Graphic::Ansi::clearLine();
    Util::Graphic::Ansi::moveCursorToBeginningOfPreviousLine(0);
    Util::Graphic::Ansi::clearLine();
    Util::Graphic::Ansi::enableCursor();

    return 0;
}