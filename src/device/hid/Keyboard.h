/*
 * Copyright (C) 2018-2022 Heinrich-Heine-Universitaet Duesseldorf,
 * Institute of Computer Science, Department Operating Systems
 * Burak Akguel, Christian Gesse, Fabian Ruhland, Filip Krakowski, Michael Schoettner; Olaf Spinczyk, TU Dortmund
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

#ifndef HHUOS_KEYBOARD_H
#define HHUOS_KEYBOARD_H

#include "kernel/interrupt/InterruptHandler.h"
#include "device/cpu/IoPort.h"
#include "lib/util/stream/ByteArrayOutputStream.h"
#include "lib/util/stream/PipedOutputStream.h"
#include "kernel/log/Logger.h"
#include "Key.h"
#include "Ps2Device.h"

namespace Device {

/**
 * Driver for the Keyboard-Controller.
 *
 * @author  original by Olaf Spinczyk, TU Dortmund
 * 			modified by Michael Schoettner, Filip Krakowski, Fabian Ruhland, Burak Akguel, Christian Gesse
 */
class Keyboard : public Ps2Device, public Util::Stream::FilterInputStream, public Kernel::InterruptHandler {

public:
    /**
     * Copy Constructor.
     */
    Keyboard(const Keyboard &copy) = delete;

    /**
     * Assignment operator.
     */
    Keyboard &operator=(const Keyboard &other) = delete;

    /**
     * Destructor.
     */
    ~Keyboard() override = default;

    static Keyboard* initialize(Ps2Controller &controller);

    /**
     * Enable keyboard-interrupts.
     */
    void plugin() override;

    /**
     * Overriding function from InterruptHandler.
     */
    void trigger(const Kernel::InterruptFrame &frame) override;

private:
    /**
     * Constructor.
     */
    explicit Keyboard(Ps2Controller &controller);

    enum Command : uint8_t {
        SET_LED = 0xed,
        ECHO = 0xee,
        SCAN_CODE_SET = 0xf0,
        IDENTIFY = 0xf2,
        SET_TYPEMATIC_SPEED = 0xf3,
        ENABLE_SCANNING = 0xf4,
        DISABLE_SCANNING = 0xf5,
        SET_DEFAULT_PARAMETERS = 0xf6,
        SET_ALL_TYPEMATIC_ONLY = 0xf7,
        SET_ALL_MAKE_BREAK = 0xf8,
        SET_ALL_MAKE_ONLY = 0xf9,
        SET_ALL_TYPEMATIC_MAKE_BREAK = 0xfa,
        SET_KEY_TYPEMATIC = 0xfb,
        SET_KEY_MAKE_BREAK = 0xfc,
        SET_KEY_MAKE_ONLY = 0xfd,
        RESEND_LAST_BYTE = 0xfe,
        RESET = 0xff
    };

    enum Reply : uint8_t {
        ERROR_1 = 0x00,
        SELF_TEST_PASSED = 0xaa,
        ECHO_RESPONSE = 0xee,
        ACK = 0xfa,
        SELF_TEST_FAILED_1 = 0xfc,
        SELF_TEST_FAILED_2 = 0xfd,
        RESEND_LAST_COMMAND = 0xfe,
        ERROR_2 = 0xff
    };

    enum Led : uint8_t {
        CAPS_LOCK = 4,
        NUM_LOCK = 2,
        SCROLL_LOCK = 1
    };

    enum Code : uint8_t {
        BREAK_BIT = 0x80,
        PREFIX1 = 0xe0,
        PREFIX2 = 0xe1
    };

    bool writeKeyboardCommand(Device::Keyboard::Command command);

    bool writeKeyboardCommand(Device::Keyboard::Command command, uint8_t data);

    /**
     * Set or unset a specified led.
     * @param led The led
     * @param on On/Off
     */
    void setLed(Device::Keyboard::Led led, bool on);

    /**
     * Decode make- and break-codes from the keyboard.
     *
     * @return true, if the key is complete, or false if there are still make-/break-codes missing.
     */
    bool decodeKey(uint8_t code);

    /**
     * Get the ascii-code of the decoded key.
     */
    void getAsciiCode(uint8_t code);

    Key gather = Key();
    uint8_t prefix = 0;
    uint8_t leds = 0;

    Util::Stream::PipedOutputStream outputStream;
    Util::Stream::PipedInputStream inputStream;

    static uint8_t normalTab[];
    static uint8_t shiftTab[];
    static uint8_t altTab[];
    static uint8_t asciiNumTab[];
    static uint8_t scanNumTab[];

    static Kernel::Logger log;
};

}

#endif
