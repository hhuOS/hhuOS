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

#include <device/time/Pit.h>
#include <device/time/Rtc.h>
#include "JobService.h"

namespace Kernel {

JobService::JobService(JobExecutor &lowPriorityExecutor, JobExecutor &highPriorityExecutor) : lowPriorityExecutor(lowPriorityExecutor), highPriorityExecutor(highPriorityExecutor) {}

Job::Id JobService::registerJob(Util::Async::Runnable &runnable, Job::Priority priority, Util::Time::Timestamp interval) {
    return registerJob(runnable, priority, interval, -1);
}

Job::Id JobService::registerJob(Util::Async::Runnable &runnable, Job::Priority priority, Util::Time::Timestamp interval, int32_t repetitions) {
    if (priority == Job::Priority::LOW) {
        return lowPriorityExecutor.registerJob(runnable, interval, repetitions);
    } else {
        return highPriorityExecutor.registerJob(runnable, interval, repetitions);
    }
}

}