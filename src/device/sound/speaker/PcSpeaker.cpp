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

#include "device/time/Pit.h"
#include "PcSpeaker.h"
#include "device/cpu/IoPort.h"

namespace Device::Sound {

IoPort PcSpeaker::controlPort(0x43);
IoPort PcSpeaker::dataPort2(0x42);
IoPort PcSpeaker::ppi(0x61);

void PcSpeaker::play(uint32_t frequency) {
    if (frequency == 0) {
        off();
        return;
    }

    // Config counter
    auto counter = static_cast<uint16_t>(Pit::BASE_FREQUENCY / frequency);
    controlPort.writeByte(0xb6);
    dataPort2.writeByte(counter % 256);
    dataPort2.writeByte(counter / 256);

    // Turn speaker on
    auto status = ppi.readByte();
    ppi.writeByte(status | 3);
}

void PcSpeaker::off() {
    uint8_t status = ppi.readByte();
    ppi.writeByte(status & 0xfc);
}

}