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

#include "JobExecutor.h"

namespace Kernel {

void JobExecutor::advanceTime(Util::Time::Timestamp elapsedTime) {
    for (uint32_t i = 0; i < jobs.size(); i++) {
        jobs.get(i)->advanceTime(elapsedTime);
    }
}

void JobExecutor::executePendingJobs() {
    if (!executionLock.tryAcquire()) {
        return;
    }

    for (uint32_t i = 0; i < jobs.size(); i++) {
        auto *job = jobs.get(i);
        job->executeIfPending();
    }

    for (uint32_t i = 0; i < jobs.size(); i++) {
        auto *job = jobs.get(i);
        if (jobs.get(i)->isFinished()) {
            jobs.removeIndex(i);
            delete job;
            i = 0;
        }
    }

    executionLock.release();
}

Job::Id JobExecutor::registerJob(Util::Async::Runnable *runnable, Util::Time::Timestamp interval) {
    return registerJob(runnable, interval, -1);
}

Job::Id JobExecutor::registerJob(Util::Async::Runnable *runnable, Util::Time::Timestamp interval, int32_t repetitions) {
    auto *job = new Job(runnable, interval, repetitions);
    jobs.add(job);
    return job->getId();
}

void JobExecutor::deleteJob(Job::Id id) {
    for (uint32_t i = 0; i < jobs.size(); i++) {
        auto *job = jobs.get(i);
        if (jobs.get(i)->getId() == id) {
            jobs.removeIndex(i);
            delete job;
            return;
        }
    }
}

}