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

#ifndef HHUOS_SYSTEMTIMERINTERRUPTHANDLER_H
#define HHUOS_SYSTEMTIMERINTERRUPTHANDLER_H

#include <stdint.h>

#include "lib/util/async/Runnable.h"
#include "lib/util/time/Timestamp.h"

namespace Device {
class Hpet;
class Timer;

class SystemTimerInterruptHandler : public Util::Async::Runnable {

public:
    /**
     * Constructor.
     */
    explicit SystemTimerInterruptHandler(Hpet &hpet, Timer &timer);

    /**
     * Copy Constructor.
     */
    SystemTimerInterruptHandler(const SystemTimerInterruptHandler &other) = delete;

    /**
     * Assignment operator.
     */
    SystemTimerInterruptHandler &operator=(const SystemTimerInterruptHandler &other) = delete;

    /**
     * Destructor.
     */
    ~SystemTimerInterruptHandler() override = default;

    void run() override;

    void armTimer();

    [[nodiscard]] Util::Time::Timestamp getTime();

private:

    Hpet &hpet;
    Timer &timer;
    uint64_t ticksPerInterrupt = 0;
    uint32_t interruptCount = 0;
    uint64_t lastCounterValue = 0;

    uint32_t pendingInterrupts = 0;
    uint32_t readerCount = 0;

    static const constexpr uint32_t SECONDS_PER_INTERRUPT = 10;
};

}

#endif
