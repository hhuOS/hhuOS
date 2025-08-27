/*
 * Copyright (C) 2017-2025 Heinrich Heine University Düsseldorf,
 * Institute of Computer Science, Department Operating Systems
 * Main developers: Christian Gesse <christian.gesse@hhu.de>, Fabian Ruhland <ruhland@hhu.de>
 * Original development team: Burak Akguel, Christian Gesse, Fabian Ruhland, Filip Krakowski, Michael Schöttner
 * This project has been supported by several students.
 * A full list of integrated student theses can be found here: https://github.com/hhuOS/hhuOS/wiki/Student-theses; Olaf Spinczyk, TU Dortmund
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
 *
 * The key decoder is based on code taken from the "Operating Systems" lecture at TU Dortmund.
 * The original code has been implemented by Olaf Spinczyk and its documentation is available here:
 * https://sys-sideshow.cs.tu-dortmund.de/lehre/ws24/bsb/aufgaben/1/classKeyDecoder.html
 */

#ifndef HHUOS_LIB_UTIL_IO_KEY_H
#define HHUOS_LIB_UTIL_IO_KEY_H

#include <stdint.h>

namespace Util::Io {

/// Represents a keyboard key, including its scancode, ASCII code, modifier states, and pressed state.
/// Instances of this class are created and managed by the `KeyDecoder`.
/// Once a key has been decoded, it is immutable.
class Key {

friend class KeyDecoder;
friend class KeyboardLayout;

public:
    /// Key modifiers as bit flags.
    enum Modifier : uint8_t {
        /// Modifier for the Shift key.
        SHIFT = 0x01,
        /// Modifier for the left Alt key.
        ALT_LEFT = 0x02,
        /// Modifier for the right Alt key.
        ALT_RIGHT = 0x04,
        /// Modifier for the left Ctrl key.
        CTRL_LEFT = 0x08,
        /// Modifier for the right Ctrl key.
        CTRL_RIGHT = 0x10,
        /// Modifier for the Caps Lock key.
        CAPS_LOCK = 0x20,
        /// Modifier for the Num Lock key.
        NUM_LOCK = 0x40,
        /// Modifier for the Scroll Lock key.
        SCROLL_LOCK = 0x80
    };

    /// Scancodes for various keys on the keyboard.
    enum Scancode : uint8_t {
        /// Scancode for the Escape key.
        ESC = 0x01,
        /// Scancode for the '1' key.
        ONE = 0x02,
        /// Scancode for the '2' key.
        TWO = 0x03,
        /// Scancode for the '3' key.
        THREE = 0x04,
        /// Scancode for the '4' key.
        FOUR = 0x05,
        /// Scancode for the '5' key.
        FIVE = 0x06,
        /// Scancode for the '6' key.
        SIX = 0x07,
        /// Scancode for the '7' key.
        SEVEN = 0x08,
        /// Scancode for the '8' key.
        EIGHT = 0x09,
        /// Scancode for the '9' key.
        NINE = 0x0a,
        /// Scancode for the '0' key.
        ZERO = 0x0b,
        /// Scancode for the 'Q' key.
        Q = 0x10,
        /// Scancode for the 'W' key.
        W = 0x11,
        /// Scancode for the 'E' key.
        E = 0x12,
        /// Scancode for the 'R' key.
        R = 0x13,
        /// Scancode for the 'T' key.
        T = 0x14,
        /// Scancode for the 'Z' key.
        Z = 0x15,
        /// Scancode for the 'U' key.
        U = 0x16,
        /// Scancode for the 'I' key.
        I = 0x17,
        /// Scancode for the 'O' key.
        O = 0x18,
        /// Scancode for the 'P' key.
        P = 0x19,
        /// Scancode for the 'A' key.
        A = 0x1e,
        /// Scancode for the 'S' key.
        S = 0x1f,
        /// Scancode for the 'D' key.
        D = 0x20,
        /// Scancode for the 'F' key.
        F = 0x21,
        /// Scancode for the 'G' key.
        G = 0x22,
        /// Scancode for the 'H' key.
        H = 0x23,
        /// Scancode for the 'J' key.
        J = 0x24,
        /// Scancode for the 'K' key.
        K = 0x25,
        /// Scancode for the 'L' key.
        L = 0x26,
        /// Scancode for the 'Y' key.
        Y = 0x2c,
        /// Scancode for the 'X' key.
        X = 0x2d,
        /// Scancode for the 'C' key.
        C = 0x2e,
        /// Scancode for the 'V' key.
        V = 0x2f,
        /// Scancode for the 'B' key.
        B = 0x30,
        /// Scancode for the 'N' key.
        N = 0x31,
        /// Scancode for the 'M' key.
        M = 0x32,
        /// Scancode for the '+' key.
        PLUS = 0x1b,
        /// Scancode for the '-' key.
        MINUS = 0x35,
        /// Scancode for the Enter key.
        ENTER = 0x1c,
        /// Scancode for the Space key.
        SPACE = 0x39,
        /// Scancode for the Up Arrow key.
        UP = 0x48,
        /// Scancode for the Down Arrow key.
        DOWN = 0x50,
        /// Scancode for the Left Arrow key.
        LEFT = 0x4b,
        /// Scancode for the Right Arrow key.
        RIGHT = 0x4d,
        /// Scancode for the Home key.
        HOME = 0x47,
        /// Scancode for the End key.
        END = 0x4f,
        /// Scancode for the Page Up key.
        PAGE_UP = 0x49,
        /// Scancode for the Page Down key.
        PAGE_DOWN = 0x51,
        /// Scancode for the Insert key.
        INSERT = 0x52,
        /// Scancode for the Delete key.
        DEL = 0x53,
        /// Scancode for the Tab key.
        TAB = 0x0f,
        /// Scancode for the Backspace key.
        BACKSPACE = 0x0e,
        /// Scancode for the F1 key.
        F1 = 0x3b,
        /// Scancode for the F2 key.
        F2 = 0x3c,
        /// Scancode for the F3 key.
        F3 = 0x3d,
        /// Scancode for the F4 key.
        F4 = 0x3e,
        /// Scancode for the F5 key.
        F5 = 0x3f,
        /// Scancode for the F6 key.
        F6 = 0x40,
        /// Scancode for the F7 key.
        F7 = 0x41,
        /// Scancode for the F8 key.
        F8 = 0x42,
        /// Scancode for the F9 key.
        F9 = 0x43,
        /// Scancode for the F10 key.
        F10 = 0x44,
        /// Scancode for the F11 key.
        F11 = 0x57,
        /// Scancode for the F12 key.
        F12 = 0x58,
        /// Scancode for the '1' key on the numeric keypad.
        NUM_1 = 0x4f,
        /// Scancode for the '2' key on the numeric keypad.
        NUM_2 = 0x50,
        /// Scancode for the '3' key on the numeric keypad.
        NUM_3 = 0x51,
        /// Scancode for the '4' key on the numeric keypad.
        NUM_4 = 0x4b,
        /// Scancode for the '5' key on the numeric keypad.
        NUM_5 = 0x4c,
        /// Scancode for the '6' key on the numeric keypad.
        NUM_6 = 0x4d,
        /// Scancode for the '7' key on the numeric keypad.
        NUM_7 = 0x47,
        /// Scancode for the '8' key on the numeric keypad.
        NUM_8 = 0x48,
        /// Scancode for the '9' key on the numeric keypad.
        NUM_9 = 0x49,
        /// Scancode for the '0' key on the numeric keypad.
        NUM_0 = 0x52,
        /// Scancode for the '+' key on the numeric keypad.
        NUM_PLUS = 0x4e,
        /// Scancode for the '-' key on the numeric keypad.
        NUM_MINUS = 0x4a,
        /// Scancode for the '/' key on the numeric keypad.
        NUM_DIV = 0x35,
        /// Scancode for the '*' key on the numeric keypad.
        NUM_MUL = 0x37,
        /// Scancode for the ',' key on the numeric keypad.
        NUM_COMMA = 0x53
    };

