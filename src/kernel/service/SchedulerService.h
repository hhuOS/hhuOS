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

#ifndef HHUOS_SCHEDULERSERVICE_H
#define HHUOS_SCHEDULERSERVICE_H

#include <cstdint>

#include "kernel/process/Scheduler.h"
#include "Service.h"

namespace Device {
class Fpu;
}  // namespace Device
namespace Util {
namespace Time {
class Timestamp;
}  // namespace Time
}  // namespace Util

namespace Kernel {
class Logger;
class Process;
class SchedulerCleaner;
class Thread;

class SchedulerService : public Service {

public:
    /**
     * Default Constructor.
     */
    SchedulerService();

    /**
     * Copy Constructor.
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

    void ready(Thread &thread);

    void yield();

    void cleanup(Thread *thread);

    void cleanup(Process *process);

    void lockScheduler();

    void unlockScheduler();

    void block();

    void unblock(Thread &thread);

    void sleep(const Util::Time::Timestamp &time);

    void kill(Thread &thread);

    void killWithoutLock(Thread &thread);

    void exitCurrentThread();

    [[nodiscard]] Thread& getCurrentThread();

    [[nodiscard]] Thread* getThread(uint32_t id);

    [[nodiscard]] uint8_t* getDefaultFpuContext();

    static const constexpr uint8_t SERVICE_ID = 4;

private:

    Scheduler scheduler;
    SchedulerCleaner *cleaner = nullptr;
    Device::Fpu *fpu = nullptr;
    uint8_t *defaultFpuContext = nullptr;

    static Logger log;
};

}

#endif