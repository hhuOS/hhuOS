/*
 * Copyright (C) 2018-2022 Heinrich-Heine-Universitaet Duesseldorf,
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

#ifndef HHUOS_PCSPEAKER_H
#define HHUOS_PCSPEAKER_H

#include <cstdint>

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
    PcSpeaker &operator=(const PcSpeaker &other) = delete;

    /**
     * Destructor.
     */
    ~PcSpeaker() = default;

    /**
     * Beep until Speaker::off() is called.
     *
     * @param f The frequency.
     */
    static void play(uint32_t frequency);

    /**
     * Turn the speaker off
     */
    static void off();

    static const constexpr float C0 = 130.81;
    static const constexpr float C0X = 138.59;
    static const constexpr float D0 = 146.83;
    static const constexpr float D0X = 155.56;
    static const constexpr float E0 = 164.81;
    static const constexpr float F0 = 174.61;
    static const constexpr float F0X = 185.00;
    static const constexpr float G0 = 196.00;
    static const constexpr float G0X = 207.65;
    static const constexpr float A0 = 220.00;
    static const constexpr float A0X  = 233.08;
    static const constexpr float B0 = 246.94;

    static const constexpr float C1 = 261.63;
    static const constexpr float C1X = 277.18;
    static const constexpr float D1 = 293.66;
    static const constexpr float D1X = 311.13;
    static const constexpr float E1 = 329.63;
    static const constexpr float F1 = 349.23;
    static const constexpr float F1X = 369.99;
    static const constexpr float G1 = 391.00;
    static const constexpr float G1X = 415.30;
    static const constexpr float A1 = 440.00;
    static const constexpr float A1X = 466.16;
    static const constexpr float B1 = 493.88;

    static const constexpr float C2 = 523.25;
    static const constexpr float C2X = 554.37;
    static const constexpr float D2 = 587.33;
    static const constexpr float D2X = 622.25;
    static const constexpr float E2 = 659.26;
    static const constexpr float F2 = 698.46;
    static const constexpr float F2X = 739.99;
    static const constexpr float G2 = 783.99;
    static const constexpr float G2X = 830.61;
    static const constexpr float A2 = 880.00;
    static const constexpr float A2X = 923.33;
    static const constexpr float B2 = 987.77;
    static const constexpr float C3 = 1046.50;

private:

    static IoPort controlPort;
    static IoPort dataPort2;
    static IoPort ppi;

};

}

#endif