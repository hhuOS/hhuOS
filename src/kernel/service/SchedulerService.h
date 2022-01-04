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

#ifndef HHUOS_SCHEDULERSERVICE_H
#define HHUOS_SCHEDULERSERVICE_H

#include "kernel/process/ProcessScheduler.h"

namespace Kernel {

class SchedulerService : public Service {

public:
    /**
     * Default Constructor.
     */
    SchedulerService() = default;

    /**
     * Copy constructor.
     */
    SchedulerService(const SchedulerService &other) = delete;

    /**
     * Assignment operator.
     */
    SchedulerService &operator=(const SchedulerService &other) = delete;

    /**
     * Destructor.
     */
    ~SchedulerService() override = default;

    void kickoffThread();

    void startScheduler();

    void ready(Process &process);

    void ready(Thread &thread);

    void yield();

    Process* createProcess(VirtualAddressSpace &addressSpace);

    void releaseSchedulerLock();

    void setSchedulerInitialized();

    [[nodiscard]] bool isSchedulerInitialized() const;

    [[nodiscard]] Process& getCurrentProcess();

    [[nodiscard]] Thread& getCurrentThread();

    static const constexpr uint8_t SERVICE_ID = 3;

private:

    ProcessScheduler scheduler;
};

}

#endif