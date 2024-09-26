/*
 * Copyright (C) 2018-2024 Heinrich-Heine-Universitaet Duesseldorf,
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

#ifndef HHUOS_KEY_H
#define HHUOS_KEY_H

#include <stdint.h>

namespace Util::Io {

/**
 * Represents a key, consisting of a key- and an Ascii-code.
 *
 * @author  original by Olaf Spinczyk, TU Dortmund
 * 			modified by Michael Schoettner, Filip Krakowski, Fabian Ruhland, Burak Akguel, Christian Gesse
 */
class Key {

friend class KeyDecoder;
friend class KeyboardLayout;

public:
    /**
     * Bit-masks for key-modifiers.
     */
    enum Modifier {
        SHIFT = 1,
        ALT_LEFT = 2,
        ALT_RIGHT = 4,
        CTRL_LEFT = 8,
        CTRL_RIGHT = 16,
        CAPS_LOCK = 32,
        NUM_LOCK = 64,
        SCROLL_LOCK = 128
    };

    /**
     * Some pre-defined scancodes.
     */
    enum Scancode {
        ESC = 0x01,
        DIV = 0x08,
        Q = 0x10,
        W = 0x11,
        A = 0x1e,
        S = 0x1f,
        D = 0x20,
        J = 0x24,
        K = 0x25,
        E = 0x12,
        R = 0x13,
        F = 0x21,
        Y = 0x2c,
        X = 0x2d,
        C = 0x2e,
        V = 0x2f,
        PLUS = 0x1b,
        MINUS = 0x35,
        ENTER = 0x1c,
        SPACE = 0x39,
        UP = 0x48,
        DOWN = 0x50,
        LEFT = 0x4b,
        RIGHT = 0x4d,
        HOME = 0x47,
        END = 0x4f,
        PAGE_UP = 0x49,
        PAGE_DOWN = 0x51,
        INSERT = 0x52,
        DEL = 0x53,
        TAB = 0x0f,
        BACKSPACE = 0x0e,
        F1 = 0x3b,
        F2 = 0x3c,
        F3 = 0x3d,
        F4 = 0x3e,
        F5 = 0x3f,
        F6 = 0x40,
        F7 = 0x41,
        F8 = 0x42,
        F9 = 0x43,
        F10 = 0x44,
        F11 = 0x57,
        F12 = 0x58,
        NUM_1 = 0x4f,
        NUM_2 = 0x50,
        NUM_3 = 0x51,
        NUM_4 = 0x4b,
        NUM_5 = 0x4c,
        NUM_6 = 0x4d,
        NUM_7 = 0x47,
        NUM_8 = 0x48,
        NUM_9 = 0x49,
        NUM_0 = 0x52,
        NUM_PLUS = 0x4e,
        NUM_MINUS = 0x4a,
        NUM_DIV = 0x35,
        NUM_MUL = 0x37,
        NUM_COMMA = 0x53
    };

    /**
     * Constructor.
     */
    Key() = default;

    /**
     * Copy Constructor.
     */
    Key(const Key &copy) = default;

    /**
     * Assignment operator.
     */
    Key &operator=(const Key &other) = default;

    /**
     * Destructor.
     */
    ~Key() = default;

    /**
     * Check, if a key is valid(setScancode == 0 -> invalid).
     */
    [[nodiscard]] bool isValid() const;

    /**
     * Get the setScancode.
     */
    [[nodiscard]] uint8_t getScancode() const;

    /**
     * Get the setAscii-code.
     */
    [[nodiscard]] char getAscii() const;

    /**
     * Check, if a key is being pressed.
     */
    [[nodiscard]] bool isPressed() const;

    /**
     * Get the shift-modifier.
     */
    [[nodiscard]] bool getShift() const;

    /**
     * Get the alt-modifier for the left getAlt-key.
     */
    [[nodiscard]] bool getAltLeft() const;

    /**
     * Get the alt-modifier for the right getAlt-key.
     */
    [[nodiscard]] bool getAltRight() const;

    /**
     * Get the getCtrl-modifier for the left getAlt-key.
     */
    [[nodiscard]] bool getCtrlLeft() const;

    /**
     * Get the getCtrl-modifier for the right getAlt-key.
     */
    [[nodiscard]] bool getCtrlRight() const;

    /**
     * Get the caps-lock modifier.
     */
    [[nodiscard]] bool getCapsLock() const;

    /**
     * Get the num-lock modifier.
     */
    [[nodiscard]] bool getNumLock() const;

    /**
     * Get the scroll-lock modifier.
     */
    [[nodiscard]] bool getScrollLock() const;

    /**
     * Check, if one of the getAlt-keys is pressed.
     */
    [[nodiscard]] bool getAlt() const;

    /**
     * Check, if one of the getCtrl-keys is pressed.
     */
    [[nodiscard]] bool getCtrl() const;

    /**
     * (char)-operator.
     * Return the ascii-code.
     */
    explicit operator char() const;

    /**
     * (uint8_t)-operator.
     * Return the ascii- code.
     */
    explicit operator uint8_t() const;

    bool operator!=(const Key &other) const;

private:
    /**
     * Set the pressed-value.
     */
    void setPressed(bool value);

    /**
     * Invalidate this key(Set setScancode to 0).
     */
    void invalidate();

    /**
     * Set the setAscii-code.
     */
    void setAscii(uint8_t asc);

    /**
     * Set the setScancode.
     */
    void setScancode(uint8_t scan);

    /**
     * Set the setShift-modifier.
     */
    void setShift(bool pressed);

    /**
     * Set the alt-modifier for the left getAlt-key.
     */
    void setAltLeft(bool pressed);

    /**
     * Set the alt-modifier for the right getAlt-key.
     */
    void setAltRight(bool pressed);

    /**
     * Set the getCtrl-modifier for the left getAlt-key.
     */
    void setCtrlLeft(bool pressed);

    /**
     * Set the getCtrl-modifier for the right getAlt-key.
     */
    void setCtrlRight(bool pressed);

    /**
     * Set the caps-lock modifier.
     */
    void setCapsLock(bool pressed);

    /**
     * Set the num-lock modifier.
     */
    void setNumLock(bool pressed);

    /**
     * Set the scroll-lock modifier.
     */
    void setScrollLock(bool pressed);

    uint8_t ascii;
    uint8_t scancode;
    uint8_t modifier;
    bool pressed;
};

}

#endif
