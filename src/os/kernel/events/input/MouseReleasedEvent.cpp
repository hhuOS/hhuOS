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

#include "MouseReleasedEvent.h"

MouseReleasedEvent::MouseReleasedEvent() : Event() {
    bitmask = 0;
}

MouseReleasedEvent::MouseReleasedEvent(uint8_t bitmask) : Event() {
    this->bitmask = bitmask;
}

bool MouseReleasedEvent::isLeftReleased() {
    return (bitmask & 0x1u) != 0;
}

bool MouseReleasedEvent::isRightReleased() {
    return (bitmask & 0x2u) != 0;
}

bool MouseReleasedEvent::isMiddleReleased() {
    return (bitmask & 0x4u) != 0;
}

String MouseReleasedEvent::getType() const {
    return TYPE;
}
