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
 */

#include "AlarmRunnable.h"

#include <sound/PcSpeaker.h>

#include "kernel/service/Service.h"
#include "kernel/service/SoundService.h"
#include "device/sound/speaker/PcSpeaker.h"
#include "lib/util/async/Thread.h"
#include "lib/util/time/Timestamp.h"

namespace Device {

AlarmRunnable::AlarmRunnable(uint32_t beepCount) : beepCount(beepCount) {}

void AlarmRunnable::run() {
    auto &speaker = Kernel::Service::getService<Kernel::SoundService>().getSpeaker();

    for (uint32_t i = 0; i < beepCount; i++) {
        speaker.play(Util::Sound::PcSpeaker::A1);
        Util::Async::Thread::sleep(Util::Time::Timestamp(0, INTERVAL * 1000000));
        speaker.off();
        Util::Async::Thread::sleep(Util::Time::Timestamp(0, INTERVAL * 1000000));
    }
}

}