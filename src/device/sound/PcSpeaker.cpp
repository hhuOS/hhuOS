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

#include "kernel/core/System.h"
#include "PcSpeaker.h"


PcSpeaker::PcSpeaker() : control(0x43), data0(0x40), data2(0x42), ppi(0x61) {
    timeService = Kernel::System::getService<Kernel::TimeService>();
}

void PcSpeaker::play(float f, uint32_t len) {

    if (f == 0.0F) {

        off();

        delay(len);

        return;
    }

    play(f);

    delay(len);

    off ();
}

void PcSpeaker::play (float f) {
    auto  freq = static_cast<uint32_t>(f);
    auto cntStart = static_cast<uint16_t>(1193180 / freq);
    uint8_t status;
    
    // Config counter
    control.outb(0xB6);
    data2.outb(static_cast<uint8_t>(cntStart % 256));
    data2.outb(static_cast<uint8_t>(cntStart / 256));

    // Turn pcSpeaker on
    status = ppi.inb();
    ppi.outb (static_cast<uint8_t>(status | 3));
}

void PcSpeaker::off () {
    uint8_t status;

    status = (uint8_t)ppi.inb ();
    ppi.outb (static_cast<uint8_t>((status >> 2) << 2));
}

/*****************************************************************************
 * Methode:         PCSPK::delay                                             *
 *---------------------------------------------------------------------------*
 * Beschreibung:    Verzoegerung um X ms (in 10ms Schritten; Min. 1ms).      *
 *                                                                           *
 * Parameter:       time (delay in ms)                                       *
 *****************************************************************************/
inline void PcSpeaker::delay(uint32_t time) {
    timeService->msleep(time);
}