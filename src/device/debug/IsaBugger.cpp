/*
 * Copyright (C) 2018-2022 Heinrich-Heine-Universitaet Duesseldorf,
 * Institute of Computer Science, Department Operating Systems
 * Burak Akguel, Christian Gesse, Fabian Ruhland, Filip Krakowski, Michael Schoettner
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

#include "IsaBugger.h"

#include "device/cpu/IoPort.h"

namespace Device {

IoPort IsaBugger::indexPort = IoPort(0x7a);
IoPort IsaBugger::dataPort = IoPort(0x7b);

void IsaBugger::setLeftDisplay(uint8_t value) {
    setValue(LEFT_DISPLAY, value);
}

void IsaBugger::setRightDisplay(uint8_t value) {
    setValue(RIGHT_DISPLAY, value);
}

void IsaBugger::setRedLeds(uint8_t value) {
    setValue(RED_LEDS, value);
}

void IsaBugger::setGreenLeds(uint8_t value) {
    setValue(GREEN_LEDS, value);
}

void IsaBugger::setValue(IsaBugger::Register reg, uint8_t value) {
    indexPort.writeByte(reg);
    dataPort.writeByte(value);
}

}