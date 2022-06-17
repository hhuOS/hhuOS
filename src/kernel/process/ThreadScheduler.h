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

#ifndef HHUOS_THREADSCHEDULER_H
#define HHUOS_THREADSCHEDULER_H

#include "lib/util/data/ArrayBlockingQueue.h"
#include "lib/util/async/Spinlock.h"
#include "Thread.h"
#include "lib/util/time/Timestamp.h"

namespace Kernel {

class Process;
class ProcessScheduler;

class ThreadScheduler {
    
    friend class ProcessScheduler;

public:
    /**
     * Constructor.
     */
    explicit ThreadScheduler(ProcessScheduler &parent);

    /**
     * Copy Constructor.
     */
    ThreadScheduler(const ThreadScheduler &other) = delete;

    /**
     * Assignment operator.
     */
    ThreadScheduler &operator=(const ThreadScheduler &other) = delete;

    /**
     * Destructor.
     */
    ~ThreadScheduler();

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

    /**
     * Kills a specific Thread.
     *
     * @param thread A Thread
     */
    void kill(Thread &thread);

    void block();

    void unblock(Thread &thread);

    void sleep(const Util::Time::Timestamp &time);

    void killAllThreadsButCurrent();

    /**
     * Returns the activeFlag Thread.
     *
     * @return The activeFlag Thread
     */
    Thread& getCurrentThread();

    Thread &getNextThread();

    [[nodiscard]] uint32_t getThreadCount() const;

private:
    /**
     * Switches to the given Thread.
     *
     * @param next A Thread
     */
    void dispatch(Thread &current, Thread &next);

    void yield(Thread &oldThread, Process &nextProcess, bool force);

    void checkSleepList();

private:

    struct SleepEntry {
        Thread *thread;
        uint32_t wakeupTime;

        bool operator!=(const SleepEntry &other) const;
    };

    ProcessScheduler &parent;
    Util::Async::Spinlock lock;
    Util::Async::Spinlock sleepLock;

    Util::Data::ArrayBlockingQueue<Thread*> threadQueue;
    Util::Data::ArrayList<SleepEntry> sleepList;
    Thread *currentThread = nullptr;

    static bool fpuAvailable;
};

}

#endif