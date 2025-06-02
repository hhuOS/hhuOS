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

#ifndef HHUOS_PCSPEAKER_H
#define HHUOS_PCSPEAKER_H

#include <stdint.h>

namespace Device {
class IoPort;
}  // namespace Device

namespace Device::Sound {

/**
 * Driver for the music producing PC speaker.
 */
class PcSpeaker {

public:
    /**
     * Default Constructor.
     * Deleted, as this class has only static members.
     */
    PcSpeaker() = delete;

    /**
     * Copy Constructor.
     */
    PcSpeaker(const PcSpeaker &other) = delete;

    /**
     * Assignment operator.
     */
    PcSpeaker&operator=(const PcSpeaker &other) = delete;

    /**
     * Destructor.
     */
    ~PcSpeaker() = default;

    /**
     * Beep until Speaker::off() is called.
     *
     * @param f The frequency.
     */
    static void play(uint16_t frequency);

    /**
     * Turn the speaker off
     */
    static void off();

private:

    static IoPort controlPort;
    static IoPort dataPort2;
    static IoPort ppi;

};

}

#endif