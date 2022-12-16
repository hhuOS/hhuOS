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

#ifndef HHUOS_SCHEDULER_H
#define HHUOS_SCHEDULER_H

#include <cstdint>

#include "lib/util/data/ArrayListBlockingQueue.h"
#include "lib/util/async/Spinlock.h"
#include "lib/util/data/Array.h"
#include "lib/util/data/ArrayList.h"
#include "lib/util/data/Collection.h"
#include "lib/util/data/Iterator.h"

namespace Util {
namespace Time {
class Timestamp;
}  // namespace Time
}  // namespace Util

namespace Kernel {
class Thread;

class Scheduler {

    friend class SchedulerService;

public:
    /**
     * Constructor.
     */
    explicit Scheduler() = default;

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

    void yield(bool force = false);

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

    /**
     * Returns the activeFlag Thread.
     *
     * @return The activeFlag Thread
     */
    Thread& getCurrentThread();

    Thread& getNextThread();

    Thread * getThread(uint32_t id);

    [[nodiscard]] uint32_t getThreadCount() const;

private:
    /**
     * Switches to the given Thread.
     *
     * @param nextThread A Thread
     */
    void dispatch(Thread &nextThread);

    void checkSleepList();

private:

    struct SleepEntry {
        Thread *thread;
        uint32_t wakeupTime;

        bool operator!=(const SleepEntry &other) const;
    };

    Util::Async::Spinlock lock;
    Util::Async::Spinlock sleepLock;

    Util::Data::ArrayListBlockingQueue<Thread*> threadQueue;
    Util::Data::ArrayList<SleepEntry> sleepList;
    Thread *currentThread = nullptr;

    static bool fpuAvailable;
};

}

#endif