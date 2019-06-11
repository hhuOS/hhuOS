/*
 * Copyright (C) 2018 Burak Akguel, Christian Gesse, Fabian Ruhland, Filip Krakowski, Michael Schoettner
 * Heinrich-Heine University; Olaf Spinczyk, TU Dortmund
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

#ifndef __Keyboard_include__
#define __Keyboard_include__

#include "kernel/service/EventBus.h"
#include "Key.h"
#include "device/cpu/IoPort.h"
#include "kernel/interrupt/InterruptHandler.h"
#include "kernel/event/input/KeyEvent.h"
#include "lib/util/RingBuffer.h"
#include "lib/stream/InputStream.h"
#include "kernel/service/GraphicsService.h"
#include "lib/math/Random.h"

/**
 * Driver for the Keyboard-Controller.
 *
 * @author Olaf Spinczyk, TU Dortmund; Michael Schoettner, Filip Krakowski, Fabian Ruhland, HHU
 * @date 2016, 2017, 2018
 */
class Keyboard : public Kernel::InterruptHandler {

private:

    enum KeyboardStatus {
        outb = 0x01,
        inpb = 0x02,
        auxb = 0x20
    };

    enum KeyboardCommmand {
        set_led = 0xed,
        set_speed = 0xf3,
        cpu_reset = 0xfe
    };

    enum KeyboardLed {
        caps_lock = 4,
        num_lock = 2,
        scroll_lock = 1
    };

    enum KeyboardReply {
        ack = 0xfa
    };

    enum KeyboardCode {
        break_bit = 0x80,
        prefix1 = 0xe0,
        prefix2   = 0xe1
    };

private:

    static uint8_t normal_tab[];
    static uint8_t shift_tab[];
    static uint8_t alt_tab[];
    static uint8_t asc_num_tab[];
    static uint8_t scan_num_tab[];

    const static uint32_t KB_BUFFER_SIZE = 4;

private:

    uint8_t prefix;
    Key gather;
    uint8_t leds;

    uint32_t keysPressed;
    uint32_t buffer[KB_BUFFER_SIZE];

    const IoPort controlPort;
    const IoPort dataPort;

private:

    /**
     * Decode make- and break-codes from the keyboard.
     *
     * Return true, if the key is complete, or false if there are still make-/break-codes missing.
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

    /**
     * A pointer to the global event bus.
     */
    Kernel::EventBus *eventBus;

    Util::RingBuffer<uint8_t> interruptDataBuffer;

    void parseInterruptData() override;

public:

    /**
     * Constructor.
     */
    Keyboard();

    /**
     * Copy-constructor.
     * @param copy
     */
    Keyboard(const Keyboard &copy) = delete;

    /**
     * Set the controller's repeat rate.
     * @param speed The speed, at which the controller shall operate
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
    uint32_t getKeysPressed();

    /**
     * Check, if a specified key is pressed.
     */
    bool isKeyPressed(uint32_t scancode);

    /**
     * Reboot the PC.
     */
    void reboot();

    /**
     * Enable keyboard-interrupts.
     */
    void plugin();

    /**
     * Overriding function from IODevice.
     */
    void trigger(Kernel::InterruptFrame &frame) override;

    /**
     * Overriding function from IODevice.
     */
    bool hasInterruptData() override;
};

#endif
