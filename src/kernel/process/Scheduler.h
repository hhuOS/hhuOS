/*
 * Copyright (C) 2018-2024 Heinrich-Heine-Universitaet Duesseldorf,
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

#ifndef HHUOS_SCHEDULER_H
#define HHUOS_SCHEDULER_H

#include <cstdint>
#include <device/cpu/Fpu.h>
#include <lib/util/collection/HashMap.h>

#include "lib/util/collection/ArrayListBlockingQueue.h"
#include "lib/util/async/Spinlock.h"
#include "lib/util/collection/ArrayList.h"
#include "kernel/process/Thread.h"

namespace Util {
namespace Time {
class Timestamp;
}  // namespace Time
}  // namespace Util

namespace Kernel {

class Scheduler {

public:
    /**
     * Constructor.
     */
    explicit Scheduler();

    /**
     * Copy Constructor.
     */
    Scheduler(const Scheduler &other) = delete;

    /**
     * Assignment operator.
     */
    Scheduler &operator=(const Scheduler &other) = delete;

    /**
     * Destructor.
     */
    ~Scheduler();

    /**
     * Set initialized to 'true'.
     */
    void setInit();

    bool isInitialized();

    /**
     * Start the first thread.
     */
    void start();

    /**
     * Registers a new Thread.
     *
     * @param thread A Thread.
     */
    void ready(Thread &thread);

    /**
     * Terminates the current Thread.
     */
    void exit();

    void yield();

    void switchFpuContext();

    /**
     * Kills a specific Thread.
     *
     * @param thread A Thread
     */
    void kill(Thread &thread);

    void killWithoutLock(Thread &thread);

    void block();

    void unblock(Thread &thread);

    void sleep(const Util::Time::Timestamp &time);

    void join(const Thread &thread);

    /**
     * Returns the activeFlag Thread.
     *
     * @return The activeFlag Thread
     */
    Thread& getCurrentThread();

    Thread* getLastFpuThread();

    Thread* getThread(uint32_t id);

    [[nodiscard]] uint32_t getThreadCount() const;

    uint8_t* getDefaultFpuContext();

    void unlockReadyQueue();

private:

    void checkSleepList();

    void unblockJoinList(Thread &thread);

    void resetLastFpuThread(Thread &terminatedThread);

    struct SleepEntry {
        Thread *thread;
        uint32_t wakeupTime;

        bool operator!=(const SleepEntry &other) const;
    };

    bool initialized = false;
    Thread *currentThread = nullptr;

    Device::Fpu *fpu = nullptr;
    uint8_t *defaultFpuContext = nullptr;
    Thread *lastFpuThread = nullptr;

    Util::ArrayListBlockingQueue<Thread*> readyQueue;
    Util::Async::Spinlock readyQueueLock;

    Util::ArrayList<SleepEntry> sleepList;
    Util::Async::Spinlock sleepQueueLock;

    Util::HashMap<uint32_t, Util::ArrayList<Thread*>*> joinMap;
    Util::Async::Spinlock joinLock;
};

}

#endif