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

#ifndef __Key_include__
#define __Key_include__

#include <cstdint>

/**
 * Represents a key, consisting of a key- and an ascii-code.
 *
 * @author Olaf Spinczyk, TU Dortmund; Fabian Ruhland, HHU
 * @date 2016, 2018
 */
class Key {

private:

    uint8_t asc;
    uint8_t scan;
    uint8_t modi;
    bool pressed;

private:

    /**
     * Bit-masks for key-modifiers.
     */
    struct mbit {
        enum {
            shift       = 1,
            alt_left    = 2,
            alt_right   = 4,
            ctrl_left   = 8,
            ctrl_right  = 16,
            caps_lock   = 32,
            num_lock    = 64,
            scroll_lock = 128
        };
    };

public:

    /**
     * Constructor.
     */
    Key();

    /**
     * Check, if a key is valid(scancode == 0 -> invalid).
     */
    bool valid();

    /**
     * Check, if a key is being pressed.
     */
    bool isPressed();

    /**
     * Set the pressed-value.
     */
    void setPressed(bool value);

    /**
     * Invalidate this key(Set scancode to 0).
     */
    void invalidate();

    /**
     * Set the ascii-code.
     */
    void ascii(uint8_t a);

    /**
     * Set the scancode.
     */
    void scancode(uint8_t s) ;

    /**
     * Get the ascii-code.
     */
    unsigned char ascii();

    /**
     * Get the scancode.
     */
    unsigned char scancode();

    /**
     * Set the shift-modifier.
     */
    void shift(bool pressed);

    /**
     * Set the alt-modifier for the left alt-key.
     */
    void altLeft(bool pressed);

    /**
     * Set the alt-modifier for the right alt-key.
     */
    void altRight(bool pressed);

    /**
     * Set the ctrl-modifier for the left alt-key.
     */
    void ctrlLeft(bool pressed);

    /**
     * Set the ctrl-modifier for the right alt-key.
     */
    void ctrlRight(bool pressed);

    /**
     * Set the caps-lock modifier.
     */
    void capsLock(bool pressed);

    /**
     * Set the num-lock modifier.
     */
    void numLock(bool pressed);

    /**
     * Set the scroll-lock modifier.
     */
    void scrollLock(bool pressed);


    /**
     * Get the shift-modifier.
     */
    bool shift();

    /**
     * Get the alt-modifier for the left alt-key.
     */
    bool altLeft();

    /**
     * Get the alt-modifier for the right alt-key.
     */
    bool altRight();

    /**
     * Get the ctrl-modifier for the left alt-key.
     */
    bool ctrlLeft();

    /**
     * Get the ctrl-modifier for the right alt-key.
     */
    bool ctrlRight();

    /**
     * Get the caps-lock modifier.
     */
    bool capsLock();

    /**
     * Get the num-lock modifier.
     */
    bool numLock();

    /**
     * Get the scroll-lock modifier.
     */
    bool scrollLock();

    /**
     * Check, if one of the alt-keys is pressed.
     */
    bool alt();

    /**
     * Check, if one of the ctrl-keys is pressed.
     */
    bool ctrl();

    /**
     *(char)-operator. Return the ascii-code.
     */
    explicit operator char();

    /**
     *(unsigned char)-operator. Return the ascii-code.
     */
    explicit operator unsigned char();

    /**
     * Some pre-defined scancodes.
     */
    enum {
        f1 = 0x3b,
        del = 0x53,
        up=72,
        down=80,
        left=75,
        right=77,
        div = 8
    };
 };

#endif
