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

#ifndef HHUOS_SCHEDULERCLEANER_H
#define HHUOS_SCHEDULERCLEANER_H

#include "lib/util/data/ArrayBlockingQueue.h"
#include "lib/util/async/Runnable.h"
#include "lib/util/data/Array.h"
#include "lib/util/data/Collection.h"
#include "lib/util/data/Iterator.h"

namespace Kernel {
class Process;
class Thread;

class SchedulerCleaner : public Util::Async::Runnable {

public:
    /**
     * Default Constructor.
     */
    SchedulerCleaner();

    /**
     * Copy Constructor.
     */
    SchedulerCleaner(const SchedulerCleaner &other) = delete;

    /**
     * Assignment operator.
     */
    SchedulerCleaner &operator=(const SchedulerCleaner &other) = delete;

    /**
     * Destructor.
     */
    ~SchedulerCleaner() override;

    void cleanup(Process *process);

    void cleanup(Thread *thread);

    void run() override;

private:

    void cleanupProcesses();

    void cleanupThreads();

    Util::Data::ArrayBlockingQueue<Process*> processQueue;
    Util::Data::ArrayBlockingQueue<Thread*> threadQueue;
};

}

#endif