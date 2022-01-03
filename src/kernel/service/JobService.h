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

#ifndef HHUOS_JOBSERVICE_H
#define HHUOS_JOBSERVICE_H

#include <kernel/job/JobExecutor.h>
#include "Service.h"

namespace Kernel {

class JobService : public Service {

public:
    /**
     * Constructor.
     */
    JobService(JobExecutor &lowPriorityExecutor, JobExecutor &highPriorityExecutor);

    /**
     * Copy constructor.
     */
    JobService(const JobService &other) = delete;

    /**
     * Assignment operator.
     */
    JobService &operator=(const JobService &other) = delete;

    /**
     * Destructor.
     */
    ~JobService() override = default;

    Job::Id registerJob(Util::Async::Runnable &runnable, Job::Priority priority, Util::Time::Timestamp interval);

    Job::Id registerJob(Util::Async::Runnable &runnable, Job::Priority priority, Util::Time::Timestamp interval, int32_t repetitions);

    static const constexpr uint8_t SERVICE_ID = 1;

private:

    JobExecutor &lowPriorityExecutor;
    JobExecutor &highPriorityExecutor;
};

}

#endif