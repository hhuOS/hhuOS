/*
 * Copyright (C) 2018-2021 Heinrich-Heine-Universitaet Duesseldorf,
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

#include <kernel/interrupt/InterruptDispatcher.h>
#include <device/interrupt/Pic.h>
#include "Pit.h"

namespace Device {

Pit::Pit(uint32_t timerInterval) : timerInterval(timerInterval) {}

Pit& Pit::getInstance() {
    static Pit instance;
    return instance;
}

void Pit::setInterval(uint32_t ns) {
    timerInterval = ns;
    uint32_t divisor = ns / TIME_BASE;

    controlPort.writeByte(0x36); // Select channel 0, Use low-/high byte access mode, Set operating mode to rate generator
    dataPort0.writeByte((uint8_t) (divisor & 0xff)); // Low byte
    dataPort0.writeByte((uint8_t) (divisor >> 8)); // High byte
}

void Pit::plugin () {
    setInterval(timerInterval);
    Kernel::InterruptDispatcher::getInstance().assign(32, *this);
    Pic::getInstance().allow(Pic::Interrupt::PIT);
}

void Pit::trigger(Kernel::InterruptFrame &frame) {
    time.addNanos(timerInterval);
    advanceTime(timerInterval);
}

uint32_t Pit::getNanos() {
    return time.toNanos();
}

uint32_t Pit::getMicros() {
    return time.toMicros();
}

uint32_t Pit::getMillis() {
    return time.toMillis();
}

uint32_t Pit::getSeconds() {
    return time.seconds;
}

uint32_t Pit::getMinutes() {
    return time.toMinutes();
}

uint32_t Pit::getHours() {
    return time.toHours();
}

uint32_t Pit::getDays() {
    return time.toDays();
}

uint32_t Pit::getYears() {
    return time.toYears();
}

}