/*
 * Copyright (C) 2018 Burak Akguel, Christian Gesse, Fabian Ruhland, Filip Krakowski, Michael Schoettner
 * Heinrich-Heine University
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

#ifndef __Scheduler_include__
#define __Scheduler_include__

#include "lib/util/ArrayBlockingQueue.h"
#include "kernel/service/InputService.h"
#include "lib/system/priority/PriorityPattern.h"
#include "kernel/thread/Thread.h"
#include "lib/async/Spinlock.h"

namespace Kernel {

class Scheduler {

public:

    explicit Scheduler(PriorityPattern &priority);

    Scheduler(const Scheduler &copy) = delete;

    Scheduler &operator=(const Scheduler &copy) = delete;

    ~Scheduler() = default;

    /**
     * Inidcates if the Scheduler has been initialized.
     *
     * @return true, if the Scheduler has been initialized, false else
     */
    bool isInitialized();

    /**
     * Starts the Scheduler.
     */
    void startUp();

    /**
     * Registers a new Thread.
     *
     * @param that A Thread.
     */
    void ready(Thread &that);

    /**
     * Terminates the current Thread.
     */
    void exit();

    /**
     * Kills a specific Thread.
     *
     * @param that A Thread
     */
    void kill(Thread &that);

    /**
     * Indicates if a Thread is waiting for execution.
     *
     * @return true, if a Thread is waiting, false else
     */
    bool isThreadWaiting();

    /**
     * Unblocks a specific Thread.
     *
     * @param that A Thread
     */
    void deblock(Thread &that);

    /**
     * Returns the active Thread.
     *
     * @return The active Thread
     */
    Thread &getCurrentThread() { return *currentThread; }

    /**
     * Returns the number of active Threads.
     *
     * @return The number of active Threads
     */
    uint32_t getThreadCount();

    uint8_t changePriority(Thread &thread, uint8_t priority);

    Thread *getNextThread();

    uint8_t getMaxPriority();

    /**
     * Returns an instance of the Scheduler.
     *
     * @return An instance of the Scheduler
     */
    static Scheduler &getInstance() noexcept;

    Spinlock lock;

    Util::Array<Util::ArrayBlockingQueue<Thread *>> readyQueues;

private:

    /**
     * Blocks the current Thread.
     */
    void block();

    /**
     * Switches to the next Thread.
     */
    void yield();

    /**
     * Switches to the given Thread.
     *
     * @param next A Thread.
     */
    void dispatch(Thread &next);

private:

    bool initialized = false;

    Thread *currentThread;

    PriorityPattern &priority;

    
};

}

#endif
