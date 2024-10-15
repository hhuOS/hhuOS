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

#include <stdint.h>

#include "lib/util/collection/ArrayListBlockingQueue.h"
#include "lib/util/async/Spinlock.h"
#include "lib/util/collection/ArrayList.h"
#include "lib/util/collection/HashMap.h"
#include "lib/util/time/Timestamp.h"
#include "kernel/service/InterruptService.h"
#include "kernel/service/Service.h"

namespace Device {
class Fpu;
}  // namespace Device

namespace Kernel {
class Thread;
enum InterruptVector : uint8_t;

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
    void setInitialized();

    bool isInitialized() const;

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

    void yield(bool interrupt = false);

    void switchFpuContext();

    /**
     * Kills a specific Thread.
     *
     * @param thread A Thread
     */
    void kill(Thread &thread);

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

    Thread* getLastFpuThread() const;

    Thread* getThread(uint32_t id);

    [[nodiscard]] uint32_t getThreadCount() const;

    uint8_t* getDefaultFpuContext();

    void unlockReadyQueue();

    void removeFromJoinMap(uint32_t threadId);

private:

    void lockReadyQueue();

    void checkSleepList();

    void resetLastFpuThread(Thread &terminatedThread);

    struct SleepEntry {
        Thread *thread;
        Util::Time::Timestamp wakeupTime;

        bool operator!=(const SleepEntry &other) const;
    };

    bool initialized = false;
    Thread *currentThread = nullptr;

    Device::Fpu *fpu = nullptr;
    uint8_t *defaultFpuContext = nullptr;
    uint32_t lastFpuThread = 0; // Actually a pointer, but needs to be a uint32_t for atomic operations

    InterruptVector timerInterrupt = Service::getService<InterruptService>().getTimerInterrupt();

    Util::ArrayListBlockingQueue<Thread*> readyQueue;
    Util::Async::Spinlock readyQueueLock;

    Util::ArrayList<SleepEntry> sleepList;
    Util::Async::Spinlock sleepQueueLock;

    Util::HashMap<uint32_t, Util::ArrayList<Thread*>*> joinMap;
    Util::Async::Spinlock joinLock;
};

}

#endif