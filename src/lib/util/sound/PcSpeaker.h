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

#ifndef HHUOS_LIB_UTIL_SOUND_PCSPEAKER_H
#define HHUOS_LIB_UTIL_SOUND_PCSPEAKER_H

#include <stdint.h>

#include "util/io/file/File.h"
#include "util/io/stream/FileOutputStream.h"
#include "util/time/Timestamp.h"

namespace Util::Sound {

/// This class provides a user space interface to the PC speaker, allowing it to play sounds at various frequencies.
/// The kernel driver for the PC speaker exposes a file interface, which sets the frequency when written to.
/// This class wraps that file interface and exposes a more convenient API for playing sounds.
///
/// ## Example
/// ```c++
/// auto speaker = Util::Sound::PcSpeaker(Util::Io::File("/device/speaker")); // Create a new PcSpeaker instance
///
/// // Read frequencies from the input stream and play them using the speaker.
/// auto line = Util::System::in.readLine();
/// while (!line.endOfFile) {
///     // Parse the frequency from the line and play it
///     // (This example does not handle errors, so it assumes the line is always a valid number)
///     const auto frequency = Util::String::parseNumber<uint16_t>(line.content);
///     speaker.play(frequency);
///
///     // Read the next line from standard input
///     line = Util::System::in.readLine();
/// }
/// ```
class PcSpeaker {

public:
    /// Contains the frequencies of several musical notes in Hz.
    /// These values can be used directly with the `play()` method to produce the corresponding sound.
    enum Note : uint16_t {
        C0 = static_cast<uint16_t>(130.81),
        C0X = static_cast<uint16_t>(138.59),
        D0 = static_cast<uint16_t>(146.83),
        D0X = static_cast<uint16_t>(155.56),
        E0 = static_cast<uint16_t>(164.81),
        F0 = static_cast<uint16_t>(174.61),
        F0X = static_cast<uint16_t>(185.00),
        G0 = static_cast<uint16_t>(196.00),
        G0X = static_cast<uint16_t>(207.65),
        A0 = static_cast<uint16_t>(220.00),
        A0X = static_cast<uint16_t>(233.08),
        B0 = static_cast<uint16_t>(246.94),

        C1 = static_cast<uint16_t>(261.63),
        C1X = static_cast<uint16_t>(277.18),
        D1 = static_cast<uint16_t>(293.66),
        D1X = static_cast<uint16_t>(311.13),
        E1 = static_cast<uint16_t>(329.63),
        F1 = static_cast<uint16_t>(349.23),
        F1X = static_cast<uint16_t>(369.99),
        G1 = static_cast<uint16_t>(391.00),
        G1X = static_cast<uint16_t>(415.30),
        A1 = static_cast<uint16_t>(440.00),
        A1X = static_cast<uint16_t>(466.16),
        B1 = static_cast<uint16_t>(493.88),

        C2 = static_cast<uint16_t>(523.25),
        C2X = static_cast<uint16_t>(554.37),
        D2 = static_cast<uint16_t>(587.33),
        D2X = static_cast<uint16_t>(622.25),
        E2 = static_cast<uint16_t>(659.26),
        F2 = static_cast<uint16_t>(698.46),
        F2X = static_cast<uint16_t>(739.99),
        G2 = static_cast<uint16_t>(783.99),
        G2X = static_cast<uint16_t>(830.61),
        A2 = static_cast<uint16_t>(880.00),
        A2X = static_cast<uint16_t>(923.33),
        B2 = static_cast<uint16_t>(987.77),
        C3 = static_cast<uint16_t>(1046.50)
    };

    /// Create a new PcSpeaker instance using the specified file.
    /// The file should point to the PC speaker device file, typically `/device/speaker`.
    explicit PcSpeaker(const Io::File &speakerFile);

    /// Play a sound at the specified frequency.
    /// This is done by writing the frequency to the PC speaker device file, given in the constructor.
    /// This method returns immediately after writing the frequency. The sound will continue to play until
    /// a new frequency is written or the speaker is turned off using `turnOff()`.
    void play(uint16_t frequency);

    /// Play a sound at the specified frequency for a given length of time.
    /// This method writes the frequency to the PC speaker device file and then sleeps for the specified length of time.
    void play(uint16_t frequency, const Time::Timestamp &length);

    /// Turn off the PC speaker.
    /// This method writes a frequency of 0 to the PC speaker device file, which stops any sound currently playing.
    void turnOff();

private:

    Io::FileOutputStream stream;
};

}

#endif
