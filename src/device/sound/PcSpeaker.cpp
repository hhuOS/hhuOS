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

#include "lib/util/time/Timestamp.h"
#include "PcSpeaker.h"
#include "kernel/system/System.h"
#include "kernel/service/TimeService.h"

namespace Device {

IoPort PcSpeaker::controlPort(0x43);
IoPort PcSpeaker::dataPort2(0x42);
IoPort PcSpeaker::ppi(0x61);

void PcSpeaker::play(float frequency, uint32_t length) {
    if (frequency == 0.0F) {
        off();
        delay(length);
        return;
    }

    play(frequency);
    delay(length);
    off();
}

void PcSpeaker::play(float frequency) {
    auto freq = static_cast<uint32_t>(frequency);
    auto cntStart = static_cast<uint16_t>(1193180 / freq);
    uint8_t status;

    // Config counter
    controlPort.writeByte(0xB6);
    dataPort2.writeByte(cntStart % 256);
    dataPort2.writeByte(cntStart / 256);

    // Turn speaker on
    status = ppi.readByte();
    ppi.writeByte(status | 3);
}

void PcSpeaker::off() {
    uint8_t status = ppi.readByte();
    ppi.writeByte(status & 0xfc);
}

void PcSpeaker::delay(uint32_t time) {
    uint32_t end = Util::Time::getSystemTime().toMilliseconds() + time;
    while (Util::Time::getSystemTime().toMilliseconds() < end) {}
}

}