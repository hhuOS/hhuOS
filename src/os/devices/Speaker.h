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

#ifndef __PCSPK_include__
#define __PCSPK_include__

#include <cstdint>
#include "kernel/IOport.h"
#include "kernel/services/TimeService.h"

/**
 * Driver for the PC-Beeper speaker.
 *
 * @author Burak Akguel, Christian Gesse, Fabian Ruhland, Filip Krakowski, Michael Schoettner
 * @date HHU, 2017
 */
class Speaker {
    
private:
    IOport control;
    IOport data0;
    IOport data2;
    IOport ppi;

    /**
     * Wait for a given amount of time. Used to hold a note.
     *
     * @param time The time in ms
     */
    void delay (uint32_t time);

    TimeService *timeService = nullptr;

public:
    /**
     * Constructor.
     */
    Speaker();

    /**
     * Copy-constructor.
     */
    Speaker(const Speaker &copy) = delete;

    /**
     * Destructor.
     */
    ~Speaker() = delete;

    /**
     * Play a demo-melody.
     */
    void demo1();

    /**
     * Play a demo-melody.
     */
    void demo2();

    void mario();

    /**
     * Beep for a given time.
     *
     * @param The frequency
     * @param The length (in milliseconds)
     */
    void play(float f, uint32_t len);

    /**
     * Beep until Speaker::off() is called.
     *
     * @param f The frequency.
     */
    void play(float f);
    
    /**
     * Turn the speaker off
     */
    void off ();

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
};

#endif
