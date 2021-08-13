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

#include "JobExecutor.h"


void Kernel::JobExecutor::advanceTime(uint32_t elapsedTime) {
    for (uint32_t i = 0; i < jobs.size(); i++) {
        auto job = jobs.get(i);
        job.advanceTime(elapsedTime);
        jobs.set(i, job);
    }

    for (uint32_t i = 0; i < jobs.size(); i++) {
        if (jobs.get(i).isFinished()) {
            jobs.remove(i);
            i = 0;
        }
    }
}

uint32_t Kernel::JobExecutor::registerJob(Util::Async::Runnable &runnable, uint32_t interval) {
    return registerJob(runnable, interval, -1);
}

uint32_t Kernel::JobExecutor::registerJob(Util::Async::Runnable &runnable, uint32_t interval, int32_t repetitions) {
    auto job = Job(runnable, interval, repetitions);
    jobs.add(job);
    return job.getId();
}

void Kernel::JobExecutor::deleteJob(uint32_t id) {
    for (uint32_t i = 0; i < jobs.size(); i++) {
        if (jobs.get(i).getId() == id) {
            jobs.remove(i);
            return;
        }
    }
}
