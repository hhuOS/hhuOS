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
 */

#include <stdint.h>

#include <util/async/Thread.h>
#include <util/base/System.h>
#include <util/base/ArgumentParser.h>
#include <util/collection/Array.h>
#include <util/io/file/File.h>
#include <util/graphic/Ansi.h>
#include <util/base/String.h>
#include <util/io/stream/InputStream.h>
#include <util/io/stream/PrintStream.h>
#include <util/sound/WaveFile.h>
#include <util/sound/AudioChannel.h>

constexpr const char *HELP_TEXT =
#include "generated/README.md"
;

static constexpr size_t BUFFER_SIZE = 8192;
static constexpr size_t BAR_LENGTH = 25;

void printStatusLine(const Util::Sound::WaveFile &waveFile, const size_t remainingBytes) {
    const auto passedTime = Util::Time::Timestamp::ofSeconds(
        (waveFile.getDataSize() - remainingBytes) / waveFile.getBytesPerSecond());
    const auto totalLength = Util::Time::Timestamp::ofSeconds(
        waveFile.getSampleCount() / waveFile.getSamplesPerSecond());

    const auto passedSeconds = passedTime.toSeconds();
    const auto passedMinutes = passedTime.toMinutes();
    const auto totalSeconds = totalLength.toSeconds();
    const auto totalMinutes = totalLength.toMinutes();

    const auto timeString = Util::String::format("%u:%02um/%u:%02um",
        passedMinutes, passedSeconds - passedMinutes * 60,
        totalMinutes, totalSeconds - totalMinutes * 60);

    const auto percentage = totalLength.toMilliseconds() == 0 ? 0 :
        passedTime.toMilliseconds() * 100 / totalLength.toMilliseconds();
    const auto filledBar = BAR_LENGTH * percentage / 100;

    Util::System::out << "[";
    for (size_t i = 0; i < filledBar; i++) {
        Util::System::out << "#";
    }
    for (size_t i = 0; i < BAR_LENGTH - filledBar; i++) {
        Util::System::out << "-";
    }
    Util::System::out << "] " << timeString << Util::Io::PrintStream::flush;
}

int32_t main(const int32_t argc, char *argv[]) {
    Util::ArgumentParser argumentParser;
    argumentParser.setHelpText(HELP_TEXT);

    if (!argumentParser.parse(argc, argv)) {
        Util::System::error << argumentParser.getErrorString() << Util::Io::PrintStream::lnFlush;
        return -1;
    }

    auto arguments = argumentParser.getUnnamedArguments();
    if (arguments.length() == 0) {
        Util::System::error << "play: No arguments provided!" << Util::Io::PrintStream::lnFlush;
        return -1;
    }

    const Util::Io::File inputFile(arguments[0]);
    if (!inputFile.exists() || inputFile.isDirectory()) {
        Util::System::error << "play: '" << arguments[0] << "' could not be opened!" << Util::Io::PrintStream::lnFlush;
        return -1;
    }

    Util::Sound::WaveFile waveFile(inputFile);
    Util::Sound::AudioChannel audioChannel;

    Util::Graphic::Ansi::enableKeyboardScancodes();
    Util::Graphic::Ansi::disableCursor();
    Util::Io::File::setAccessMode(Util::Io::STANDARD_INPUT, Util::Io::File::NON_BLOCKING);

    const Util::Io::DeLayout layout;
    Util::Io::KeyDecoder keyDecoder(layout);

    Util::System::out << "Playing '" << inputFile.getName() << "'... Press <ESC> to stop." << Util::Io::PrintStream::ln;

    audioChannel.play();

    auto *fileBuffer = new uint8_t[BUFFER_SIZE];
    size_t remaining = waveFile.getDataSize();

    while (remaining > 0) {
        // Exit application if ESC is pressed
        if (Util::System::in.isReadyToRead()) {
            if (keyDecoder.parseScancode(Util::System::in.read())) {
                if (keyDecoder.getKeyEvent().getScancode() == Util::Io::KeyEvent::ESC) {
                    break;
                }
            }
        }

        // Update the status line
        Util::Graphic::Ansi::saveCursorPosition();
        printStatusLine(waveFile, remaining);
        Util::Graphic::Ansi::restoreCursorPosition();

        // Read data from the wave file and write it to the audio channel
        const auto toWrite = remaining >= BUFFER_SIZE ? BUFFER_SIZE : remaining;
        waveFile.read(fileBuffer, 0, toWrite);
        audioChannel.write(fileBuffer, 0, toWrite);

        remaining -= toWrite;
    }

    // End of file reached (or ESC pressed) -> Stop audio playback
    audioChannel.stop(true);

    Util::Graphic::Ansi::clearLine();
    Util::Graphic::Ansi::moveCursorToBeginningOfPreviousLine(0);
    Util::Graphic::Ansi::clearLine();
    Util::Graphic::Ansi::moveCursorToBeginningOfPreviousLine(0);
    Util::Graphic::Ansi::clearLine();
    Util::Graphic::Ansi::enableCursor();

    return 0;
}