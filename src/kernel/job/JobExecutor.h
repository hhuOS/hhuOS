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

#ifndef HHUOS_JOBEXECUTOR_H
#define HHUOS_JOBEXECUTOR_H

#include <lib/util/data/ArrayList.h>
#include "Job.h"

namespace Kernel {

class JobExecutor {

public:
    /**
     * Default Constructor.
     */
    JobExecutor() = default;

    /**
     * Copy constructor.
     */
    JobExecutor(const JobExecutor &other) = delete;

    /**
     * Assignment operator.
     */
    JobExecutor &operator=(const JobExecutor &other) = delete;

    /**
     * Destructor.
     */
    ~JobExecutor() = default;

    void advanceTime(uint32_t elapsedTime);

    uint32_t registerJob(Util::Async::Runnable &runnable, uint32_t interval);

    void deleteJob(uint32_t id);

private:

    Util::Data::ArrayList<Job> jobs;

};

}


#endif