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

#ifndef HHUOS_PROCESSSCHEDULER_H
#define HHUOS_PROCESSSCHEDULER_H

#include "Process.h"
#include "lib/util/data/HashMap.h"

namespace Kernel {

class ProcessScheduler {

    friend class SchedulerService;
    friend class ThreadScheduler;

public:
    /**
     * Default Constructor.
     */
    ProcessScheduler() = default;

    /**
     * Copy Constructor.
     */
    ProcessScheduler(const ProcessScheduler &other) = delete;

    /**
     * Assignment operator.
     */
    ProcessScheduler &operator=(const ProcessScheduler &other) = delete;

    /**
     * Destructor.
     */
    ~ProcessScheduler();

    void setInitialized();

    [[nodiscard]] uint32_t isInitialized() const;

    void start();

    void ready(Process &process);

    void exit();

    void kill(Process &process);

    void yield();

    bool isProcessWaiting();

    bool isProcessActive(uint32_t id);

    void blockCurrentThread();

    void unblockThread(Thread &thread);

    Process& getCurrentProcess();

    uint32_t getProcessCount();

private:

    Process& getNextProcess();

    void forceYield();

    void dispatch(Process &next, bool force);

    uint32_t initialized = 0;
    Util::Async::Spinlock lock;
    Process *currentProcess = nullptr;
    Util::Data::ArrayBlockingQueue<Process*> processQueue;
    Util::Data::ArrayList<uint32_t> processIds;
};

}

#endif