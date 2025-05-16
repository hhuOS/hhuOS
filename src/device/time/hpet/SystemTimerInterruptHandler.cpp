/*
 * Copyright (C) 2017-2025 Heinrich Heine University Düsseldorf,
 * Institute of Computer Science, Department Operating Systems
 * Main developers: Christian Gesse <christian.gesse@hhu.de>, Fabian Ruhland <ruhland@hhu.de>
 * Original development team: Burak Akguel, Christian Gesse, Fabian Ruhland, Filip Krakowski, Michael Schöttner
 * This project has been supported by several students.
 * A full list of integrated student theses can be found here: https://github.com/hhuOS/hhuOS/wiki/Student-theses
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
 * The HPET driver is based on a bachelor's thesis, written by Suratsch Hassan.
 * The original source code can be found here: https://git.hhu.de/bsinfo/thesis/ba-suhas102
 */

#include "SystemTimerInterruptHandler.h"

#include "Hpet.h"
#include "lib/util/async/Atomic.h"
#include "device/time/hpet/Timer.h"

namespace Device {

SystemTimerInterruptHandler::SystemTimerInterruptHandler(Hpet &hpet, Timer &timer) : hpet(hpet), timer(timer) {
    ticksPerInterrupt = (1000000000000000 / hpet.getFemtosecondsPerTick()) * SECONDS_PER_INTERRUPT;
}

void SystemTimerInterruptHandler::run() {
    auto counterValue = hpet.readCounter();
    if (lastCounterValue + ticksPerInterrupt > counterValue && lastCounterValue + ticksPerInterrupt > lastCounterValue) {
        // Less than a second has passed since the last interrupt (for some reason, this happens right after enabling the timer in QEMU)
        timer.arm(lastCounterValue + ticksPerInterrupt, *this);
        return;
    }

    pendingInterrupts++;

    if (readerCount == 0) {
        lastCounterValue = hpet.readCounter();
        interruptCount += pendingInterrupts;
        pendingInterrupts = 0;
    }

    timer.arm(lastCounterValue + ticksPerInterrupt, *this);
}

void SystemTimerInterruptHandler::armTimer() {
    lastCounterValue = hpet.readCounter();
    timer.arm(lastCounterValue + ticksPerInterrupt, *this);
}

Util::Time::Timestamp SystemTimerInterruptHandler::getTime() {
    auto readerCountAtomic = Util::Async::Atomic<uint32_t>(readerCount);

    readerCountAtomic.inc();
    auto interrupts = interruptCount;
    auto lastCounter = lastCounterValue;
    readerCountAtomic.dec();

    auto currentCounter = hpet.readCounter();
    auto ticksSinceLastInterrupt = currentCounter >= lastCounter ? currentCounter - lastCounter : hpet.getMaxValue() - lastCounter + currentCounter;

    return Util::Time::Timestamp::ofNanoseconds(((interrupts * ticksPerInterrupt + ticksSinceLastInterrupt) * hpet.getFemtosecondsPerTick()) / 1000000);
}

}