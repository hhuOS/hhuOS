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
 */

#include "Key.h"

namespace Util::Io {

bool Key::isValid() const {
    return scancode != 0;
}

bool Key::isPressed() const {
    return pressed;
}

void Key::setPressed(const bool pressed) {
    Key::pressed = pressed;
}

void Key::setAscii(const uint8_t ascii) {
    Key::ascii = ascii;
}

void Key::setScancode(const uint8_t scancode)  {
    Key::scancode = scancode;
}

char Key::getAscii() const {
    return ascii;
}

uint8_t Key::getScancode() const {
    return scancode;
}

void Key::setShift(const bool pressed) {
    modifier = pressed ? modifier | SHIFT : modifier & ~SHIFT;
}

void Key::setAltLeft(const bool pressed) {
    modifier = pressed ? modifier | ALT_LEFT : modifier & ~ALT_LEFT;
}

void Key::setAltRight(const bool pressed) {
    modifier = pressed ? modifier | ALT_RIGHT : modifier & ~ALT_RIGHT;
}

void Key::setCtrlLeft(const bool pressed) {
    modifier = pressed ? modifier | CTRL_LEFT : modifier & ~CTRL_LEFT;
}

void Key::setCtrlRight(const bool pressed) {
    modifier = pressed ? modifier | CTRL_RIGHT : modifier & ~CTRL_RIGHT;
}

void Key::setCapsLock(const bool pressed) {
    modifier = pressed ? modifier | CAPS_LOCK : modifier & ~CAPS_LOCK;
}

void Key::setNumLock(const bool pressed) {
    modifier = pressed ? modifier | NUM_LOCK : modifier & ~NUM_LOCK;
}

void Key::setScrollLock(const bool pressed) {
    modifier = pressed ? modifier | SCROLL_LOCK : modifier & ~SCROLL_LOCK;
}


bool Key::getShift() const {
    return modifier & SHIFT;
}

bool Key::getAltLeft() const {
    return modifier & ALT_LEFT;
}

bool Key::getAltRight() const {
    return modifier & ALT_RIGHT;
}

bool Key::getCtrlLeft() const {
    return modifier & CTRL_LEFT;
}

bool Key::getCtrlRight() const {
    return modifier & CTRL_RIGHT;
}

bool Key::getCapsLock() const {
    return modifier & CAPS_LOCK;
}

bool Key::getNumLock() const {
    return modifier & NUM_LOCK;
}

bool Key::getScrollLock() const {
    return modifier & SCROLL_LOCK;
}

bool Key::getAlt() const {
    return getAltLeft() || getAltRight();
}


bool Key::getCtrl() const {
    return getCtrlLeft() || getCtrlRight();
}

bool Key::operator!=(const Key &other) const {
    return scancode != other.scancode || modifier != other.modifier || pressed != other.pressed;
}

}