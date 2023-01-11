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

#include "Key.h"

namespace Util::Io {

bool Key::isValid() const {
    return scancode != 0;
}


bool Key::isPressed() const {
    return pressed;
}


void Key::setPressed(bool value) {
    pressed = value;
}


void Key::invalidate() {
    scancode = 0;
}


void Key::setAscii(uint8_t a) {
    ascii = a;
}


void Key::setScancode(uint8_t s)  {
    scancode = s;
}


uint8_t Key::getAscii() const {
    return ascii;
}


uint8_t Key::getScancode() const {
    return scancode;
}


void Key::setShift(bool pressed) {
    modifier = pressed ? modifier | Modifier::SHIFT : modifier & ~Modifier::SHIFT;
}


void Key::setAltLeft(bool pressed) {
    modifier = pressed ? modifier | Modifier::ALT_LEFT : modifier & ~Modifier::ALT_LEFT;
}


void Key::setAltRight(bool pressed) {
    modifier = pressed ? modifier | Modifier::ALT_RIGHT : modifier & ~Modifier::ALT_RIGHT;
}


void Key::setCtrlLeft(bool pressed) {
    modifier = pressed ? modifier | Modifier::CTRL_LEFT : modifier & ~Modifier::CTRL_LEFT;
}


void Key::setCtrlRight(bool pressed) {
    modifier = pressed ? modifier | Modifier::CTRL_RIGHT : modifier & ~Modifier::CTRL_RIGHT;
}


void Key::setCapsLock(bool pressed) {
    modifier = pressed ? modifier | Modifier::CAPS_LOCK : modifier & ~Modifier::CAPS_LOCK;
}


void Key::setNumLock(bool pressed) {
    modifier = pressed ? modifier | Modifier::NUM_LOCK : modifier & ~Modifier::NUM_LOCK;
}


void Key::setScrollLock(bool pressed) {
    modifier = pressed ? modifier | Modifier::SCROLL_LOCK : modifier & ~Modifier::SCROLL_LOCK;
}



bool Key::getShift() const {
    return modifier & Modifier::SHIFT;
}


bool Key::getAltLeft() const {
    return modifier & Modifier::ALT_LEFT;
}


bool Key::getAltRight() const {
    return modifier & Modifier::ALT_RIGHT;
}


bool Key::getCtrlLeft() const {
    return modifier & Modifier::CTRL_LEFT;
}


bool Key::getCtrlRight() const {
    return modifier & Modifier::CTRL_RIGHT;
}


bool Key::getCapsLock() const {
    return modifier & Modifier::CAPS_LOCK;
}


bool Key::getNumLock() const {
    return modifier & Modifier::NUM_LOCK;
}


bool Key::getScrollLock() const {
    return modifier & Modifier::SCROLL_LOCK;
}


bool Key::getAlt() const {
    return getAltLeft() || getAltRight();
}


bool Key::getCtrl() const {
    return getCtrlLeft() || getCtrlRight();
}

Key::operator char() const {
    return(char) ascii;
}

Key::operator uint8_t() const {
    return(uint8_t) ascii;
}

}