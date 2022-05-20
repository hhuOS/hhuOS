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

namespace Device {

/**
 * Driver for the Keyboard-Controller.
 *
 * @author  original by Olaf Spinczyk, TU Dortmund
 * 			modified by Michael Schoettner, Filip Krakowski, Fabian Ruhland, Burak Akguel, Christian Gesse
 */
class Keyboard : public Kernel::InterruptHandler {

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

    /**
     * Create a keyboard instance, which is readable at '/device/keyboard'
     */
     static void initialize();

    /**
     * Set the controller's repeat rate.
     *
     * @param speed The dataRate, at which the controller shall operate
     * @param delay The delay
     */
    void setRepeatRate(uint32_t speed, uint32_t delay);

    /**
     * Set or unset a specified led.
     * @param led The led
     * @param on On/Off
     */
    void setLed(uint8_t led, bool on);

    /**
     * Get the amount of pressed keys.
     */
    [[nodiscard]] uint32_t getKeysPressed() const;

    /**
     * Check, if a specified key is pressed.
     */
    [[nodiscard]] bool isKeyPressed(uint32_t scancode) const;

    /**
     * Enable keyboard-interrupts.
     */
    void plugin() override;

    /**
     * Overriding function from InterruptHandler.
     */
    void trigger(Kernel::InterruptFrame &frame) override;

private:

    enum KeyboardStatus {
        OUTB = 0x01,
        INPB = 0x02,
        AUXB = 0x20
    };

    enum KeyboardCommand {
        SET_LED = 0xed,
        SET_SPEED = 0xf3,
        CPU_RESET = 0xfe
    };

    enum KeyboardLed {
        CAPS_LOCK = 4,
        NUM_LOCK = 2,
        SCROLL_LOCK = 1
    };

    enum KeyboardReply {
        ACK = 0xfa
    };

    enum KeyboardCode {
        BREAK_BIT = 0x80,
        PREFIX1 = 0xe0,
        PREFIX2 = 0xe1
    };

    /**
     * Constructor.
     */
    explicit Keyboard(Util::Stream::PipedInputStream &inputStream);

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

    /**
     * Add a key to the software-buffer.
     */
    void addToBuffer(uint32_t scancode);

    /**
     * Remove a key from the software-buffer.
     */
    void removeFromBuffer(uint32_t scancode);

    static const constexpr uint32_t BUFFER_SIZE = 16;

    static Kernel::Logger log;

    static uint8_t normalTab[];
    static uint8_t shiftTab[];
    static uint8_t altTab[];
    static uint8_t asciiNumTab[];
    static uint8_t scanNumTab[];

    const IoPort controlPort = IoPort(0x64);
    const IoPort dataPort = IoPort(0x60);

    Key gather = Key();
    uint8_t prefix = 0;
    uint8_t leds = 0;

    uint32_t keysPressed = 0;
    uint32_t buffer[BUFFER_SIZE]{};

    Util::Stream::PipedOutputStream outputStream;
};

}

#endif