    /// Create a new invalid key (scancode = 0).
    /// The key remains invalid until a valid scancode is set by the `KeyDecoder`.
    Key() = default;

    /// Check if the key is valid (scancode != 0).
    [[nodiscard]] bool isValid() const;

    /// Get the scancode.
    [[nodiscard]] uint8_t getScancode() const;

    /// Get the ASCII code corresponding to the key,
    /// taking into account the current keyboard layout and modifier states.
    /// If no ASCII code is associated with the key, 0 is returned.
    [[nodiscard]] char getAscii() const;

    /// Check if the key is currently pressed (true) or released (false).
    [[nodiscard]] bool isPressed() const;

    /// Check if the Shift key is pressed.
    [[nodiscard]] bool getShift() const;

    /// Check if the left Alk key is pressed.
    [[nodiscard]] bool getAltLeft() const;

    /// Check if the right Alk key is pressed.
    [[nodiscard]] bool getAltRight() const;

    /// Check if either of the Alt keys is pressed.
    [[nodiscard]] bool getAlt() const;

    /// Check if the left Ctrl key is pressed.
    [[nodiscard]] bool getCtrlLeft() const;

    /// Check if the right Ctrl key is pressed.
    [[nodiscard]] bool getCtrlRight() const;

    /// Check if either of the Ctrl keys is pressed.
    [[nodiscard]] bool getCtrl() const;

    /// Check if caps lock is active.
    [[nodiscard]] bool getCapsLock() const;

    /// Check if num lock is active.
    [[nodiscard]] bool getNumLock() const;

    /// Check if scroll lock is active.
    [[nodiscard]] bool getScrollLock() const;

    /// Compare this key with another key for inequality.
    bool operator!=(const Key &other) const;

private:

    void setPressed(bool pressed);

    void setAscii(uint8_t ascii);

    void setScancode(uint8_t scancode);

    void setShift(bool pressed);

    void setAltLeft(bool pressed);

    void setAltRight(bool pressed);

    void setCtrlLeft(bool pressed);

    void setCtrlRight(bool pressed);

    void setCapsLock(bool pressed);

    void setNumLock(bool pressed);

    void setScrollLock(bool pressed);

    uint8_t ascii = 0;
    uint8_t scancode = 0;
    uint8_t modifier = 0;
    bool pressed = false;
};

}

#endif
